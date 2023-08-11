#include "gtest/gtest.h"
#include "config_parser.h"


class NginxConfigParserTest : public :: testing::Test{ // test fixture
protected:
  NginxConfigParser parser;
  NginxConfig config;
};

/*
 * These tests are for the ToString function in config_parser.cc
 */

TEST_F(NginxConfigParserTest, NginxConfigToString) {
  bool success = parser.Parse("configs/example_config", &config);
  std::string res =  "foo \"bar\";\nserver {\n  listen 80;\n  server_name foo.com;\n  root /home/ubuntu/sites/foo/;\n}\n";
  std::string config_string = config.ToString();
  EXPECT_TRUE(config_string.compare(res) == 0);
}


/*
 * These tests are for the get_port_num function in config_parser.cc
 */

// handles test case of no nesting (no child block)
TEST_F(NginxConfigParserTest, portnum) {
  bool success = parser.Parse("configs/portnotest", &config);
  int portno = config.get_port_num();
  EXPECT_TRUE(portno == 8080);
}

// handles test case with child block
TEST_F(NginxConfigParserTest, portnumwithchildblock) {
  bool success = parser.Parse("configs/withchild", &config);
  int portno = config.get_port_num();
  EXPECT_TRUE(portno == 8080);
}

// handles test case with invalid portno
TEST_F(NginxConfigParserTest, badport) {
  bool success = parser.Parse("configs/invalidport", &config);
  int portno = config.get_port_num();
  EXPECT_TRUE(portno == -1);
}

// handles test case with more than one portno provided
TEST_F(NginxConfigParserTest, multiple) {
  bool success = parser.Parse("configs/multipleports", &config);
  int portno = config.get_port_num();
  EXPECT_TRUE(portno == -1);
}

// handles test case with incorrect config file syntax
TEST_F(NginxConfigParserTest, badsyntax) {
  bool success = parser.Parse("configs/conbadsyntax", &config);
  EXPECT_FALSE(success);
}

// handles test case with no portno provided
TEST_F(NginxConfigParserTest, noport) {
  bool success = parser.Parse("configs/noport", &config);
  int portno = config.get_port_num();
  EXPECT_TRUE(portno == -1);
}

/*
 * below are the tests I used for my previous assignment on config_parser
 * used for testing the parse function in config_parser.cc
 */

// This test deals with a simple nginx config file
TEST_F(NginxConfigParserTest, SimpleConfig) {
bool success = parser.Parse("configs/example_config", &config);
EXPECT_TRUE(success);
}
// This test deals with non-existing/bad files
TEST_F(NginxConfigParserTest, ReturnFalseForBadFile) {
bool fail = parser.Parse("configs/IDontExist", &config);
EXPECT_FALSE(fail);
}

// This test deals with an empty file
TEST_F(NginxConfigParserTest, SimpleConfigEmpty) {
bool success = parser.Parse("configs/example_config_empty", &config);
EXPECT_TRUE(success);
}

// This test deals with space before colons
TEST_F(NginxConfigParserTest, SimpleConfigWithSpaceBeforeColon) {
bool success = parser.Parse("configs/example_config_with_space", &config);
EXPECT_TRUE(success);
}
// This test deals with config file with various comments
TEST_F(NginxConfigParserTest, SimpleConfigWithComment) {
bool success = parser.Parse("configs/example_config_with_comment", &config);
EXPECT_TRUE(success);
}
// This test deals with nested block,
TEST_F(NginxConfigParserTest, SimpleConfigNested) {
bool success = parser.Parse("configs/example_config_with_location", &config);
EXPECT_TRUE(success);
}
// This test deals with empty nested block,
// i.e. '{' followed by '}' immediately
TEST_F(NginxConfigParserTest, SimpleConfigNestedEmpty) {
bool success = parser.Parse("configs/example_config_with_empty_location", &config);
EXPECT_TRUE(success);
}
// This test deals with mismatched curly braces.
// with two parts, the first with an excess in '}'
// and the second with an excess in '{'
TEST_F(NginxConfigParserTest, SimpleConfigWithMismatchedCurlyBrackets) {
bool fail = parser.Parse("configs/example_config_with_mismatched_curly_brackets_1", &config);
EXPECT_FALSE(fail);
fail = parser.Parse("configs/example_config_with_mismatched_curly_brackets_2", &config);
EXPECT_FALSE(fail);
}

// This test deals with nested block with no token.
TEST_F(NginxConfigParserTest, SimpleConfigWithNoToken) {
bool fail = parser.Parse("configs/example_config_with_no_location_keyword", &config);
EXPECT_FALSE(fail);
}

// This test deals with multiple token before nested block.
TEST_F(NginxConfigParserTest, SimpleConfigWithMultipleToken) {
bool success = parser.Parse("configs/example_config_with_multiple_token", &config);
EXPECT_TRUE(success);
}

// This test deals with multiple blocks
TEST_F(NginxConfigParserTest, SimpleConfigWithMultipleBlock) {
bool success = parser.Parse("configs/example_config_with_multiple_block", &config);
EXPECT_TRUE(success);
}

