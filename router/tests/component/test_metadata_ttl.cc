/*
  Copyright (c) 2018, 2020, Oracle and/or its affiliates.

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

#include <chrono>
#include <thread>

#include <gmock/gmock.h>

#include "cluster_metadata.h"
#include "keyring/keyring_manager.h"
#include "mock_server_rest_client.h"
#include "mock_server_testutils.h"
#include "mysql_session.h"
#include "mysqlrouter/rest_client.h"
#include "rest_api_testutils.h"
#include "router_component_test.h"
#include "router_component_testutils.h"
#include "router_config.h"
#include "tcp_port_pool.h"

using mysqlrouter::ClusterType;
using mysqlrouter::MySQLSession;
using ::testing::PrintToString;
using namespace std::chrono_literals;
using namespace std::string_literals;

class MetadataChacheTTLTest : public RouterComponentTest {
 protected:
  std::string get_metadata_cache_section(
      std::vector<uint16_t> metadata_server_ports,
      ClusterType cluster_type = ClusterType::GR_V2,
      const std::string &ttl = "0.5") {
    std::string bootstrap_server_addresses;
    bool use_comma = false;
    for (const auto &port : metadata_server_ports) {
      if (use_comma) {
        bootstrap_server_addresses += ",";
      } else {
        use_comma = true;
      }
      bootstrap_server_addresses += "mysql://localhost:" + std::to_string(port);
    }
    const std::string cluster_type_str =
        (cluster_type == ClusterType::RS_V2) ? "rs" : "gr";

    return "[metadata_cache:test]\n"
           "cluster_type=" +
           cluster_type_str +
           "\n"
           "router_id=1\n"
           "bootstrap_server_addresses=" +
           bootstrap_server_addresses + "\n" +
           "user=mysql_router1_user\n"
           "connect_timeout=1\n"
           "metadata_cluster=test\n" +
           (ttl.empty() ? "" : std::string("ttl=" + ttl + "\n")) + "\n";
  }

  std::string get_metadata_cache_routing_section(
      uint16_t router_port, const std::string &role,
      const std::string &strategy, const std::string &mode = "",
      const std::string &section_name = "default") {
    std::string result =
        "[routing:" + section_name +
        "]\n"
        "bind_port=" +
        std::to_string(router_port) + "\n" +
        "destinations=metadata-cache://test/default?role=" + role + "\n" +
        "protocol=classic\n";

    if (!strategy.empty())
      result += std::string("routing_strategy=" + strategy + "\n");
    if (!mode.empty()) result += std::string("mode=" + mode + "\n");

    return result;
  }

  auto get_array_field_value(const std::string &json_string,
                             const std::string &field_name) {
    std::vector<std::string> result;

    rapidjson::Document json_doc;
    json_doc.Parse(json_string.c_str());
    EXPECT_TRUE(json_doc.HasMember(field_name.c_str()))
        << "json:" << json_string;
    EXPECT_TRUE(json_doc[field_name.c_str()].IsArray()) << json_string;

    auto arr = json_doc[field_name.c_str()].GetArray();
    for (size_t i = 0; i < arr.Size(); ++i) {
      result.push_back(arr[i].GetString());
    }

    return result;
  }

  int get_ttl_queries_count(const std::string &json_string) {
    return get_int_field_value(json_string, "md_query_count");
  }

  int get_update_version_count(const std::string &json_string) {
    return get_int_field_value(json_string, "update_version_count");
  }

  int get_update_last_check_in_count(const std::string &json_string) {
    return get_int_field_value(json_string, "update_last_check_in_count");
  }

  bool wait_for_refresh_thread_started(
      const ProcessWrapper &router, const std::chrono::milliseconds timeout) {
    const std::string needle = "Starting metadata cache refresh thread";

    return wait_log_contains(router, needle, timeout);
  }

  bool wait_metadata_read(const ProcessWrapper &router,
                          const std::chrono::milliseconds timeout) {
    const std::string needle = "Potential changes detected in cluster";

    return wait_log_contains(router, needle, timeout);
  }

  auto &launch_router(const std::string &temp_test_dir,
                      const std::string &conf_dir,
                      const std::string &metadata_cache_section,
                      const std::string &routing_section,
                      const int expected_exitcode,
                      bool wait_for_md_refresh_started = false) {
    auto default_section = get_DEFAULT_defaults();
    init_keyring(default_section, temp_test_dir);

    // launch the router
    const std::string conf_file = create_config_file(
        conf_dir, metadata_cache_section + routing_section, &default_section);
    auto &router = ProcessManager::launch_router(
        {"-c", conf_file}, expected_exitcode, true, false);
    if (wait_for_md_refresh_started) {
      bool ready = wait_for_refresh_thread_started(router, 5000ms);
      EXPECT_TRUE(ready);
    }

    return router;
  }

  TcpPortPool port_pool_;
};

struct MetadataTTLTestParams {
  // trace file
  std::string tracefile;
  // ttl value we want to set (floating point decimal in seconds)
  // additional info about the testcase that gets printed by the gtest in the
  // results
  std::string description;
  ClusterType cluster_type;
  std::string ttl;
  // how long do we run the router and count the metadata queries
  std::chrono::milliseconds router_uptime;
  // how many metadata queries we expect over this period
  int expected_md_queries_count;
  // if true expected_md_queries_count is only a minimal expected
  // value, we should not check for maximum
  bool at_least;

  MetadataTTLTestParams(std::string tracefile_, std::string description_,
                        ClusterType cluster_type_, std::string ttl_,
                        std::chrono::milliseconds router_uptime_ = 0ms,
                        int expected_md_queries_count_ = 0,
                        bool at_least_ = false)
      : tracefile(tracefile_),
        description(description_),
        cluster_type(cluster_type_),
        ttl(ttl_),
        router_uptime(router_uptime_),
        expected_md_queries_count(expected_md_queries_count_),
        at_least(at_least_) {}
};

auto get_test_description(
    const ::testing::TestParamInfo<MetadataTTLTestParams> &info) {
  return info.param.description;
}

std::ostream &operator<<(std::ostream &os, const MetadataTTLTestParams &param) {
  return os << "(" << param.ttl << ", " << param.router_uptime.count() << "ms, "
            << param.expected_md_queries_count << ", " << param.at_least << ")";
}

class MetadataChacheTTLTestParam
    : public MetadataChacheTTLTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

MATCHER_P2(IsBetween, a, b,
           std::string(negation ? "isn't" : "is") + " between " +
               PrintToString(a) + " and " + PrintToString(b)) {
  return a <= arg && arg <= b;
}

TEST_P(MetadataChacheTTLTestParam, CheckTTLValid) {
  auto test_params = GetParam();

  // create and RAII-remove tmp dirs
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");

  SCOPED_TRACE(
      "// launch the server mock (it's our metadata server and single cluster "
      "node)");
  auto md_server_port = port_pool_.get_next_available();
  auto md_server_http_port = port_pool_.get_next_available();
  const std::string json_metadata =
      get_data_dir().join(test_params.tracefile).str();

  auto &metadata_server = launch_mysql_server_mock(
      json_metadata, md_server_port, EXIT_SUCCESS, false, md_server_http_port);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(metadata_server, md_server_port));

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  const auto router_port = port_pool_.get_next_available();
  const std::string metadata_cache_section = get_metadata_cache_section(
      {md_server_port}, test_params.cluster_type, test_params.ttl);
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  auto &router =
      launch_router(temp_test_dir.name(), conf_dir.name(),
                    metadata_cache_section, routing_section, EXIT_SUCCESS,
                    /*wait_for_md_refresh_started=*/true);

  // keep the router running to see how many times it queries for metadata
  std::this_thread::sleep_for(test_params.router_uptime);

  // let's ask the mock how many metadata queries it got after
  std::string server_globals =
      MockServerRestClient(md_server_http_port).get_globals_as_json_string();
  int ttl_count = get_ttl_queries_count(server_globals);

  if (!test_params.at_least) {
    // it is timing based test so to decrease random failures chances let's
    // take some error marigin, we kverify that number of metadata queries
    // falls into <expected_count-1, expected_count+1>
    EXPECT_THAT(ttl_count,
                IsBetween(test_params.expected_md_queries_count - 1,
                          test_params.expected_md_queries_count + 1));
  } else {
    // we only check that the TTL was queried at least N times
    EXPECT_GE(ttl_count, test_params.expected_md_queries_count);
  }

  ASSERT_THAT(router.kill(), testing::Eq(0));
}

