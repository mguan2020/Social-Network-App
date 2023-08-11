#include "gtest/gtest.h"
#include "handler/static_handler.h"
#include <fstream>

class StaticHandlerTest : public ::testing::Test {

};

TEST_F(StaticHandlerTest, FileNotFound) {
    std::string target = "DNE_file";

    StaticHandler handler = StaticHandler(target);
    
    http::request<http::string_body> req(http::verb::get, target, 11);
    http::response<http::string_body> res;
    
    // process request
    status s = handler.handle_request(req, res);

    EXPECT_EQ(s, http::status::not_found);
}

TEST_F(StaticHandlerTest, ServeTxtFile) {
    std::string target = "static_test_files/test.txt";

    StaticHandler handler = StaticHandler(target);
    
    http::request<http::string_body> req(http::verb::get, target, 11);
    http::response<http::string_body> res;
    
    // process request
    status s = handler.handle_request(req, res);

    // read file
    std::ifstream file(target.c_str(), std::ios::in | std::ios::binary);
    std::string content;
    char c;
    while (file.get(c)) content += c;
    file.close();

    // read response body
    std::string res_body_str = res.body();

    EXPECT_EQ(res_body_str, content);
}

TEST_F(StaticHandlerTest, ServeHTMLFile) {
    std::string target = "static_test_files/test.html";

    StaticHandler handler = StaticHandler(target);
    
    http::request<http::string_body> req(http::verb::get, target, 11);
    http::response<http::string_body> res;
    
    // process request
    status s = handler.handle_request(req, res);

    // read file
    std::ifstream file(target.c_str(), std::ios::in | std::ios::binary);
    std::string content;
    char c;
    while (file.get(c)) content += c;
    file.close();

    // read response body
    std::string res_body_str = res.body();

    EXPECT_EQ(res_body_str, content);
}