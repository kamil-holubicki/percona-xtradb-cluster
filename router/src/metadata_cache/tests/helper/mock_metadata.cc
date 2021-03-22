/*
  Copyright (c) 2016, 2021, Oracle and/or its affiliates.

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

#include "mock_metadata.h"

#include <map>
#include <memory>
#include <vector>

#include "mysqlrouter/metadata_cache.h"

using namespace std;

MockNG::MockNG(const std::string &user, const std::string &password,
               int connect_timeout, int read_timeout, int connection_attempts,
               const mysqlrouter::SSLOptions &ssl_options,
               const bool use_cluster_notifications)
    : GRClusterMetadata(user, password, connect_timeout, read_timeout,
                        connection_attempts, ssl_options,
                        use_cluster_notifications) {
  ms1.mysql_server_uuid = "instance-1";
  ms1.host = "host-1";
  ms1.port = 3306;
  ms1.xport = 33060;
  ms1.mode = metadata_cache::ServerMode::ReadWrite;

  ms2.mysql_server_uuid = "instance-2";
  ms2.host = "host-2";
  ms2.port = 3306;
  ms2.xport = 33060;
  ms2.mode = metadata_cache::ServerMode::ReadOnly;

  ms3.mysql_server_uuid = "instance-3";
  ms3.host = "host-3";
  ms3.port = 3306;
  ms3.xport = 33060;
  ms3.mode = metadata_cache::ServerMode::ReadOnly;

  cluster_instances_vector.push_back(ms1);
  cluster_instances_vector.push_back(ms2);
  cluster_instances_vector.push_back(ms3);

  cluster_info.single_primary_mode = true;
  cluster_info.members = cluster_instances_vector;
}

/** @brief Destructor
 *
 * Disconnect and release the connection to the metadata node.
 */
MockNG::~MockNG() = default;

/** @brief Returns cluster topology information object
 *
 *
 * @return cluster topology information.
 */
metadata_cache::ManagedCluster MockNG::fetch_instances(
    const mysqlrouter::TargetCluster & /*target_cluster*/,
    const string & /*cluster_type_specific_id*/) {
  return cluster_info;
}

metadata_cache::ManagedCluster MockNG::fetch_instances(
    const std::vector<metadata_cache::ManagedInstance> & /*instances*/,
    const string & /*group_replication_id*/, size_t & /*instance_id*/) {
  return cluster_info;
}

/** @brief Mock connect method.
 *
 * Mock connect method, does nothing.
 *
 * @return a boolean to indicate if the connection was successful.
 */
bool MockNG::connect_and_setup_session(
    const metadata_cache::ManagedInstance &metadata_server) noexcept {
  (void)metadata_server;
  return true;
}

/**
 * Mock disconnect method, does nothing.
 */
void MockNG::disconnect() noexcept {}

#if 0  // not used so far
/**
 *
 * Returns a mock refresh interval.
 *
 * @return refresh interval of the Metadata cache.
 */
unsigned int MockNG::fetch_ttl() {
  return 5;
}
#endif
