/*
  Copyright (c) 2017, 2021, Oracle and/or its affiliates.

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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mysql_session.h"
#include "mysqlxclient.h"
#include "router_component_test.h"
#include "router_config.h"
#include "tcp_port_pool.h"

using namespace std::chrono_literals;
using namespace std::string_literals;

struct MockServerCLITestParam {
  const char *test_name;

  std::vector<std::string> cmdline_args;

  int expected_exit_code;

  std::function<void(const std::string &output)> checker;
};

class MockServerCLITest
    : public RouterComponentTest,
      public ::testing::WithParamInterface<MockServerCLITestParam> {};

TEST_P(MockServerCLITest, check) {
  auto mysql_server_mock_path = get_mysqlserver_mock_exec().str();

  ASSERT_THAT(mysql_server_mock_path, ::testing::StrNe(""));

  SCOPED_TRACE("// start binary");
  auto &cmd = launch_command(mysql_server_mock_path, GetParam().cmdline_args,
                             GetParam().expected_exit_code, true);

  SCOPED_TRACE("// wait for exit");
  check_exit_code(cmd, GetParam().expected_exit_code,
                  1000ms);  // should be quick, and return 0
  SCOPED_TRACE("// checking stdout");

  GetParam().checker(cmd.get_full_output());
}

const MockServerCLITestParam mock_server_cli_test_param[] = {
    {"version",
     // ensure mock-server supports --version.
     // WL#12118::TS_1-3
     {"--version"},
     EXIT_SUCCESS,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr(MYSQL_ROUTER_VERSION));
     }},
    {"help",
     // ensure mock-server supports --help.
     {"--help"},
     EXIT_SUCCESS,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("--version"));
     }},
    {"invalid_port",
     // ensure mock-server fails with --http-port=65536.
     // WL#12118::TS_1-4
     {"--http-port", "65536"},
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("was '65536'"));
     }},
    {"hex_http_port",
     // ensure mock-server fails with --http-port=0xffff.
     {"--http-port", "0xffff",  //
      "--filename", "@filename@"},
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("was '0xffff'"));
     }},
    {"hex_xport",
     // ensure mock-server fails with --xport=0xffff.
     {"--port", "0xffff",  //
      "--filename", "@filename@"},
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("was '0xffff'"));
     }},
    {"hex_port",
     // ensure mock-server fails with --port=0xffff.
     {"--xport", "0xffff",  //
      "--filename", "@filename@"},
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("was '0xffff'"));
     }},
    {"invalid_ssl_mode",
     {"--filename", "@filename@",  //
      "--ssl-mode", "verify_ca"},
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("invalid value 'verify_ca'"));
     }},
    {"ssl_mode_required_no_cert_no_key",
     {
         "--filename", "@filename@",  //
         "--ssl-mode", "required",    //
     },
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("ssl_cert is empty"));
     }},
    {"ssl_mode_required_no_key",
     {
         "--filename", "@filename@",                 //
         "--ssl-mode", "required",                   //
         "--ssl-cert", "@datadir@/server-cert.pem",  //
     },
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("ssl_key is empty"));
     }},
    {"ssl_mode_required_no_cert",
     {
         "--filename", "@filename@",               //
         "--ssl-mode", "required",                 //
         "--ssl-key", "@datadir@/server-key.pem",  //
     },
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("ssl_cert is empty"));
     }},
    {"ssl_mode_preferred_no_cert_no_key",
     {
         "--filename", "@filename@",  //
         "--ssl-mode", "preferred",   //
     },
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("ssl_cert is empty"));
     }},
    {"ssl_mode_preferred_no_key",
     {
         "--filename", "@filename@",                 //
         "--ssl-mode", "preferred",                  //
         "--ssl-cert", "@datadir@/server-cert.pem",  //
     },
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("ssl_key is empty"));
     }},
    {"ssl_mode_preferred_no_cert",
     {
         "--filename", "@filename@",               //
         "--ssl-mode", "preferred",                //
         "--ssl-key", "@datadir@/server-key.pem",  //
     },
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output, ::testing::HasSubstr("ssl_cert is empty"));
     }},
    {"tls_version_unknown",
     {
         "--filename", "@filename@",                 // required
         "--ssl-mode", "preferred",                  // required
         "--ssl-key", "@datadir@/server-key.pem",    // required
         "--ssl-cert", "@datadir@/server-cert.pem",  // required
         "--tls-version", "sslv3",                   //
     },
     EXIT_FAILURE,
     [](const std::string &output) {
       EXPECT_THAT(output,
                   ::testing::HasSubstr("setting 'tls_version=sslv3' failed"));
     }},
};

INSTANTIATE_TEST_CASE_P(Spec, MockServerCLITest,
                        ::testing::ValuesIn(mock_server_cli_test_param),
                        [](const auto &info) { return info.param.test_name; });

class MockServerCLITestBase : public RouterComponentTest {};

struct MockServerConnectOkTestParam {
  const char *test_name;

  std::vector<std::string> cmdline_args;
};

class MockServerConnectOkTest
    : public RouterComponentTest,
      public ::testing::WithParamInterface<MockServerConnectOkTestParam> {
 protected:
  TcpPortPool port_pool_;
};

/**
 * replace named placeholders in a string.
 *
 * format: voo@key@bar
 */
