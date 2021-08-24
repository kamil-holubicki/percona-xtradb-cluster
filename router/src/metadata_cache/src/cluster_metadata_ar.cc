/*
  Copyright (c) 2019, 2021, Oracle and/or its affiliates.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License, version 2.0,
  as published by the Free Software Foundation.

  This program is also distributed with certain software (including
  but not limited to OpenSSL) that is licensed under separate terms,
  as designated in a particular file or component or in included license
  documentation.  The authors of MySQL hereby grant you an additional
  permission to link the program and your derivative works with the
  separately licensed software that they have included with MySQL.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "cluster_metadata_ar.h"

#include "mysql/harness/logging/logging.h"
#include "mysqlrouter/mysql_session.h"
#include "mysqlrouter/utils.h"
#include "mysqlrouter/utils_sqlstring.h"

using mysqlrouter::MySQLSession;
using mysqlrouter::sqlstring;
using mysqlrouter::strtoull_checked;
IMPORT_LOG_FUNCTIONS()

ARClusterMetadata::~ARClusterMetadata() = default;

stdx::expected<metadata_cache::ClusterTopology, std::error_code>
ARClusterMetadata::fetch_cluster_topology(
    const std::atomic<bool> &terminated,
    mysqlrouter::TargetCluster & /*target_cluster*/,
    const unsigned /*router_id*/,
    const metadata_cache::metadata_servers_list_t &metadata_servers,
    bool /* needs_writable_node */, const std::string &cluster_type_specific_id,
    const std::string & /*clusterset_id*/, std::size_t &instance_id) {
  std::vector<metadata_cache::ManagedInstance> new_instances;

  bool metadata_read = false;

  for (size_t i = 0; i < metadata_servers.size(); ++i) {
    if (terminated) {
      return stdx::make_unexpected(make_error_code(
          metadata_cache::metadata_errc::metadata_refresh_terminated));
    }
    const auto &metadata_server = metadata_servers[i];
    try {
      if (!connect_and_setup_session(metadata_server)) {
        continue;
      }

      MySQLSession::Transaction transaction(metadata_connection_.get());

      // throws metadata_cache::metadata_error and
      // MetadataUpgradeInProgressException
      const auto version =
          get_and_check_metadata_schema_version(*metadata_connection_);

      const auto cluster_type =
          mysqlrouter::get_cluster_type(version, metadata_connection_.get());

      if (cluster_type != mysqlrouter::ClusterType::RS_V2) {
        log_error(
            "Invalid cluster type '%s'. Configured '%s'",
            mysqlrouter::to_string(cluster_type).c_str(),
            mysqlrouter::to_string(mysqlrouter::ClusterType::RS_V2).c_str());
        continue;
      }

      uint64_t view_id{0};
      if (!get_member_view_id(*metadata_connection_, cluster_type_specific_id,
                              view_id)) {
        log_warning("Failed fetching view_id from the metadata server on %s:%d",
                    metadata_server.address().c_str(), metadata_server.port());
        continue;
      }

      if (view_id < this->view_id_) {
        continue;
      }

      if (view_id == this->view_id_ && metadata_read) {
        continue;
      }

      new_instances = fetch_instances_from_member(*metadata_connection_,
                                                  cluster_type_specific_id);

      this->view_id_ = view_id;
      metadata_read = true;
      instance_id = i;
    } catch (const mysqlrouter::MetadataUpgradeInProgressException &) {
      throw;
    } catch (const std::exception &e) {
      log_warning("Failed fetching metadata from metadata server on %s:%d - %s",
                  metadata_server.address().c_str(), metadata_server.port(),
                  e.what());
    }
  }

  if (new_instances.empty()) {
    return stdx::make_unexpected(make_error_code(
        metadata_cache::metadata_errc::no_metadata_read_successful));
  }

  metadata_cache::ClusterTopology result;
  result.cluster_data.single_primary_mode = true;
  result.cluster_data.members = std::move(new_instances);
  result.cluster_data.writable_server =
      find_rw_server(result.cluster_data.members);
  result.cluster_data.view_id = this->view_id_;

  // for ReplicaSet Cluster we assume metadata servers are just Cluster nodes
  for (const auto &cluster_node : result.cluster_data.members) {
    result.metadata_servers.push_back({cluster_node.host, cluster_node.port});
  }

  return result;
}