INSTANTIATE_TEST_SUITE_P(
    CheckTTLIsUsedCorrectly, MetadataChacheTTLTestParam,
    ::testing::Values(
        MetadataTTLTestParams("metadata_1_node_repeat_v2_gr.js", "0_gr_v2",
                              ClusterType::GR_V2, "0.4",
                              std::chrono::milliseconds(600), 2),
        MetadataTTLTestParams("metadata_1_node_repeat.js", "0_gr",
                              ClusterType::GR_V1, "0.4",
                              std::chrono::milliseconds(600), 2),
        MetadataTTLTestParams("metadata_1_node_repeat_v2_ar.js", "0_ar_v2",
                              ClusterType::RS_V2, "0.4",
                              std::chrono::milliseconds(600), 2),

        MetadataTTLTestParams("metadata_1_node_repeat_v2_gr.js", "1_gr_v2",
                              ClusterType::GR_V2, "1",
                              std::chrono::milliseconds(2500), 3),
        MetadataTTLTestParams("metadata_1_node_repeat.js", "1_gr",
                              ClusterType::GR_V1, "1",
                              std::chrono::milliseconds(2500), 3),
        MetadataTTLTestParams("metadata_1_node_repeat_v2_ar.js", "1_ar_v2",
                              ClusterType::RS_V2, "1",
                              std::chrono::milliseconds(2500), 3),

        // check that default is 0.5 if not provided:
        MetadataTTLTestParams("metadata_1_node_repeat_v2_gr.js", "2_gr_v2",
                              ClusterType::GR_V2, "",
                              std::chrono::milliseconds(1750), 4),
        MetadataTTLTestParams("metadata_1_node_repeat.js", "2_gr",
                              ClusterType::GR_V1, "",
                              std::chrono::milliseconds(1750), 4),
        MetadataTTLTestParams("metadata_1_node_repeat_v2_ar.js", "2_ar_v2",
                              ClusterType::RS_V2, "",
                              std::chrono::milliseconds(1750), 4),

        // check that for 0 there are multiple ttl queries (we can't really
        // guess how many there will be, but we should be able to safely assume
        // that in 1 second it shold be at least 5 queries)
        MetadataTTLTestParams("metadata_1_node_repeat_v2_gr.js", "3_gr_v2",
                              ClusterType::GR_V2, "0",
                              std::chrono::milliseconds(1000), 5,
                              /*at_least=*/true),
        MetadataTTLTestParams("metadata_1_node_repeat.js", "3_gr",
                              ClusterType::GR_V1, "0",
                              std::chrono::milliseconds(1000), 5,
                              /*at_least=*/true),
        MetadataTTLTestParams("metadata_1_node_repeat_v2_ar.js", "3_ar_v2",
                              ClusterType::RS_V2, "0",
                              std::chrono::milliseconds(1000), 5,
                              /*at_least=*/true)),
    get_test_description);

class MetadataChacheTTLTestParamInvalid
    : public MetadataChacheTTLTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

TEST_P(MetadataChacheTTLTestParamInvalid, CheckTTLInvalid) {
  auto test_params = GetParam();

  // create and RAII-remove tmp dirs
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");

  // launch the server mock (it's our metadata server and single cluster node)
  auto md_server_port = port_pool_.get_next_available();
  auto md_server_http_port = port_pool_.get_next_available();
  const std::string json_metadata =
      get_data_dir().join(GetParam().tracefile).str();

  auto &metadata_server = launch_mysql_server_mock(
      json_metadata, md_server_port, false, md_server_http_port);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(metadata_server, md_server_port));

  // launch the router with metadata-cache configuration
  const auto router_port = port_pool_.get_next_available();
  const std::string metadata_cache_section = get_metadata_cache_section(
      {md_server_port}, test_params.cluster_type, test_params.ttl);
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  auto &router =
      launch_router(temp_test_dir.name(), conf_dir.name(),
                    metadata_cache_section, routing_section, EXIT_FAILURE,
                    /*wait_for_md_refresh_started=*/false);

  check_exit_code(router, EXIT_FAILURE);
  EXPECT_THAT(router.exit_code(), testing::Ne(0));
  EXPECT_TRUE(wait_log_contains(router,
                                "Configuration error: option ttl in "
                                "\\[metadata_cache:test\\] needs value "
                                "between 0 and 3600 inclusive",
                                500ms));
}

INSTANTIATE_TEST_SUITE_P(
    CheckInvalidTTLRefusesStart, MetadataChacheTTLTestParamInvalid,
    ::testing::Values(
        MetadataTTLTestParams("metadata_1_node_repeat_gr_v2.js", "0_all",
                              ClusterType::GR_V2, "-0.001"),
        MetadataTTLTestParams("metadata_1_node_repeat_gr_v2.js", "1_all",
                              ClusterType::GR_V2, "3600.001"),
        MetadataTTLTestParams("metadata_1_node_repeat_gr_v2.js", "2_all",
                              ClusterType::GR_V2, "INVALID"),
        MetadataTTLTestParams("metadata_1_node_repeat_gr_v2.js", "3_all",
                              ClusterType::GR_V2, "1,1")),
    get_test_description);

static size_t count_str_occurences(const std::string &s,
                                   const std::string &needle) {
  if (needle.length() == 0) return 0;
  size_t result = 0;
  for (size_t pos = s.find(needle); pos != std::string::npos;) {
    ++result;
    pos = s.find(needle, pos + needle.length());
  }
  return result;
}

class MetadataChacheTTLTestInstanceListUnordered
    : public MetadataChacheTTLTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

/**
 * @test Checks that when for some reason the metadata server starts
 *       returning the information about the cluster nodes in different order we
 *       will not treat this as a change (Bug#29264764).
 */