static std::string replace_placeholders(
    const std::string &arg, const std::map<std::string, std::string> &config) {
  std::string value;

  auto cur = arg.begin();
  auto end = arg.end();

  do {
    // find pairs of @
    auto first_at = std::find(cur, end, '@');
    if (first_at == end) {
      break;
    }

    auto second_at = std::find(first_at + 1, end, '@');
    if (second_at == end) {
      throw std::runtime_error("expected 2nd @.");
    }

    value += std::string(cur, first_at);

    auto lookup = std::string(first_at + 1, second_at);

    value += config.at(lookup);
    cur = second_at + 1;
  } while (true);

  value += std::string(cur, end);

  return value;
}

static void classic_protocol_connect_ok(
    const std::string &host, uint16_t port,
    const std::string &username = "someuser",
    const std::string &password = "somepass") {
  mysqlrouter::MySQLSession sess;

  sess.connect(host, port,
               username,  // user
               password,  // pass
               "",        // socket
               ""         // schema
  );
}

static void classic_protocol_connect_fail(const std::string &host,
                                          uint16_t port,
                                          const std::string &username,
                                          const std::string &password,
                                          int expected_error_code) {
  mysqlrouter::MySQLSession sess;
  try {
    sess.connect(host, port,
                 username,  // user
                 password,  // pass
                 "",        // socket
                 ""         // schema
    );
    FAIL() << "expected to fail";
  } catch (const mysqlrouter::MySQLSession::Error &e) {
    ASSERT_EQ(e.code(), expected_error_code);
  }
}

static void x_protocol_connect_ok(const std::string &host, uint16_t port,
                                  const std::string &username = "someuser",
                                  const std::string &password = "somepass") {
  auto sess = xcl::create_session();
  ASSERT_THAT(
      sess->set_mysql_option(
          xcl::XSession::Mysqlx_option::Ssl_mode,
          mysqlrouter::MySQLSession::ssl_mode_to_string(SSL_MODE_PREFERRED)),
      ::testing::Truly([](const xcl::XError &xerr) { return !xerr; }));
  ASSERT_THAT(
      sess->connect(host.c_str(), port, username.c_str(), password.c_str(), ""),
      ::testing::Truly([](auto const &err) { return err.error() == 0; }));
}

