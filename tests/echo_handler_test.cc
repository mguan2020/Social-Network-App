#include "gtest/gtest.h"
#include "handler/echo_handler.h"

class EchoHandlerTest : public ::testing::Test {
protected:
    EchoHandler handler;
};

TEST_F(EchoHandlerTest, EmptyEchoTest) {
    std::string target = "echo";
    
    http::request<http::string_body> req(http::verb::get, target, 11);
    http::response<http::string_body> res;
    
    // process request
    handler.handle_request(req, res);

    // read request
    std::ostringstream oss;
	oss << req;
	std::string req_str = oss.str();
	oss.clear();

    // read response body
    std::string res_body_str = res.body();

    EXPECT_EQ(res_body_str, req_str);
}

TEST_F(EchoHandlerTest, NonEmptyEchoTest) {
    std::string target = "echo";
    
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, "example.com");
    req.set(http::field::user_agent, "Boost.Beast");

    http::response<http::string_body> res;

    
    // process request
    handler.handle_request(req, res);

    // read request
    std::ostringstream oss;
	oss << req;
	std::string req_str = oss.str();
	oss.clear();

    // read response body
    std::string res_body_str = res.body();

    EXPECT_EQ(res_body_str, req_str);
}