TEST_P(MetadataChacheTTLTestInstanceListUnordered, InstancesListUnordered) {
  // create and RAII-remove tmp dirs
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");

  const std::string kGroupID = "";

  SCOPED_TRACE("// launch 2 server mocks");
  std::vector<ProcessWrapper *> nodes;
  std::vector<uint16_t> node_classic_ports;
  std::vector<uint16_t> node_http_ports;
  const std::string json_metadata =
      get_data_dir().join(GetParam().tracefile).str();
  for (size_t i = 0; i < 2; ++i) {
    node_classic_ports.push_back(port_pool_.get_next_available());
    node_http_ports.push_back(port_pool_.get_next_available());

    nodes.push_back(
        &launch_mysql_server_mock(json_metadata, node_classic_ports[i],
                                  EXIT_SUCCESS, false, node_http_ports[i]));
    ASSERT_NO_FATAL_FAILURE(check_port_ready(*nodes[i], node_classic_ports[i]));

    ASSERT_TRUE(
        MockServerRestClient(node_http_ports[i]).wait_for_rest_endpoint_ready())
        << nodes[i]->get_full_output();
  }

  for (size_t i = 0; i < 2; ++i) {
    set_mock_metadata(node_http_ports[i], kGroupID, node_classic_ports);
  }

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  const auto router_port = port_pool_.get_next_available();
  const std::string metadata_cache_section = get_metadata_cache_section(
      node_classic_ports, GetParam().cluster_type, GetParam().ttl);
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  auto &router = launch_router(temp_test_dir.name(), conf_dir.name(),
                               metadata_cache_section, routing_section,
                               EXIT_SUCCESS, true);

  EXPECT_TRUE(wait_for_transaction_count_increase(node_http_ports[0]));

  SCOPED_TRACE("// instruct the mocks to return nodes in reverse order");
  std::vector<uint16_t> node_classic_ports_reverse(node_classic_ports.rbegin(),
                                                   node_classic_ports.rend());
  for (size_t i = 0; i < 2; ++i) {
    set_mock_metadata(node_http_ports[i], kGroupID, node_classic_ports_reverse,
                      1);
  }

  EXPECT_TRUE(wait_for_transaction_count_increase(node_http_ports[0]));

  SCOPED_TRACE("// check it is not treated as a change");
  const std::string needle = "Potential changes detected in cluster";
  const std::string log_content = router.get_full_logfile();

  // 1 is expected, that comes from the inital reading of the metadata
  EXPECT_EQ(1, count_str_occurences(log_content, needle)) << log_content;
}

INSTANTIATE_TEST_SUITE_P(
    InstancesListUnordered, MetadataChacheTTLTestInstanceListUnordered,
    ::testing::Values(
        MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                              "unordered_gr_v2", ClusterType::GR_V1, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes.js", "unordered_gr",
                              ClusterType::GR_V2, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                              "unordered_ar_v2", ClusterType::RS_V2, "0.1")),
    get_test_description);

class MetadataChacheTTLTestInvalidMysqlXPort
    : public MetadataChacheTTLTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

/**
 * @test Check that invalid mysqlx port in the metadata does not cause the node
 * to be discarded for the classic protocol connections (Bug#30617645)
 */
TEST_P(MetadataChacheTTLTestInvalidMysqlXPort, InvalidMysqlXPort) {
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");

  const std::string json_metadata =
      get_data_dir().join(GetParam().tracefile).str();

  SCOPED_TRACE("// single node cluster is fine for this test");
  const uint16_t node_classic_port{port_pool_.get_next_available()};
  const uint16_t node_http_port{port_pool_.get_next_available()};
  const uint32_t kInvalidPort{76000};

  auto &cluster_node = launch_mysql_server_mock(
      json_metadata, node_classic_port, EXIT_SUCCESS, false, node_http_port);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(cluster_node, node_classic_port));

  ASSERT_TRUE(
      MockServerRestClient(node_http_port).wait_for_rest_endpoint_ready())
      << cluster_node.get_full_output();

  SCOPED_TRACE(
      "// let the metadata for our single node report invalid mysqlx port");
  set_mock_metadata(node_http_port, "", {node_classic_port}, 0, 0, false,
                    "127.0.0.1", {kInvalidPort});

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  const auto router_port = port_pool_.get_next_available();
  const std::string metadata_cache_section = get_metadata_cache_section(
      {node_classic_port}, GetParam().cluster_type, GetParam().ttl);
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  auto &router = launch_router(temp_test_dir.name(), conf_dir.name(),
                               metadata_cache_section, routing_section,
                               EXIT_SUCCESS, true);

  ASSERT_NO_FATAL_FAILURE(check_port_ready(router, router_port));
  ASSERT_TRUE(wait_metadata_read(router, 5000ms)) << router.get_full_output();

  SCOPED_TRACE(
      "// Even though the metadata contains invalid mysqlx port we still "
      "should be able to connect on the classic port");
  MySQLSession client;
  ASSERT_NO_FATAL_FAILURE(
      client.connect("127.0.0.1", router_port, "username", "password", "", ""));
}

INSTANTIATE_TEST_SUITE_P(
    InvalidMysqlXPort, MetadataChacheTTLTestInvalidMysqlXPort,
    ::testing::Values(MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                                            "gr_v2", ClusterType::GR_V1, "5"),
                      MetadataTTLTestParams("metadata_dynamic_nodes.js", "gr",
                                            ClusterType::GR_V2, "5"),
                      MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                                            "ar_v2", ClusterType::RS_V2, "5")),
    get_test_description);

/**
 * @test Checks that the router operates smoothly when the metadata version has
 * changed between the metadata refreshes.
 */
TEST_F(MetadataChacheTTLTest, CheckMetadataUpgradeBetweenTTLs) {
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");

  SCOPED_TRACE(
      "// launch the server mock (it's our metadata server and single cluster "
      "node)");
  auto md_server_port = port_pool_.get_next_available();
  auto md_server_http_port = port_pool_.get_next_available();
  const std::string json_metadata =
      get_data_dir().join("metadata_1_node_repeat_metadatada_upgrade.js").str();

  auto &metadata_server = launch_mysql_server_mock(
      json_metadata, md_server_port, EXIT_SUCCESS, false, md_server_http_port);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(metadata_server, md_server_port));

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  const auto router_port = port_pool_.get_next_available();

  const std::string metadata_cache_section =
      get_metadata_cache_section({md_server_port}, ClusterType::GR_V1, "0.5");
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  auto &router =
      launch_router(temp_test_dir.name(), conf_dir.name(),
                    metadata_cache_section, routing_section, EXIT_SUCCESS,
                    /*wait_for_md_refresh_started=*/true);

  // keep the router running for a while and change the metadata version
  std::this_thread::sleep_for(1s);

  MockServerRestClient(md_server_http_port)
      .set_globals("{\"new_metadata\" : 1}");

  // let the router run a bit more
  std::this_thread::sleep_for(1s);

  const std::string log_content = router.get_full_logfile();

  SCOPED_TRACE(
      "// check that the router really saw the version upgrade at some point");
  std::string needle =
      "Metadata version change was discovered. New metadata version is 2.0.0";
  EXPECT_GE(1, count_str_occurences(log_content, needle)) << log_content;

  SCOPED_TRACE(
      "// there should be no cluster change reported caused by the version "
      "upgrade");
  needle = "Potential changes detected in cluster";
  // 1 is expected, that comes from the inital reading of the metadata
  EXPECT_EQ(1, count_str_occurences(log_content, needle)) << log_content;

  // router should exit noramlly
  ASSERT_THAT(router.kill(), testing::Eq(0));
}

class CheckRouterVersionUpdateOnceTest
    : public MetadataChacheTTLTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