TEST_P(MockServerConnectOkTest, classic_protocol) {
  auto mysql_server_mock_path = get_mysqlserver_mock_exec().str();
  auto bind_port = port_pool_.get_next_available();
  ASSERT_THAT(mysql_server_mock_path, ::testing::StrNe(""));

  std::map<std::string, std::string> config{
      {"http_port", std::to_string(port_pool_.get_next_available())},
      {"datadir", get_data_dir().str()},
      {"certdir", SSL_TEST_DATA_DIR},
      {"hostname", "127.0.0.1"},
  };

  std::vector<std::string> cmdline_args;

  for (const auto &arg : GetParam().cmdline_args) {
    if (arg.empty()) {
      cmdline_args.push_back(arg);
    } else {
      cmdline_args.push_back(replace_placeholders(arg, config));
    }
  }

  cmdline_args.emplace_back("--port");
  cmdline_args.push_back(std::to_string(bind_port));

  SCOPED_TRACE("// start binary");
  launch_command(mysql_server_mock_path, cmdline_args, EXIT_SUCCESS, true,
                 1000ms);

  SCOPED_TRACE("// checking "s + GetParam().test_name);
  classic_protocol_connect_ok(config.at("hostname"), bind_port);
}

TEST_P(MockServerConnectOkTest, x_protocol) {
  auto mysql_server_mock_path = get_mysqlserver_mock_exec().str();
  auto bind_port = port_pool_.get_next_available();
  ASSERT_THAT(mysql_server_mock_path, ::testing::StrNe(""));

  std::map<std::string, std::string> config{
      {"http_port", std::to_string(port_pool_.get_next_available())},
      {"datadir", get_data_dir().str()},
      {"certdir", SSL_TEST_DATA_DIR},
      {"hostname", "127.0.0.1"},
  };

  std::vector<std::string> cmdline_args;

  for (const auto &arg : GetParam().cmdline_args) {
    if (arg.empty()) {
      cmdline_args.push_back(arg);
    } else {
      cmdline_args.push_back(replace_placeholders(arg, config));
    }
  }

  cmdline_args.emplace_back("--xport");
  cmdline_args.push_back(std::to_string(bind_port));

  SCOPED_TRACE("// start binary");
  launch_command(mysql_server_mock_path, cmdline_args, EXIT_SUCCESS, true,
                 1000ms);

  SCOPED_TRACE("// checking "s + GetParam().test_name);
  x_protocol_connect_ok(config.at("hostname"), bind_port);
}