// This test deals with escape within quoted strings
TEST_F(NginxConfigParserTest, SimpleConfigWithEscapeInString) {
bool success = parser.Parse("configs/example_config_with_escape_in_string_1", &config);
EXPECT_TRUE(success);
success = parser.Parse("configs/example_config_with_escape_in_string_2", &config);
EXPECT_TRUE(success);
}

// This test deals with no blank space after quoted strings
TEST_F(NginxConfigParserTest, SimpleConfigWithNoBlankAfterString) {
bool fail = parser.Parse("configs/example_config_with_no_blank_after_string", &config);
EXPECT_FALSE(fail);
}

// newly added test dealing with multiple semi_colons
TEST_F(NginxConfigParserTest, SimpleConfigWithMultipleSemiColon) {
  bool fail = parser.Parse("configs/example_config_with_multiple_semi_colon", &config);
  EXPECT_FALSE(fail);
}



// tests parsing of a config file with a single local path

TEST_F(NginxConfigParserTest, ConfigWithLocalPathsSimple){
  parser.Parse("configs/configwithpaths_1",&config);

  std::vector<path> paths = config.getPaths();
  
  EXPECT_EQ(paths.size(),1);
  bool success = paths[0].endpointType == STATIC && paths[0].endpoint == "/static1" && paths[0].path_map["root"] == "/usr/bin";

  EXPECT_TRUE(success);
}



// tests parsing of local paths with multiple (two) static local paths

TEST_F(NginxConfigParserTest, ConfigWithMultipleLocalPaths){
  parser.Parse("configs/configwithpaths_2",&config);

  std::vector<path> paths = config.getPaths();

  EXPECT_EQ(paths.size(),2);
  bool success1 = paths[0].endpointType == STATIC && paths[0].endpoint == "/static1" && paths[0].path_map["root"] == "/usr/bin";
  bool success2 = paths[1].endpointType == STATIC && paths[1].endpoint == "/static2" && paths[1].path_map["root"] == "/foo/bar";
  EXPECT_TRUE(success1 && success2);
}



// tests parsing of both static and echo paths

TEST_F(NginxConfigParserTest, ConfigWithLocalPathsBothStaticAndEcho){
  parser.Parse("configs/configwithpaths_3",&config);

  std::vector<path> paths = config.getPaths();

  EXPECT_EQ(paths.size(),3);
 
  bool success1 = paths[0].endpointType == STATIC && paths[0].endpoint == "/static1" && paths[0].path_map["root"] == "/usr/bin";
  bool success2 = paths[1].endpointType == STATIC && paths[1].endpoint == "/static2" && paths[1].path_map["root"] == "/foo/bar";
  bool success3 = paths[2].endpointType == ECHO_TYPE && paths[2].endpoint == "/echo" && paths[2].root == "";

  EXPECT_TRUE(success1 && success2 && success3);
}

// tests removal of trailing slashes in a path

TEST_F(NginxConfigParserTest, TrailingSlash){
  parser.Parse("configs/configwithpaths_4",&config);

  std::vector<path> paths = config.getPaths();

  EXPECT_EQ(paths.size(),1);

  bool success1 = paths[0].endpointType == STATIC && paths[0].endpoint == "/static1" && paths[0].path_map["root"] == "/usr/bin";

  EXPECT_TRUE(success1);
}

// tests the case with multiple arguments within a statichandler block

TEST_F(NginxConfigParserTest, MultipleArgs){
  parser.Parse("configs/configwithpaths_5",&config);

  std::vector<path> paths = config.getPaths();

  EXPECT_EQ(paths.size(),4);
 
  bool success1 = paths[0].endpointType == STATIC && paths[0].endpoint == "/static1" && paths[0].path_map["root"] == "usr/bin";
  bool success2 = paths[1].endpointType == STATIC && paths[1].endpoint == "/static1" && paths[1].path_map["altpath"] == "foo/bar";
  bool success3 = paths[2].endpointType == ECHO_TYPE && paths[2].endpoint == "/echo" && paths[2].root == "";
  bool success4 = paths[3].endpointType == STATIC && paths[3].endpoint == "/static2" && paths[3].path_map["root"] == "foo/bar";


  EXPECT_TRUE(success1 && success2 && success3 && success4);
}

// tests parsing of a config file with no static handler

TEST_F(NginxConfigParserTest, NoStaticHandler){
  parser.Parse("configs/configwithpaths_6",&config);

  std::vector<path> paths = config.getPaths();
  
  EXPECT_EQ(paths.size(),1);
  bool success = paths[0].endpointType == ECHO_TYPE && paths[0].endpoint == "/echo" && paths[0].root == "";

  EXPECT_TRUE(success);
}

// Tests getArguments() function in the config parser.
TEST_F(NginxConfigParserTest, GetArgumentsTest){
     parser.Parse("configs/configwithpaths_5",&config);
     std::map<std::string, NginxConfig*> paths = config.getArguments();
     EXPECT_EQ(paths.size(), 3);

}

TEST_F(NginxConfigParserTest, GetFactoriesTest){
     parser.Parse("configs/config_with_multiple_echos",&config);
     std::map<std::string, RequestHandlerFactory*> factories = config.getFactories();
     EXPECT_EQ(factories.size(), 3);
}