TEST_P(CheckRouterVersionUpdateOnceTest, CheckRouterVersionUpdateOnce) {
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");

  SCOPED_TRACE(
      "// launch the server mock (it's our metadata server and single cluster "
      "node)");
  auto md_server_port = port_pool_.get_next_available();
  auto md_server_http_port = port_pool_.get_next_available();
  const std::string json_metadata =
      get_data_dir().join(GetParam().tracefile).str();

  auto &metadata_server = launch_mysql_server_mock(
      json_metadata, md_server_port, EXIT_SUCCESS, false, md_server_http_port);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(metadata_server, md_server_port));
  ASSERT_TRUE(
      MockServerRestClient(md_server_http_port).wait_for_rest_endpoint_ready())
      << metadata_server.get_full_output();

  SCOPED_TRACE(
      "// let's tell the mock which version it should expect so that it does "
      "the strict sql matching for us");
  auto globals = mock_GR_metadata_as_json("", {md_server_port});
  JsonAllocator allocator;
  globals.AddMember("router_version", MYSQL_ROUTER_VERSION, allocator);
  const auto globals_str = json_to_string(globals);
  MockServerRestClient(md_server_http_port).set_globals(globals_str);

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  const auto router_port = port_pool_.get_next_available();

  const std::string metadata_cache_section = get_metadata_cache_section(
      {md_server_port}, GetParam().cluster_type, GetParam().ttl);
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  launch_router(temp_test_dir.name(), conf_dir.name(), metadata_cache_section,
                routing_section, EXIT_SUCCESS,
                /*wait_for_md_refresh_started=*/true);

  SCOPED_TRACE("// let the router run for about 10 ttl periods");
  std::this_thread::sleep_for(1s);

  SCOPED_TRACE("// we still expect the version to be only set once");
  std::string server_globals =
      MockServerRestClient(md_server_http_port).get_globals_as_json_string();
  const int version_upd_count = get_update_version_count(server_globals);
  EXPECT_EQ(1, version_upd_count);

  SCOPED_TRACE(
      "// Let's check if the first query is starting a trasaction and the "
      "second checking the version");

  const auto queries = get_array_field_value(server_globals, "queries");
  EXPECT_EQ(4u, queries.size()) << server_globals;

  EXPECT_STREQ(
      "SET @@SESSION.autocommit=1, @@SESSION.character_set_client=utf8, "
      "@@SESSION.character_set_results=utf8, "
      "@@SESSION.character_set_connection=utf8, "
      "@@SESSION.sql_mode='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_"
      "DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION', "
      "@@SESSION.optimizer_switch='derived_merge=on'",
      queries.at(0).c_str());
  EXPECT_STREQ("SET @@SESSION.group_replication_consistency='EVENTUAL'",
               queries.at(1).c_str());
  EXPECT_STREQ("START TRANSACTION", queries.at(2).c_str());
  EXPECT_STREQ("SELECT * FROM mysql_innodb_cluster_metadata.schema_version",
               queries.at(3).c_str());

  if (GetParam().cluster_type != ClusterType::GR_V1) {
    SCOPED_TRACE("// last_check_in should be attempted at least once");
    std::string server_globals =
        MockServerRestClient(md_server_http_port).get_globals_as_json_string();
    const int last_check_in_upd_count =
        get_update_last_check_in_count(server_globals);
    EXPECT_GE(1, last_check_in_upd_count);
  }
}

INSTANTIATE_TEST_SUITE_P(
    CheckRouterVersionUpdateOnce, CheckRouterVersionUpdateOnceTest,
    ::testing::Values(
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update.js",
                              "router_version_update_once_gr_v1",
                              ClusterType::GR_V1, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update_v2_gr.js",
                              "router_version_update_once_gr_v2",
                              ClusterType::GR_V2, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update_v2_ar.js",
                              "router_version_update_once_ar_v2",
                              ClusterType::RS_V2, "0.1")),
    get_test_description);

class PermissionErrorOnVersionUpdateTest
    : public MetadataChacheTTLTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

TEST_P(PermissionErrorOnVersionUpdateTest, PermissionErrorOnVersionUpdate) {
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");

  SCOPED_TRACE(
      "// launch the server mock (it's our metadata server and single cluster "
      "node)");
  auto md_server_port = port_pool_.get_next_available();
  auto md_server_http_port = port_pool_.get_next_available();
  const std::string json_metadata =
      get_data_dir().join(GetParam().tracefile).str();

  auto &metadata_server = launch_mysql_server_mock(
      json_metadata, md_server_port, EXIT_SUCCESS, false, md_server_http_port);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(metadata_server, md_server_port));
  ASSERT_TRUE(
      MockServerRestClient(md_server_http_port).wait_for_rest_endpoint_ready())
      << metadata_server.get_full_output();

  SCOPED_TRACE(
      "// let's tell the mock which version it should expect so that it does "
      "the strict sql matching for us, also tell it to issue the permission "
      "error on the update attempt");
  auto globals = mock_GR_metadata_as_json("", {md_server_port});
  JsonAllocator allocator;
  globals.AddMember("router_version", MYSQL_ROUTER_VERSION, allocator);
  globals.AddMember("perm_error_on_version_update", 1, allocator);
  const auto globals_str = json_to_string(globals);
  MockServerRestClient(md_server_http_port).set_globals(globals_str);

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  const auto router_port = port_pool_.get_next_available();

  const std::string metadata_cache_section = get_metadata_cache_section(
      {md_server_port}, GetParam().cluster_type, GetParam().ttl);
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  auto &router =
      launch_router(temp_test_dir.name(), conf_dir.name(),
                    metadata_cache_section, routing_section, EXIT_SUCCESS,
                    /*wait_for_md_refresh_started=*/true);

  SCOPED_TRACE("// let the router run for about 10 ttl periods");
  std::this_thread::sleep_for(1s);

  SCOPED_TRACE(
      "// we expect the error trying to update the version in the log");
  const std::string log_content = router.get_full_logfile();
  const std::string pattern =
      "Updating the router version in metadata failed:.*\n"
      "Make sure to follow the correct steps to upgrade your metadata.\n"
      "Run the dba.upgradeMetadata\\(\\) then launch the new Router version "
      "when prompted";
  ASSERT_TRUE(pattern_found(log_content, pattern)) << log_content;

  SCOPED_TRACE(
      "// we expect that the router attempted to update the version only once, "
      "even tho it failed");
  std::string server_globals =
      MockServerRestClient(md_server_http_port).get_globals_as_json_string();
  const int version_upd_count = get_update_version_count(server_globals);
  EXPECT_EQ(1, version_upd_count);

  SCOPED_TRACE(
      "// It should still not be fatal, the router should accept the "
      "connections to the cluster");
  MySQLSession client;
  ASSERT_NO_FATAL_FAILURE(
      client.connect("127.0.0.1", router_port, "username", "password", "", ""));
}

INSTANTIATE_TEST_SUITE_P(
    PermissionErrorOnVersionUpdate, PermissionErrorOnVersionUpdateTest,
    ::testing::Values(
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update.js",
                              "router_version_update_fail_on_perm_gr_v1",
                              ClusterType::GR_V1, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update_v2_gr.js",
                              "router_version_update_fail_on_perm_gr_v2",
                              ClusterType::GR_V2, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update_v2_ar.js",
                              "router_version_update_fail_on_perm_ar_v2",
                              ClusterType::RS_V2, "0.1")),
    get_test_description);

class UpgradeInProgressTest
    : public MetadataChacheTTLTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

