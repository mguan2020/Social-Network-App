#include "gtest/gtest.h"
#include "factory/file_not_found_handler_factory.h"

class FileNotFoundHandlerFactoryTest : public :: testing::Test{ // test fixture
protected:
  NginxConfig config;
  RequestHandler * handler;
  FileNotFoundHandlerFactory factory;
  void SetUp(){
    handler = factory.create("","",config);
  }
  void TearDown(){
    delete handler;
  }
};

TEST_F(FileNotFoundHandlerFactoryTest, HandleFileNotFound){
    std::string target = "/";
    http::request<http::string_body> req{http::verb::get, target, 11};
    http::response<http::string_body> res;
    
    // process request
    status http_status = handler->handle_request(req, res);

    EXPECT_EQ(http_status, http::status::not_found);
}
