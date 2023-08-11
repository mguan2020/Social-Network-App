#include "gtest/gtest.h"
#include "handler/health_handler.h"



class HealthHandlerTest : public ::testing::Test {

};

TEST_F(HealthHandlerTest, TestHealth) {
    std::string target = "/health";

    HealthHandler handler = HealthHandler("/health","/health");
    
    http::request<http::string_body> req(http::verb::get, target, 11);
    http::response<http::string_body> res;
    
    // process request
    status s = handler.handle_request(req, res);

    EXPECT_EQ(s, http::status::ok);

    EXPECT_EQ(res.body(), "OK");
}