TEST_P(UpgradeInProgressTest, UpgradeInProgress) {
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");

  SCOPED_TRACE(
      "// launch the server mock (it's our metadata server and single cluster "
      "node)");
  auto md_server_port = port_pool_.get_next_available();
  auto md_server_http_port = port_pool_.get_next_available();
  const std::string json_metadata =
      get_data_dir().join(GetParam().tracefile).str();

  auto &metadata_server = launch_mysql_server_mock(
      json_metadata, md_server_port, EXIT_SUCCESS, false, md_server_http_port);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(metadata_server, md_server_port));
  ASSERT_TRUE(
      MockServerRestClient(md_server_http_port).wait_for_rest_endpoint_ready())
      << metadata_server.get_full_output();
  set_mock_metadata(md_server_http_port, "", {md_server_port});

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  const auto router_port = port_pool_.get_next_available();

  const std::string metadata_cache_section = get_metadata_cache_section(
      {md_server_port}, GetParam().cluster_type, GetParam().ttl);
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  auto &router =
      launch_router(temp_test_dir.name(), conf_dir.name(),
                    metadata_cache_section, routing_section, EXIT_SUCCESS,
                    /*wait_for_md_refresh_started=*/true);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(router, router_port));

  SCOPED_TRACE("// let us make some user connection via the router port");
  MySQLSession client;
  std::this_thread::sleep_for(500ms);
  ASSERT_NO_FATAL_FAILURE(
      client.connect("127.0.0.1", router_port, "username", "password", "", ""));

  SCOPED_TRACE("// let's mimmic start of the metadata update now");
  auto globals = mock_GR_metadata_as_json("", {md_server_port});
  JsonAllocator allocator;
  globals.AddMember("upgrade_in_progress", 1, allocator);
  globals.AddMember("md_query_count", 0, allocator);
  const auto globals_str = json_to_string(globals);
  MockServerRestClient(md_server_http_port).set_globals(globals_str);

  SCOPED_TRACE(
      "// Wait some more and read the metadata update count once more to avoid "
      "race condition.");
  std::this_thread::sleep_for(500ms);
  MockServerRestClient(md_server_http_port).get_globals_as_json_string();
  std::string server_globals =
      MockServerRestClient(md_server_http_port).get_globals_as_json_string();
  int metadata_upd_count = get_ttl_queries_count(server_globals);

  SCOPED_TRACE(
      "// Now wait another 5 ttl periods, since the metadata update is in "
      "progress we do not expect the increased number of metadata queries "
      "after that period");
  std::this_thread::sleep_for(500ms);
  server_globals =
      MockServerRestClient(md_server_http_port).get_globals_as_json_string();
  const int metadata_upd_count2 = get_ttl_queries_count(server_globals);
  EXPECT_EQ(metadata_upd_count, metadata_upd_count2);

  SCOPED_TRACE(
      "// Even though the upgrade is in progress the existing connection "
      "should still be active.");
  auto result{client.query_one("select @@port")};
  EXPECT_EQ(static_cast<uint16_t>(std::stoul(std::string((*result)[0]))),
            md_server_port);

  SCOPED_TRACE("// Also we should be able to create a new conenction.");
  MySQLSession client2;
  ASSERT_NO_FATAL_FAILURE(client2.connect("127.0.0.1", router_port, "username",
                                          "password", "", ""));

  SCOPED_TRACE("// Info about the update should be logged.");
  const std::string log_content = router.get_full_logfile();
  ASSERT_TRUE(log_content.find("Cluster metadata upgrade in progress, aborting "
                               "the metada refresh") != std::string::npos);
}

INSTANTIATE_TEST_SUITE_P(
    UpgradeInProgress, UpgradeInProgressTest,
    ::testing::Values(
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update.js",
                              "metadata_upgrade_in_progress_gr_v1",
                              ClusterType::GR_V1, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update_v2_gr.js",
                              "metadata_upgrade_in_progress_gr_v2",
                              ClusterType::GR_V2, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes_version_update_v2_ar.js",
                              "metadata_upgrade_in_progress_ar_v2",
                              ClusterType::RS_V2, "0.1")),
    get_test_description);

/**
 * @test
 * Verify that when the cluster node returns empty dataset from the
 * v2_this_instance view, the router fails over to the other known nodes to try
 * to read the metadata (BUG#30733189)
 */
class NodeRemovedTest
    : public MetadataChacheTTLTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

TEST_P(NodeRemovedTest, NodeRemoved) {
  TempDirectory temp_test_dir;
  TempDirectory conf_dir("conf");
  const size_t NUM_NODES = 2;
  std::vector<uint16_t> node_ports, node_http_ports;
  std::vector<ProcessWrapper *> cluster_nodes;

  SCOPED_TRACE("// launch cluster with 2 nodes");
  const std::string json_metadata =
      get_data_dir().join(GetParam().tracefile).str();

  for (size_t i = 0; i < NUM_NODES; ++i) {
    node_ports.push_back(port_pool_.get_next_available());
    node_http_ports.push_back(port_pool_.get_next_available());

    cluster_nodes.push_back(&launch_mysql_server_mock(
        json_metadata, node_ports[i], EXIT_SUCCESS, false, node_http_ports[i]));
    ASSERT_NO_FATAL_FAILURE(check_port_ready(*cluster_nodes[i], node_ports[i]));
    ASSERT_TRUE(
        MockServerRestClient(node_http_ports[i]).wait_for_rest_endpoint_ready())
        << cluster_nodes[i]->get_full_output();
    set_mock_metadata(node_http_ports[i], "", node_ports);
  }

  for (size_t i = 0; i < NUM_NODES; ++i) {
    ASSERT_NO_FATAL_FAILURE(check_port_ready(*cluster_nodes[i], node_ports[i]));
    ASSERT_TRUE(
        MockServerRestClient(node_http_ports[i]).wait_for_rest_endpoint_ready())
        << cluster_nodes[i]->get_full_output();
    set_mock_metadata(node_http_ports[i], "", node_ports);
  }

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  const auto router_port = port_pool_.get_next_available();

  const std::string metadata_cache_section = get_metadata_cache_section(
      node_ports, GetParam().cluster_type, GetParam().ttl);
  const std::string routing_section = get_metadata_cache_routing_section(
      router_port, "PRIMARY", "first-available");
  auto &router =
      launch_router(temp_test_dir.name(), conf_dir.name(),
                    metadata_cache_section, routing_section, EXIT_SUCCESS,
                    /*wait_for_md_refresh_started=*/true);
  ASSERT_NO_FATAL_FAILURE(check_port_ready(router, router_port));

  EXPECT_TRUE(wait_for_transaction_count_increase(node_http_ports[0], 2));
  SCOPED_TRACE(
      "// Make a connection to the primary, it should be the first node");
  {
    MySQLSession client;
    ASSERT_NO_FATAL_FAILURE(client.connect("127.0.0.1", router_port, "username",
                                           "password", "", ""));

    auto result{client.query_one("select @@port")};
    EXPECT_EQ(static_cast<uint16_t>(std::stoul(std::string((*result)[0]))),
              node_ports[0]);
  }

  SCOPED_TRACE(
      "// Mimic the removal of the first node, this_instance view on this node "
      "should return empty dataset");
  auto globals = mock_GR_metadata_as_json("", node_ports);
  JsonAllocator allocator;
  globals.AddMember("cluster_type", "", allocator);
  const auto globals_str = json_to_string(globals);
  MockServerRestClient(node_http_ports[0]).set_globals(globals_str);

  SCOPED_TRACE(
      "// Tell the second node that it is a new Primary and the only member of "
      "the cluster");
  set_mock_metadata(node_http_ports[1], "", {node_ports[1]});

  SCOPED_TRACE(
      "// Connect to the router primary port, the connection should be ok and "
      "we should be connected to the new primary now");
  EXPECT_TRUE(wait_for_transaction_count_increase(node_http_ports[1], 2));
  SCOPED_TRACE("// let us make some user connection via the router port");
  {
    MySQLSession client;
    ASSERT_NO_FATAL_FAILURE(client.connect("127.0.0.1", router_port, "username",
                                           "password", "", ""));

    auto result{client.query_one("select @@port")};
    EXPECT_EQ(static_cast<uint16_t>(std::stoul(std::string((*result)[0]))),
              node_ports[1]);
  }
}

INSTANTIATE_TEST_SUITE_P(
    NodeRemoved, NodeRemovedTest,
    ::testing::Values(MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                                            "node_removed_gr_v2",
                                            ClusterType::GR_V2, "0.1"),
                      MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                                            "node_removed_ar_v2",
                                            ClusterType::RS_V2, "0.1")),
    get_test_description);

