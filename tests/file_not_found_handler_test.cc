#include "gtest/gtest.h"
#include "handler/file_not_found_handler.h"

TEST(FileNotFoundTest, FileNotFound) {
    std::string target = "DNE_file";

    FileNotFoundHandler handler = FileNotFoundHandler();
    
    http::request<http::string_body> req(http::verb::get, target, 11);
    http::response<http::string_body> res;
    
    // process request
    status s = handler.handle_request(req, res);

    EXPECT_EQ(s, http::status::not_found);
}
