#include "gtest/gtest.h"
#include "factory/echo_handler_factory.h"

class EchoHandlerFactoryTest : public :: testing::Test{ // test fixture
protected:
  NginxConfig config;
  RequestHandler * handler;
  EchoHandlerFactory factory;
  void SetUp(){
    handler = factory.create("","",config);
  }
  void TearDown(){
    delete handler;
  }
};

TEST_F(EchoHandlerFactoryTest, handle_echo_request){
    std::string target = "/echo1";
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, "example.com");
    req.set(http::field::user_agent, "Boost.Beast");

    http::response<http::string_body> res;

    
    // process request
    status http_status = handler->handle_request(req, res);

    // read request
    std::ostringstream oss;
	oss << req;
	std::string req_str = oss.str();
	oss.clear();

    // read response body
    std::string res_body_str = res.body();
    EXPECT_EQ(res_body_str, req_str);
}