class NodeHiddenTest : public MetadataChacheTTLTest {
 protected:
  // first node is RW, all others (if any) RO
  void setup_cluster(const size_t nodes_count, const std::string &tracefile,
                     const std::vector<std::string> &nodes_attributes = {}) {
    assert(nodes_count > 0);

    const std::string json_metadata = get_data_dir().join(tracefile).str();

    for (size_t i = 0; i < nodes_count; ++i) {
      // if we are "relaunching" the cluster we want to use the same port as
      // before as router has them in the configuration
      if (node_ports.size() < nodes_count) {
        node_ports.push_back(port_pool_.get_next_available());
        node_http_ports.push_back(port_pool_.get_next_available());
      }

      cluster_nodes.push_back(
          &launch_mysql_server_mock(json_metadata, node_ports[i], EXIT_SUCCESS,
                                    false, node_http_ports[i]));
    }

    for (size_t i = 0; i < nodes_count; ++i) {
      ASSERT_NO_FATAL_FAILURE(
          check_port_ready(*cluster_nodes[i], node_ports[i]));
      ASSERT_TRUE(MockServerRestClient(node_http_ports[i])
                      .wait_for_rest_endpoint_ready());
      set_mock_metadata(node_http_ports[i], "", node_ports, 0, 0, false,
                        "127.0.0.1", {}, nodes_attributes);
    }
  }

  void setup_router(ClusterType cluster_type, const std::string &ttl) {
    router_rw_port = port_pool_.get_next_available();
    router_ro_port = port_pool_.get_next_available();

    const std::string metadata_cache_section =
        get_metadata_cache_section(node_ports, cluster_type, ttl);
    const std::string routing_rw_section = get_metadata_cache_routing_section(
        router_rw_port, "PRIMARY", "first-available", "", "rw");
    const std::string routing_ro_section = get_metadata_cache_routing_section(
        router_ro_port, "SECONDARY", "round-robin", "", "ro");
    router = &launch_router(
        temp_test_dir.name(), conf_dir.name(), metadata_cache_section,
        routing_rw_section + routing_ro_section, EXIT_SUCCESS,
        /*wait_for_md_refresh_started=*/true);
    ASSERT_NO_FATAL_FAILURE(check_port_ready(*router, router_rw_port));

    EXPECT_TRUE(wait_for_transaction_count_increase(node_http_ports[0], 2));
  }

  void set_nodes_attributes(const std::vector<std::string> &nodes_attributes) {
    set_mock_metadata(node_http_ports[0], "", node_ports, 0, 0, false,
                      "127.0.0.1", {}, nodes_attributes);
    EXPECT_TRUE(wait_for_transaction_count_increase(node_http_ports[0], 3));
  }

  std::unique_ptr<MySQLSession> make_new_connection_ok(
      uint16_t router_port, uint16_t expected_node_port) {
    std::unique_ptr<MySQLSession> session{std::make_unique<MySQLSession>()};
    EXPECT_NO_THROW(session->connect("127.0.0.1", router_port, "username",
                                     "password", "", ""));

    auto result{session->query_one("select @@port")};
    EXPECT_EQ(static_cast<uint16_t>(std::stoul(std::string((*result)[0]))),
              expected_node_port);

    return session;
  }

  void verify_new_connection_fails(uint16_t router_port) {
    MySQLSession session;
    ASSERT_ANY_THROW(session.connect("127.0.0.1", router_port, "username",
                                     "password", "", ""));
  }

  void verify_existing_connection_ok(MySQLSession *session,
                                     uint16_t expected_node) {
    auto result{session->query_one("select @@port")};
    EXPECT_EQ(static_cast<uint16_t>(std::stoul(std::string((*result)[0]))),
              expected_node);
  }

  void verify_existing_connection_dropped(MySQLSession *session) {
    ASSERT_ANY_THROW(session->query_one("select @@port"));
  }

  std::vector<uint16_t> node_ports, node_http_ports;
  std::vector<ProcessWrapper *> cluster_nodes;
  ProcessWrapper *router;

  uint16_t router_rw_port, router_ro_port;

 private:
  TempDirectory temp_test_dir;
  TempDirectory conf_dir{"conf"};
};

/**
 * @test Verifies that setting the _hidden tags in the metadata for the node is
 * handled as expected by the Router.
 *
 * TS_FR02_01, TS_FR02_02, TS_FR02_04
 */
class RWNodeHiddenTest
    : public NodeHiddenTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

TEST_P(RWNodeHiddenTest, RWNodeHidden) {
  SCOPED_TRACE("// launch cluster with 3 nodes, 1 RW/2 RO");
  setup_cluster(3, GetParam().tracefile);

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  setup_router(GetParam().cluster_type, GetParam().ttl);

  SCOPED_TRACE("// Make rw connection, should be ok");
  auto rw_con_1 = make_new_connection_ok(router_rw_port, node_ports[0]);

  SCOPED_TRACE(
      "// Configure RW node to hidden=true, "
      "disconnect_existing_sessions_when_hidden stays default which is 'true'");
  set_nodes_attributes({R"({"tags" : {"_hidden": true} })", "", ""});

  SCOPED_TRACE("// The existing connection should get dropped");
  verify_existing_connection_dropped(rw_con_1.get());
  SCOPED_TRACE("// Making new connection should not be possible");
  verify_new_connection_fails(router_rw_port);

  SCOPED_TRACE("// Configure RW node back to hidden=false");
  set_nodes_attributes({R"({"tags" : {"_hidden": false} })", "", ""});
  SCOPED_TRACE("// Making new connection should be possible again");
  auto rw_con_2 = make_new_connection_ok(router_rw_port, node_ports[0]);

  SCOPED_TRACE("// Configure RW node again to hidden=true");
  set_nodes_attributes({R"({"tags" : {"_hidden": true} })", "", ""});
  SCOPED_TRACE("// The existing connection should get dropped");
  verify_existing_connection_dropped(rw_con_2.get());
  SCOPED_TRACE("// Making new connection should not be possible");
  verify_new_connection_fails(router_rw_port);

  SCOPED_TRACE("// Configure RW node back to hidden=false");
  set_nodes_attributes({R"({"tags" : {"_hidden": false} })", "", ""});
  SCOPED_TRACE("// Making new connection should be possible again");
  /*auto rw_con_3 =*/make_new_connection_ok(router_rw_port, node_ports[0]);
}

INSTANTIATE_TEST_SUITE_P(
    RWNodeHidden, RWNodeHiddenTest,
    ::testing::Values(MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                                            "rw_hidden_gr_v2",
                                            ClusterType::GR_V2, "0.1"),
                      MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                                            "rw_hidden_ar_v2",
                                            ClusterType::RS_V2, "0.1")),
    get_test_description);

/**
 * @test Verifies that setting the _disconnect_existing_sessions_when_hidden
 *       tags back and forth in the metadata for the node is handled as expected
 *        by the Router.
 *
 *  TS_FR02_03, TS_FR04_01
 */
class RWNodeHiddenDontDisconnectToggleTest
    : public NodeHiddenTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