const MockServerConnectOkTestParam mock_server_connect_ok_test_param[] = {
    {"ssl_mode_default",
     {
         "--filename", "@datadir@/my_port.js",  //
         "--module-prefix", "@datadir@",        //
     }},
    {"ssl_mode_disabled",
     {
         "--filename", "@datadir@/my_port.js",  //
         "--module-prefix", "@datadir@",        //
         "--ssl-mode", "disabled",              //
     }},
    {"ssl_mode_disabled_ignored_ssl_key",
     {
         "--filename", "@datadir@/my_port.js",     //
         "--module-prefix", "@datadir@",           //
         "--ssl-mode", "disabled",                 //
         "--ssl-key", "@certdir@/does-not-exist",  //
     }},
    {"ssl_mode_disabled_ignored_ssl_cert",
     {
         "--filename", "@datadir@/my_port.js",      //
         "--module-prefix", "@datadir@",            //
         "--ssl-mode", "disabled",                  //
         "--ssl-cert", "@certdir@/does-not-exist",  //
     }},
    {"ssl_mode_required",
     {
         "--filename", "@datadir@/my_port.js",       //
         "--module-prefix", "@datadir@",             //
         "--ssl-mode", "required",                   //
         "--ssl-key", "@certdir@/server-key.pem",    //
         "--ssl-cert", "@certdir@/server-cert.pem",  //
     }},
    {"ssl_mode_preferred",
     {
         "--filename", "@datadir@/my_port.js",       //
         "--module-prefix", "@datadir@",             //
         "--ssl-mode", "preferred",                  //
         "--ssl-key", "@certdir@/server-key.pem",    //
         "--ssl-cert", "@certdir@/server-cert.pem",  //
     }},
    {"ssl_cipher_aes128_sha256",
     {
         "--filename", "@datadir@/my_port.js",       //
         "--module-prefix", "@datadir@",             //
         "--ssl-mode", "preferred",                  //
         "--ssl-key", "@certdir@/server-key.pem",    //
         "--ssl-cert", "@certdir@/server-cert.pem",  //
         "--ssl-cipher", "AES128-SHA256",            // a known cipher
     }},
    {"ssl_cipher_empty",
     {
         "--filename", "@datadir@/my_port.js",       //
         "--module-prefix", "@datadir@",             //
         "--ssl-mode", "preferred",                  //
         "--ssl-key", "@certdir@/server-key.pem",    //
         "--ssl-cert", "@certdir@/server-cert.pem",  //
         "--ssl-cipher", "",                         //
     }},
    {"ssl_ca",
     {
         "--filename", "@datadir@/my_port.js",       //
         "--module-prefix", "@datadir@",             //
         "--ssl-mode", "preferred",                  //
         "--ssl-key", "@certdir@/server-key.pem",    //
         "--ssl-cert", "@certdir@/server-cert.pem",  //
         "--ssl-ca", "@certdir@/cacert.pem",         //
     }},
    {"ssl_crl_no_client_cert",
     // if no cert is presented, and it isn't required by the test-file, check
     // the CRL file is not having an impact.
     {
         "--filename", "@datadir@/my_port.js",             //
         "--module-prefix", "@datadir@",                   //
         "--ssl-mode", "preferred",                        //
         "--ssl-key", "@certdir@/server-key.pem",          //
         "--ssl-cert", "@certdir@/server-cert.pem",        //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",          //
         "--ssl-crl", "@certdir@/crl-client-revoked.crl",  //
     }},
    {"auth_with_username",
     {
         "--filename", "@datadir@/mock_server_require_username.js",  //
         "--module-prefix", "@datadir@",                             //
         "--ssl-mode", "preferred",                                  //
         "--ssl-key", "@certdir@/server-key.pem",                    //
         "--ssl-cert", "@certdir@/server-cert.pem",                  //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                    //
     }},
    {"auth_with_password",
     {
         "--filename", "@datadir@/mock_server_require_password.js",  //
         "--module-prefix", "@datadir@",                             //
         "--ssl-mode", "preferred",                                  //
         "--ssl-key", "@certdir@/server-key.pem",                    //
         "--ssl-cert", "@certdir@/server-cert.pem",                  //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                    //
     }},
    {"client_cert_no_cert_required",
     // testfile doesn't require a cert.
     {
         "--filename", "@datadir@/my_port.js",       //
         "--module-prefix", "@datadir@",             //
         "--ssl-mode", "preferred",                  //
         "--ssl-key", "@certdir@/server-key.pem",    //
         "--ssl-cert", "@certdir@/server-cert.pem",  //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",    //
     }},
};

INSTANTIATE_TEST_CASE_P(Spec, MockServerConnectOkTest,
                        ::testing::ValuesIn(mock_server_connect_ok_test_param),
                        [](const auto &info) { return info.param.test_name; });

// custom connect tests

struct MockServerConnectTestParam {
  const char *test_name;

  std::vector<std::string> cmdline_args;

  std::function<void(const std::map<std::string, std::string> &args)> checker;
};

class MockServerConnectTest
    : public RouterComponentTest,
      public ::testing::WithParamInterface<MockServerConnectTestParam> {
 protected:
  TcpPortPool port_pool_;
};