bool ARClusterMetadata::get_member_view_id(mysqlrouter::MySQLSession &session,
                                           const std::string &cluster_id,
                                           uint64_t &result) {
  std::string query =
      "select view_id from mysql_innodb_cluster_metadata.v2_ar_members where "
      "CAST(member_id AS char ascii) = CAST(@@server_uuid AS char ascii)";
  if (!cluster_id.empty()) {
    query += " and cluster_id = " + session.quote(cluster_id);
  }

  std::unique_ptr<MySQLSession::ResultRow> row(session.query_one(query));
  if (!row) {
    return false;
  }

  result = strtoull_checked((*row)[0]);

  return true;
}

// throws metadata_cache::metadata_error
std::vector<metadata_cache::ManagedInstance>
ARClusterMetadata::fetch_instances_from_member(
    mysqlrouter::MySQLSession &session, const std::string &cluster_id) {
  std::vector<metadata_cache::ManagedInstance> result;

  // Get expected topology (what was configured) from metadata server. This will
  // later be compared against current topology (what exists NOW) obtained by
  // comparing to other members view of the world
  std::string query =
      "select M.member_id, I.endpoint, I.xendpoint, M.member_role, "
      "I.attributes from "
      "mysql_innodb_cluster_metadata.v2_ar_members M join "
      "mysql_innodb_cluster_metadata.v2_instances I on I.instance_id = "
      "M.instance_id join mysql_innodb_cluster_metadata.v2_ar_clusters C on "
      "I.cluster_id = C.cluster_id";

  if (!cluster_id.empty()) {
    query += " where C.cluster_id = " + session.quote(cluster_id);
  }

  // example response
  // clang-format off
  // +--------------------------------------+----------------+-----------------+-------------+--------------------------------------------------------------------+
  // | member_id                            | endpoint       | xendpoint       | member_role | attributes                                                         |
  // +--------------------------------------+----------------+-----------------+-------------+--------------------------------------------------------------------+
  // | dc46223b-d620-11e9-9f25-0800276c00e7 | 127.0.0.1:5000 | 127.0.0.1:50000 | PRIMARY     | {"tags": {"_hidden": true}, "joinTime": "2020-03-18 09:36:50.416"} |
  // | f167ceb4-d620-11e9-9155-0800276c00e7 | 127.0.0.1:5001 | 127.0.0.1:50010 | SECONDARY   | {"joinTime": "2020-03-18 09:36:51.000"}                            |
  // | 06c519c9-d621-11e9-9451-0800276c00e7 | 127.0.0.1:5002 | 127.0.0.1:50020 | SECONDARY   | {"joinTime": "2020-03-18 09:36:53.456"}                            |
  // +--------------------------------------+----------------+-----------------+-------------+--------------------------------------------------------------------+
  // clang-format on

  auto result_processor = [&result](const MySQLSession::Row &row) -> bool {
    if (row.size() != 5) {
      throw metadata_cache::metadata_error(
          "Unexpected number of fields in the resultset. "
          "Expected = 5, got = " +
          std::to_string(row.size()));
    }

    metadata_cache::ManagedInstance instance;
    instance.mysql_server_uuid = get_string(row[0]);

    if (!set_instance_ports(instance, row, 1, 2)) {
      return true;  // next row
    }

    instance.mode = get_string(row[3]) == "PRIMARY"
                        ? metadata_cache::ServerMode::ReadWrite
                        : metadata_cache::ServerMode::ReadOnly;

    set_instance_attributes(instance, get_string(row[4]));

    result.push_back(instance);
    return true;  // get next row if available
  };

  assert(session.is_connected());

  try {
    session.query(query, result_processor);
  } catch (const MySQLSession::Error &e) {
    throw metadata_cache::metadata_error(e.what());
  }

  return result;
}
