#include "gtest/gtest.h"
#include "request_dispatcher.h"

class RequestDispatcherTest : public :: testing::Test{ // test fixture
protected:
  NginxConfigParser parser;
  NginxConfig config;
  RequestDispatcher * dispatcher;
  void SetUp(){
    parser.Parse("configs/config_with_multiple_echos",&config);
    std::map<std::string, RequestHandlerFactory*> factories = config.getFactories();
    std::map<std::string, NginxConfig*> arguments = config.getArguments();
    dispatcher = new RequestDispatcher(factories, arguments);
  }
  void TearDown(){
    delete dispatcher;
  }
};


TEST_F(RequestDispatcherTest, handle_echo_request){
    std::string target = "/echo1";
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, "example.com");
    req.set(http::field::user_agent, "Boost.Beast");

    http::response<http::string_body> res;

    
    // process request
    dispatcher->handle_request(req, res);

    // read request
    std::ostringstream oss;
	oss << req;
	std::string req_str = oss.str();
	oss.clear();

    // read response body
    std::string res_body_str = res.body();
    EXPECT_EQ(res_body_str, req_str);
}

TEST_F(RequestDispatcherTest, handle_invalid_path){
    std::string target = "/not_a_path";
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, "example.com");
    req.set(http::field::user_agent, "Boost.Beast");

    http::response<http::string_body> res;


    // process request
    dispatcher->handle_request(req, res);

    // read response body
    status http_status= res.result();
    EXPECT_TRUE(http_status == http::status::not_found);
}

TEST_F(RequestDispatcherTest, handle_return_404){
    std::string target = "/static/file_non_exist.txt";
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, "example.com");
    req.set(http::field::user_agent, "Boost.Beast");

    http::response<http::string_body> res;


    // process request
    dispatcher->handle_request(req, res);

    // read response body
    status http_status= res.result();
    EXPECT_TRUE(http_status == http::status::not_found);
}