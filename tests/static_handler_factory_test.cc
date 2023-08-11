#include "gtest/gtest.h"
#include "factory/static_handler_factory.h"
#include <fstream>

class StaticHandlerFactoryTest : public :: testing::Test{ // test fixture
protected:
  NginxConfig config;
  RequestHandler * handler;
  StaticHandlerFactory factory;
  NginxConfigParser parser;
  std::map<std::string, NginxConfig*> arguments;
  NginxConfig* argument;
  void SetUp(){
    parser.Parse("configs/static_test_config",&config);
    arguments = config.getArguments();
    argument = arguments["/static1"];
    handler = factory.create("/static1","/static1/test.txt", *argument);
  }
  void TearDown(){
    delete handler;
  }
};

TEST_F(StaticHandlerFactoryTest, serve_txt_file){

    std::string target = "/static1/test.txt";
    std::string file_path = "static_test_files/test.txt";
    
    http::request<http::string_body> req(http::verb::get, target, 11);
    http::response<http::string_body> res;
    
    // process request
    status s = handler->handle_request(req, res);

    // read file
    std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
    std::string content;
    char c;
    while (file.get(c)) content += c;
    file.close();

    // read response body
    std::string res_body_str = res.body();

    EXPECT_EQ(res_body_str, content);
}