TEST_P(RWNodeHiddenDontDisconnectToggleTest, RWNodeHiddenDontDisconnectToggle) {
  SCOPED_TRACE("// launch cluster with 3 nodes, 1 RW/2 RO");
  setup_cluster(3, GetParam().tracefile);

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  setup_router(GetParam().cluster_type, GetParam().ttl);

  // test tags: {hidden, disconnect}
  {
    SCOPED_TRACE("// Make rw connection, should be ok");
    auto rw_con_1 = make_new_connection_ok(router_rw_port, node_ports[0]);

    SCOPED_TRACE(
        "// Configure the first RW node to hidden=true, "
        "set disconnect_existing_sessions_when_hidden stays default which is "
        "true");
    set_nodes_attributes({R"({"tags" : {"_hidden": true} })", "", ""});

    SCOPED_TRACE("// The connection should get dropped");
    verify_existing_connection_dropped(rw_con_1.get());
  }

  // reset test (clear hidden flag)
  {
    SCOPED_TRACE(
        "// Unhide the node, "
        "set disconnect_existing_sessions_when_hidden to false");
    set_nodes_attributes(
        {R"({"tags" : {"_hidden": false, "_disconnect_existing_sessions_when_hidden": false} })",
         "", ""});
  }

  // test tags: {hidden}, then {hidden, disconnect}
  {
    // test tags: {hidden}

    SCOPED_TRACE("// Make rw connection, should be ok");
    auto rw_con_2 = make_new_connection_ok(router_rw_port, node_ports[0]);

    SCOPED_TRACE(
        "// Now configure the first RW node to hidden=true, "
        "disconnect_existing_sessions_when_hidden stays false");
    set_nodes_attributes(
        {R"({"tags" : {"_hidden": true, "_disconnect_existing_sessions_when_hidden": false} })",
         "", ""});

    SCOPED_TRACE("// The existing connection should be ok");
    verify_existing_connection_ok(rw_con_2.get(), node_ports[0]);

    // reset test (clear hidden flag); connection should still be alive
    // therefore we can reuse it for the next test
    SCOPED_TRACE("// Set disconnect_existing_sessions_when_hidden=true");
    set_nodes_attributes(
        {R"({"tags" : {"_disconnect_existing_sessions_when_hidden": true} })",
         "", ""});

    // test tags: {hidden, disconnect}

    SCOPED_TRACE("// And also _hidden=true");
    set_nodes_attributes(
        {R"({"tags" : {"_hidden": true, "_disconnect_existing_sessions_when_hidden": true} })",
         "", ""});

    SCOPED_TRACE("// The connection should get dropped");
    verify_existing_connection_dropped(rw_con_2.get());
  }

  // reset test (clear hidden flag)
  {
    SCOPED_TRACE(
        "// Unhide the node and et disconnect_existing_sessions_when_hidden to "
        "false");
    set_nodes_attributes(
        {R"({"tags" : {"_hidden": false, "_disconnect_existing_sessions_when_hidden": false })",
         "", ""});
  }

  // test tags: {hidden}
  {
    SCOPED_TRACE("// Make rw connection, should be ok");
    auto rw_con_3 = make_new_connection_ok(router_rw_port, node_ports[0]);

    SCOPED_TRACE("// Hide the node again");
    set_nodes_attributes(
        {R"({"tags" : {"_hidden": true, "_disconnect_existing_sessions_when_hidden": false })",
         "", ""});

    SCOPED_TRACE("// The existing connection should be ok");
    verify_existing_connection_ok(rw_con_3.get(), node_ports[0]);
  }
}

INSTANTIATE_TEST_SUITE_P(
    RWNodeHiddenDontDisconnectToggle, RWNodeHiddenDontDisconnectToggleTest,
    ::testing::Values(
        MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                              "rw_hidden_dont_disconnect_toggle_gr_v2",
                              ClusterType::GR_V2, "0.1"),
        MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                              "rw_hidden_dont_disconnect_toggle_ar_v2",
                              ClusterType::RS_V2, "0.1")),
    get_test_description);

class RWNodeHideThenDisconnectTest
    : public NodeHiddenTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

/**
 * @test Verify _disconnect_existing_sessions_when_hidden also works when
 * applied AFTER hiding
 *
 * TS_FR04_02
 * */
TEST_P(RWNodeHideThenDisconnectTest, RWNodeHideThenDisconnect) {
  SCOPED_TRACE("// launch cluster with 3 nodes, 1 RW/2 RO");
  setup_cluster(3, GetParam().tracefile);

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  setup_router(GetParam().cluster_type, GetParam().ttl);

  SCOPED_TRACE("// Make rw connection, should be ok");
  auto rw_con_1 = make_new_connection_ok(router_rw_port, node_ports[0]);

  SCOPED_TRACE("// Set disconnect_existing_sessions_when_hidden=false");
  set_nodes_attributes(
      {R"({"tags" : {"_disconnect_existing_sessions_when_hidden": false} })",
       "", ""});
  SCOPED_TRACE("// Then also set hidden=true");
  set_nodes_attributes(
      {R"({"tags" : {"_hidden": true, "_disconnect_existing_sessions_when_hidden": false} })",
       "", ""});

  SCOPED_TRACE("// The existing connection should stay ok");
  verify_existing_connection_ok(rw_con_1.get(), node_ports[0]);

  SCOPED_TRACE(
      "// Now disconnect_existing_sessions_when_hidden also gets set to true");
  set_nodes_attributes(
      {R"({"tags" : {"_hidden": true, "_disconnect_existing_sessions_when_hidden": true} })",
       "", ""});

  SCOPED_TRACE("// The existing connection should be disconnected");
  verify_existing_connection_dropped(rw_con_1.get());
}

INSTANTIATE_TEST_SUITE_P(
    RWNodeHideThenDisconnect, RWNodeHideThenDisconnectTest,
    ::testing::Values(MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                                            "rw_hide_then_disconnect_gr_v2",
                                            ClusterType::GR_V2, "0.1"),
                      MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                                            "rw_hide_then_disconnect_ar_v2",
                                            ClusterType::RS_V2, "0.1")),
    get_test_description);

/**
 * @test Verify _hidden works well with round-robin
 *
 * TS_FR02_05
 */
class RORoundRobinNodeHiddenTest
    : public NodeHiddenTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {};

TEST_P(RORoundRobinNodeHiddenTest, RORoundRobinNodeHidden) {
  SCOPED_TRACE("// launch cluster with 3 nodes, 1 RW/2 RO");
  setup_cluster(3, GetParam().tracefile);

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  setup_router(GetParam().cluster_type, GetParam().ttl);

  SCOPED_TRACE(
      "// Make one rw connection to check it's not affected by the RO being "
      "hidden");
  auto rw_con_1 = make_new_connection_ok(router_rw_port, node_ports[0]);

  SCOPED_TRACE("// Make ro connection, should be ok and go to the first RO");
  auto ro_con_1 = make_new_connection_ok(router_ro_port, node_ports[1]);

  SCOPED_TRACE("// Configure first RO node to be hidden");
  set_nodes_attributes({"", R"({"tags" : {"_hidden": true} })", ""});

  SCOPED_TRACE("// The existing connection should get dropped");
  verify_existing_connection_dropped(ro_con_1.get());

  SCOPED_TRACE(
      "// Make 2 new connections, both should go to the second RO node");
  auto ro_con_2 = make_new_connection_ok(router_ro_port, node_ports[2]);
  auto ro_con_3 = make_new_connection_ok(router_ro_port, node_ports[2]);

  SCOPED_TRACE("// Now hide also the second RO node");
  set_nodes_attributes({"", R"({"tags" : {"_hidden": true} })",
                        R"({"tags" : {"_hidden": true} })"});
  SCOPED_TRACE("// Both connections to that node should get dropped");
  verify_existing_connection_dropped(ro_con_2.get());
  verify_existing_connection_dropped(ro_con_3.get());
  SCOPED_TRACE(
      "// Since both RO nodes are hidden no new connection to RO port should "
      "be possible");
  verify_new_connection_fails(router_ro_port);

  SCOPED_TRACE("// Unhide the first RO node now");
  set_nodes_attributes({"", "", R"({"tags" : {"_hidden": true} })"});

  SCOPED_TRACE(
      "// Make 2 new connections, both should go to the first RO node this "
      "time");
  /*auto ro_con_4 =*/make_new_connection_ok(router_ro_port, node_ports[1]);
  /*auto ro_con_5 =*/make_new_connection_ok(router_ro_port, node_ports[1]);

  SCOPED_TRACE("// Unhide also the second RO node now");
  set_nodes_attributes({"", "", ""});

  SCOPED_TRACE(
      "// Make more connections to the RO port, they should be assinged in a "
      "round robin fashion as no node is hidden");
  /*auto ro_con_6 =*/make_new_connection_ok(router_ro_port, node_ports[1]);
  /*auto ro_con_7 =*/make_new_connection_ok(router_ro_port, node_ports[2]);
  /*auto ro_con_8 =*/make_new_connection_ok(router_ro_port, node_ports[1]);

  SCOPED_TRACE(
      "// RW connection that we made at the beginning should survive all of "
      "that");
  verify_existing_connection_ok(rw_con_1.get(), node_ports[0]);
}

