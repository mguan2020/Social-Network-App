#include "gtest/gtest.h"
#include "get_response.h"

class GetResponseTest : public :: testing::Test{ // test fixture

};

// This test is to test http_text() function in get_response.cc
TEST_F(GetResponseTest, http_echo) {
    std::string text = "Hello World!";
    std::string response = GetResponse::http_echo(text);
    EXPECT_TRUE(response.find("Hello World!") != std::string::npos);
}