TEST_P(MockServerConnectTest, check) {
  auto mysql_server_mock_path = get_mysqlserver_mock_exec().str();

  ASSERT_THAT(mysql_server_mock_path, ::testing::StrNe(""));

  std::map<std::string, std::string> config{
      {"http_port", std::to_string(port_pool_.get_next_available())},
      {"port", std::to_string(port_pool_.get_next_available())},
      {"xport", std::to_string(port_pool_.get_next_available())},
      {"datadir", get_data_dir().str()},
      {"certdir", SSL_TEST_DATA_DIR},
      {"hostname", "127.0.0.1"},
  };

  std::vector<std::string> cmdline_args;

  for (const auto &arg : GetParam().cmdline_args) {
    if (arg.empty()) {
      cmdline_args.push_back(arg);
    } else {
      cmdline_args.push_back(replace_placeholders(arg, config));
    }
  }

  SCOPED_TRACE("// start binary");
  launch_command(mysql_server_mock_path, cmdline_args, EXIT_SUCCESS, true,
                 1000ms);

  SCOPED_TRACE("// checking "s + GetParam().test_name);
  GetParam().checker(config);
}

const MockServerConnectTestParam mock_server_connect_test_param[] = {
    {"client_cert_verify_cert_no_cert",
     // certificate is required, but no cert is presented
     {
         "--filename", "@datadir@/mock_server_cert_verify_cert.js",  //
         "--module-prefix", "@datadir@",                             //
         "--port", "@port@",                                         //
         "--ssl-mode", "required",                                   //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                    //
         "--ssl-key", "@certdir@/crl-server-key.pem",                //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",              //
         "--verbose",                                                //
     },
     [](const std::map<std::string, std::string> &config) {
       classic_protocol_connect_fail(config.at("hostname"),
                                     atol(config.at("port").c_str()),
                                     "someuser", "somepass", 1045);
     }},
    {"auth_wrong_password",
     {
         "--filename", "@datadir@/mock_server_require_password.js",  //
         "--module-prefix", "@datadir@",                             //
         "--port", "@port@",                                         //
         "--ssl-mode", "preferred",                                  //
         "--ssl-key", "@certdir@/server-key.pem",                    //
         "--ssl-cert", "@certdir@/server-cert.pem",                  //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                    //
     },
     [](const std::map<std::string, std::string> &config) {
       classic_protocol_connect_fail(config.at("hostname"),
                                     atol(config.at("port").c_str()),
                                     "someuser", "wrongpass", 1045);
     }},
    {"auth_wrong_username",
     {
         "--filename", "@datadir@/mock_server_require_username.js",  //
         "--module-prefix", "@datadir@",                             //
         "--port", "@port@",                                         //
         "--ssl-mode", "preferred",                                  //
         "--ssl-key", "@certdir@/server-key.pem",                    //
         "--ssl-cert", "@certdir@/server-cert.pem",                  //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                    //
     },
     [](const std::map<std::string, std::string> &config) {
       classic_protocol_connect_fail(config.at("hostname"),
                                     atol(config.at("port").c_str()),
                                     "wronguser", "wrongpass", 1045);
     }},
    {"client_cert_no_cert_required_with_trusted_cert",
     // testfile doesn't require a cert. But we send one anyway.
     {
         "--filename", "@datadir@/my_port.js",       //
         "--module-prefix", "@datadir@",             //
         "--port", "@port@",                         //
         "--ssl-mode", "preferred",                  //
         "--ssl-key", "@certdir@/server-key.pem",    //
         "--ssl-cert", "@certdir@/server-cert.pem",  //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",    //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       sess.set_ssl_cert(
           replace_placeholders("@certdir@/crl-client-cert.pem", config),
           replace_placeholders("@certdir@/crl-client-key.pem", config));

       sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                    "someuser",  // user
                    "somepass",  // pass
                    "",          // socket
                    ""           // schema
       );
     }},
    {"client_cert_require_certifiticate",
     {
         "--filename", "@datadir@/mock_server_cert_verify_cert.js",  //
         "--module-prefix", "@datadir@",                             //
         "--port", "@port@",                                         //
         "--ssl-mode", "required",                                   //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                    //
         "--ssl-key", "@certdir@/crl-server-key.pem",                //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",              //
         "--verbose",                                                //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       sess.set_ssl_cert(
           replace_placeholders("@certdir@/crl-client-cert.pem", config),
           replace_placeholders("@certdir@/crl-client-key.pem", config));

       try {
         sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                      "someuser",  // user
                      "somepass",  // pass
                      "",          // socket
                      ""           // schema
         );
       } catch (const std::exception &e) {
         FAIL() << e.what();
       }
     }},
    {"client_cert_verify_issuer",
     // testfile requires that issuer is the one of the crl-ca-cert.pem
     {
         "--filename", "@datadir@/mock_server_cert_verify_issuer.js",  //
         "--module-prefix", "@datadir@",                               //
         "--port", "@port@",                                           //
         "--ssl-mode", "required",                                     //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                      //
         "--ssl-key", "@certdir@/crl-server-key.pem",                  //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",                //
         "--verbose",                                                  //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       sess.set_ssl_cert(
           replace_placeholders("@certdir@/crl-client-cert.pem", config),
           replace_placeholders("@certdir@/crl-client-key.pem", config));

       try {
         sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                      "someuser",  // user
                      "somepass",  // pass
                      "",          // socket
                      ""           // schema
         );
       } catch (const std::exception &e) {
         FAIL() << e.what();
       }
     }},
    {"client_cert_verify_subject",
     {
         "--filename", "@datadir@/mock_server_cert_verify_subject.js",  //
         "--module-prefix", "@datadir@",                                //
         "--port", "@port@",                                            //
         "--ssl-mode", "required",                                      //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                       //
         "--ssl-key", "@certdir@/crl-server-key.pem",                   //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",                 //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       sess.set_ssl_cert(
           replace_placeholders("@certdir@/crl-client-cert.pem", config),
           replace_placeholders("@certdir@/crl-client-key.pem", config));

       try {
         sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                      "someuser",  // user
                      "somepass",  // pass
                      "",          // socket
                      ""           // schema
         );
       } catch (const std::exception &e) {
         FAIL() << e.what();
       }
     }},
    {"client_cert_verify_subject_wrong_subject",
     // present a client cert with the wrong subject
     {
         "--filename", "@datadir@/mock_server_cert_verify_subject.js",  //
         "--module-prefix", "@datadir@",                                //
         "--port", "@port@",                                            //
         "--ssl-mode", "required",                                      //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                       //
         "--ssl-key", "@certdir@/crl-server-key.pem",                   //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",                 //
         "--verbose",                                                   //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       // present the server cert instead of the client cert.
       sess.set_ssl_cert(
           replace_placeholders("@certdir@/crl-server-cert.pem", config),
           replace_placeholders("@certdir@/crl-server-key.pem", config));

       try {
         sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                      "someuser",  // user
                      "somepass",  // pass
                      "",          // socket
                      ""           // schema
         );
         FAIL() << "expected to fail";
       } catch (const mysqlrouter::MySQLSession::Error &e) {
         // access denied
         ASSERT_EQ(e.code(), 1045);
       }
     }},
    {"client_cert_revoked",
     // present a client cert with the wrong subject
     {
         "--filename", "@datadir@/mock_server_cert_verify_subject.js",  //
         "--module-prefix", "@datadir@",                                //
         "--port", "@port@",                                            //
         "--ssl-mode", "required",                                      //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                       //
         "--ssl-key", "@certdir@/crl-server-key.pem",                   //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",                 //
         "--ssl-crl", "@certdir@/crl-client-revoked.crl",               //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       // present the server cert instead of the client cert.
       sess.set_ssl_cert(replace_placeholders(
                             "@certdir@/crl-client-revoked-cert.pem", config),
                         replace_placeholders(
                             "@certdir@/crl-client-revoked-key.pem", config));

       try {
         sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                      "someuser",  // user
                      "somepass",  // pass
                      "",          // socket
                      ""           // schema
         );
         FAIL() << "expected to fail";
       } catch (const mysqlrouter::MySQLSession::Error &e) {
         // connection aborted
         //
         // openssl 1.1.1: 2013
         // openssl 1.0.2: 2026
         ASSERT_THAT(e.code(), ::testing::AnyOf(2013, 2026));
       }
     }},
    {"client_cert_revoked_other_cert",
     // present a client cert with the wrong subject
     {
         "--filename", "@datadir@/mock_server_cert_verify_subject.js",  //
         "--module-prefix", "@datadir@",                                //
         "--port", "@port@",                                            //
         "--ssl-mode", "required",                                      //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                       //
         "--ssl-key", "@certdir@/crl-server-key.pem",                   //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",                 //
         "--ssl-crl", "@certdir@/crl-client-revoked.crl",               //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       // present the server cert instead of the client cert.
       sess.set_ssl_cert(
           replace_placeholders("@certdir@/crl-client-cert.pem", config),
           replace_placeholders("@certdir@/crl-client-key.pem", config));

       try {
         sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                      "someuser",  // user
                      "somepass",  // pass
                      "",          // socket
                      ""           // schema
         );
       } catch (const mysqlrouter::MySQLSession::Error &e) {
         FAIL() << e.what();
       }
     }},
    {"tls_version_12",
     // allow only TLSv1.2 on the server and force client to TLSv1.2 too
     //
     // should succeed.
     {
         "--filename", "@datadir@/mock_server_cert_verify_subject.js",  //
         "--module-prefix", "@datadir@",                                //
         "--port", "@port@",                                            //
         "--ssl-mode", "required",                                      //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                       //
         "--ssl-key", "@certdir@/crl-server-key.pem",                   //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",                 //
         "--tls-version", "TLSv1.2",                                    //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       sess.set_ssl_cert(
           replace_placeholders("@certdir@/crl-client-cert.pem", config),
           replace_placeholders("@certdir@/crl-client-key.pem", config));

       sess.set_ssl_options(SSL_MODE_REQUIRED, "TLSv1.2",
                            "",  //
                            "",  //
                            "",  //
                            "",  //
                            "");

       try {
         sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                      "someuser",  // user
                      "somepass",  // pass
                      "",          // socket
                      ""           // schema
         );
       } catch (const mysqlrouter::MySQLSession::Error &e) {
         FAIL() << e.what();
       }
     }},

    {"tls_version_12_wrong_version",
     // allow only TLSv1.2 on the server and force client to TLSv1.1 too
     //
     // should fail
     {
         "--filename", "@datadir@/mock_server_cert_verify_subject.js",  //
         "--module-prefix", "@datadir@",                                //
         "--port", "@port@",                                            //
         "--ssl-mode", "required",                                      //
         "--ssl-ca", "@certdir@/crl-ca-cert.pem",                       //
         "--ssl-key", "@certdir@/crl-server-key.pem",                   //
         "--ssl-cert", "@certdir@/crl-server-cert.pem",                 //
         "--tls-version", "TLSv1.2",                                    //
     },
     [](const std::map<std::string, std::string> &config) {
       // connect should work.
       mysqlrouter::MySQLSession sess;

       sess.set_ssl_cert(
           replace_placeholders("@certdir@/crl-client-cert.pem", config),
           replace_placeholders("@certdir@/crl-client-key.pem", config));

       sess.set_ssl_options(SSL_MODE_REQUIRED, "TLSv1.1",
                            "",  //
                            "",  //
                            "",  //
                            "",  //
                            "");

       try {
         sess.connect(config.at("hostname"), atol(config.at("port").c_str()),
                      "someuser",  // user
                      "somepass",  // pass
                      "",          // socket
                      ""           // schema
         );
         FAIL() << "expected to failed";
       } catch (const mysqlrouter::MySQLSession::Error &e) {
         ASSERT_EQ(e.code(), 2026);
       }
     }},
};

INSTANTIATE_TEST_CASE_P(Spec, MockServerConnectTest,
                        ::testing::ValuesIn(mock_server_connect_test_param),
                        [](const auto &info) { return info.param.test_name; });

int main(int argc, char *argv[]) {
  ProcessManager::set_origin(Path(argv[0]).dirname());
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