INSTANTIATE_TEST_SUITE_P(
    RORoundRobinNodeHidden, RORoundRobinNodeHiddenTest,
    ::testing::Values(MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                                            "ro_round_robin_hidden_gr_v2",
                                            ClusterType::GR_V2, "0.1"),
                      MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                                            "ro_round_robin_hidden_ar_v2",
                                            ClusterType::RS_V2, "0.1")),
    get_test_description);

class OneNodeClusterHiddenTest
    : public NodeHiddenTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {
 protected:
  void kill_server(ProcessWrapper *server) { EXPECT_NO_THROW(server->kill()); }
};

/**
 * @test Verify _hidden works fine with one node cluster and after the node
 * resurrection
 *
 * TS_FR02_06, TS_FR02_07
 */
TEST_P(OneNodeClusterHiddenTest, OneNodeClusterHidden) {
  SCOPED_TRACE("// launch one node cluster (single RW node)");
  setup_cluster(1, GetParam().tracefile);

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  setup_router(GetParam().cluster_type, GetParam().ttl);

  SCOPED_TRACE("// Hide the single node that we have");
  set_nodes_attributes({R"({"tags" : {"_hidden": true} })"});

  verify_new_connection_fails(router_rw_port);

  SCOPED_TRACE(
      "// Check that hiding also works after node dissapearing and getting "
      "back");
  kill_server(cluster_nodes[0]);

  SCOPED_TRACE(
      "// Relaunch the node, set the node as hidden from the very start");
  setup_cluster(1, GetParam().tracefile, {R"({"tags" : {"_hidden": true} })"});

  SCOPED_TRACE("// We still should not be able to connect");
  verify_new_connection_fails(router_rw_port);

  SCOPED_TRACE("// Un-hide the node");
  set_nodes_attributes({R"({"tags" : {"_hidden": false} })"});

  SCOPED_TRACE("// Now we should be able to connect");
  make_new_connection_ok(router_rw_port, node_ports[0]);
}

INSTANTIATE_TEST_SUITE_P(
    OneNodeClusterHidden, OneNodeClusterHiddenTest,
    ::testing::Values(MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                                            "one_node_cluster_hidden_gr_v2",
                                            ClusterType::GR_V2, "0.1"),
                      MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                                            "one_node_cluster_hidden_ar_v2",
                                            ClusterType::RS_V2, "0.1")),
    get_test_description);

class InvalidAttributesTagsTest
    : public NodeHiddenTest,
      public ::testing::WithParamInterface<MetadataTTLTestParams> {
 protected:
  void check_log_contains(const std::string &expected_string,
                          size_t expected_occurences) {
    const std::string log_content = router->get_full_logfile();
    EXPECT_EQ(expected_occurences,
              count_str_occurences(log_content, expected_string))
        << log_content;
  }
};

/**
 * @test Checks that the router logs a proper warning once when the attributes
 * for the node becomes invalid.
 *
 * The test covers the following scenarios from the test plan (plus add some
 * more cases):
 * TS_log_parse_error_01 TS_log_parse_error_02
 */
TEST_P(InvalidAttributesTagsTest, InvalidAttributesTags) {
  SCOPED_TRACE("// launch cluster with 1 RW node");
  setup_cluster(1, GetParam().tracefile);

  SCOPED_TRACE("// launch the router with metadata-cache configuration");
  setup_router(GetParam().cluster_type, GetParam().ttl);

  SCOPED_TRACE("// Set the node's attributes to invalid JSON");
  set_nodes_attributes({"not a valid json for sure [] (}", ""});

  SCOPED_TRACE("// Check the expected warnings were logged once");
  check_log_contains(
      "Error parsing _hidden from attributes JSON string: not a valid JSON "
      "object",
      1);
  check_log_contains(
      "Error parsing _disconnect_existing_sessions_when_hidden from attributes "
      "JSON string: not a valid JSON object",
      1);

  SCOPED_TRACE("// Set the node's attributes.tags to invalid JSON");
  set_nodes_attributes({R"({"tags" : false})"});

  SCOPED_TRACE("// Check the expected warnings were logged once");
  check_log_contains(
      "Error parsing _hidden from attributes JSON string: tags - not a valid "
      "JSON object",
      1);
  check_log_contains(
      "Error parsing _disconnect_existing_sessions_when_hidden from attributes "
      "JSON string: tags - not a valid JSON object",
      1);

  SCOPED_TRACE("// Set the attributes.tags to be invalid types");
  set_nodes_attributes(
      {R"({"tags" : { "_hidden" : [], "_disconnect_existing_sessions_when_hidden": "True" }})"});

  SCOPED_TRACE("// Check the expected warnings were logged once");
  check_log_contains(
      "Error parsing _hidden from attributes JSON string: tags._hidden not a "
      "boolean",
      1);
  check_log_contains(
      "Error parsing _disconnect_existing_sessions_when_hidden from attributes "
      "JSON string: tags._disconnect_existing_sessions_when_hidden not a "
      "boolean",
      1);

  SCOPED_TRACE(
      "// Now fix both _hidden and _disconnect_existing_sessions_when_hidden "
      "in the metadata");
  set_nodes_attributes(
      {R"({"tags": { "_hidden" : false, "_disconnect_existing_sessions_when_hidden": false } })"});

  SCOPED_TRACE(
      "// Check the expected warnings about the attributes been valid were "
      "logged once");
  check_log_contains("Successfully parsed _hidden from attributes JSON string",
                     1);
  check_log_contains(
      "Successfully parsed _disconnect_existing_sessions_when_hidden from "
      "attributes JSON string",
      1);

  SCOPED_TRACE("// Set the attributes.tags to be invalid types again");
  set_nodes_attributes(
      {R"({"tags" : { "_hidden" : [], "_disconnect_existing_sessions_when_hidden": "True" }})"});

  SCOPED_TRACE("// Check the expected warnings were logged twice");
  check_log_contains(
      "Error parsing _hidden from attributes JSON string: tags._hidden not a "
      "boolean",
      2);
  check_log_contains(
      "Error parsing _disconnect_existing_sessions_when_hidden from attributes "
      "JSON string: tags._disconnect_existing_sessions_when_hidden not a "
      "boolean",
      2);
}

INSTANTIATE_TEST_SUITE_P(
    InvalidAttributesTags, InvalidAttributesTagsTest,
    ::testing::Values(MetadataTTLTestParams("metadata_dynamic_nodes_v2_gr.js",
                                            "invalid_attributes_tags_gr_v2",
                                            ClusterType::GR_V2, "0.1"),
                      MetadataTTLTestParams("metadata_dynamic_nodes_v2_ar.js",
                                            "invalid_attributes_tags_ar_v2",
                                            ClusterType::RS_V2, "0.1")),
    get_test_description);

int main(int argc, char *argv[]) {
  init_windows_sockets();
  ProcessManager::set_origin(Path(argv[0]).dirname());
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
