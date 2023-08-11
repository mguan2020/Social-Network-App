#include "gtest/gtest.h"
#include "factory/health_handler_factory.h"

class HealthHandlerFactoryTest : public :: testing::Test{ // test fixture
protected:
  NginxConfig config;
  RequestHandler * handler;
  HealthHandlerFactory factory;
  void SetUp(){
    handler = factory.create("/health","/health",config);
  }
  void TearDown(){
    delete handler;
  }
};

TEST_F(HealthHandlerFactoryTest, HandleHealthFactory){
    std::string target = "/health";
    http::request<http::string_body> req{http::verb::get, target, 11};
    http::response<http::string_body> res;
    
    // process request
    status http_status = handler->handle_request(req, res);

    EXPECT_EQ(http_status, http::status::ok); // was the handler successfully created?
}