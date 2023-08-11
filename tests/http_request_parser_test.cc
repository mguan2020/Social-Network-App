#ifndef HTTP_REQUEST_PARSER_TEST_H
#define HTTP_REQUEST_PARSER_TEST_H

#include "gtest/gtest.h"
#include "http_request_parser.h"


class HttpRequestParserTest : public :: testing::Test{ // test fixture
protected:
  HttpRequestParser parser;
};

TEST_F(HttpRequestParserTest, GET_METHOD) {

  bool success = parser.parse("GET / HTTP/1.1\r\nHost: example.com\r\nContent-Length: 30\r\n\r\n");
  EXPECT_TRUE(success);
  std::cout << parser.get_header("Content-Length") << std::endl;
  EXPECT_TRUE(parser.get_method() == "GET");
  EXPECT_TRUE(parser.get_url().compare("/") == 0);
  EXPECT_TRUE(parser.get_http_version().compare("HTTP/1.1") == 0);
  EXPECT_TRUE(parser.get_header("Connection").compare("") == 0);
  EXPECT_EQ(parser.get_header("Content-Length"), " 30\r");
}

TEST_F(HttpRequestParserTest, POST_METHOD) {

  bool success = parser.parse("POST / HTTP/1.0\r\nHost: example.com\r\nContent-Length: 30\r\n\r\n123456789012345678901234567890\r\n\r\n"); 
    
  EXPECT_TRUE(success); 
  EXPECT_TRUE(parser.get_method() == "POST");
}


TEST_F(HttpRequestParserTest, INVALID_GET_REQ) {

  bool fail = parser.parse("GET \r\nHost: example.com\r\n\r\n\r\n");
  EXPECT_FALSE(fail);
}

TEST_F(HttpRequestParserTest, INVALID_HEADER_FIELD) {

  bool fail = parser.parse("GET / HTTP/1.1\r\nA_invalid_header_field\r\nContent-Length: 30\r\n\r\n");
  EXPECT_FALSE(fail);
}

#endif // HTTP_REQUEST_PARSER_TEST_H