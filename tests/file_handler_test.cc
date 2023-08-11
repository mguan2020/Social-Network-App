#include "gtest/gtest.h"
#include "handler/file_handler.h"
#include "http_request_parser.h"

class FileHandlerTest : public ::testing::Test {
protected:
    FileHandler* handler;
    void SetUp(std::vector<path> &paths) {
        handler = new FileHandler(paths);
    }
    void TearDown() {
        delete handler;
    }
};

// This test is to test get_path() function with a simple case
TEST_F(FileHandlerTest, SimpleGetPathTest) {
  std::vector<path> paths;
  path p1;
  p1.endpoint = "/static";
  p1.endpointType = STATIC;
  p1.root = "/foo/bar";
  paths.push_back(p1);
  path p2;
  p2.endpoint = "/static2";
  p2.endpointType = STATIC;
  p2.root = "/huh";
  paths.push_back(p2);
  path p3;
  p3.endpoint = "/echo";
  p3.endpointType = ECHO_TYPE;
  p3.root = "";
  paths.push_back(p3);
  SetUp(paths);
  std::string request = "GET /static/index.html HTTP/1.1\r\n\r\n";
  HttpRequestParser rp = HttpRequestParser();
  rp.parse(request);
  std::string url = handler->get_path(rp);
  EXPECT_EQ(url, "/foo/bar/index.html");
}

// This test is to test get_path() and handle_request() function with no matching path
TEST_F(FileHandlerTest, SimpleNoMatchingPath) {
  std::vector<path> paths;
  path p1;
  p1.endpoint = "/static/";
  p1.endpointType = STATIC;
  p1.root = "/foo/bar";
  paths.push_back(p1);
  path p2;
  p2.endpoint = "/static2/";
  p2.endpointType = STATIC;
  p2.root = "/huh";
  paths.push_back(p2);
  path p3;
  p3.endpoint = "/echo/";
  p3.endpointType = ECHO_TYPE;
  p3.root = "";
  paths.push_back(p3);
  SetUp(paths);
  std::string request = "GET /static3/index.html HTTP/1.1\r\n\r\n";
  HttpRequestParser rp = HttpRequestParser();
  rp.parse(request);
  std::string url = handler->get_path(rp);
  EXPECT_EQ(url, "");
  EXPECT_EQ(handler->handle_request(rp), GetResponse::http_not_found());
}