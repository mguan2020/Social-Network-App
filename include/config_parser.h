#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>

class NginxConfig;
class RequestHandlerFactory;

#define STATIC 0
#define ECHO_TYPE 1
#define INVALID 2
#define OTHER 3


struct path{ // stores information about the paths of the file to locate
    int endpointType = ECHO_TYPE;
    std::string endpoint = "";
    std::string root = "";
    std::map<std::string, std::string> path_map; // map argument names to URL
};

struct ParsedData{
    int portno;
    std::vector<path> paths;
    std::map<std::string, NginxConfig*> arguments;
    std::map<std::string, RequestHandlerFactory*> factories;
    // more fields to be added as needed
};


// The parsed representation of a single config statement.
class NginxConfigStatement {
 public:
  std::string ToString(int depth);
  std::vector<std::string> tokens_;
  std::unique_ptr<NginxConfig> child_block_;
};

// The parsed representation of the entire config.
class NginxConfig {
 public:
  std::string ToString(int depth = 0);
  std::vector<std::shared_ptr<NginxConfigStatement>> statements_;
  int get_port_num();
  std::vector<path> getPaths();
  std::map<std::string, NginxConfig*> getArguments();
  std::map<std::string, RequestHandlerFactory*>  getFactories();

 private:
   //std::vector<path> paths; // store this into a hashmap
   int numPorts();
   ParsedData pd; 
};

// The driver that parses a config file and generates an NginxConfig.
class NginxConfigParser {
 public:
  NginxConfigParser();

  // Take an opened config file or file name (respectively) and store the
  // parsed config in the provided NginxConfig out-param.  Returns true
  // iff the input config file is valid.
  bool Parse(std::istream* config_file, NginxConfig* config);
  bool Parse(const char* file_name, NginxConfig* config);
 private:
  enum TokenType {
    TOKEN_TYPE_START = 0,
    TOKEN_TYPE_NORMAL = 1,
    TOKEN_TYPE_START_BLOCK = 2,
    TOKEN_TYPE_END_BLOCK = 3,
    TOKEN_TYPE_COMMENT = 4,
    TOKEN_TYPE_STATEMENT_END = 5,
    TOKEN_TYPE_EOF = 6,
    TOKEN_TYPE_ERROR = 7
  };
  const char* TokenTypeAsString(TokenType type);

  enum TokenParserState {
    TOKEN_STATE_INITIAL_WHITESPACE = 0,
    TOKEN_STATE_SINGLE_QUOTE = 1,
    TOKEN_STATE_DOUBLE_QUOTE = 2,
    TOKEN_STATE_TOKEN_TYPE_COMMENT = 3,
    TOKEN_STATE_TOKEN_TYPE_NORMAL = 4
  };

  TokenType ParseToken(std::istream* input, std::string* value);
};

#endif  // CONFIG_PARSER_H
