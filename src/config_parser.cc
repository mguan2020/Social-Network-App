// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <map>
#include "config_parser.h"
#include "logger.h"
#include "factory/request_handler_factory_factory.h"

NginxConfigParser::NginxConfigParser() {}

std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
	    serialized_config.append(statement->ToString(depth));
	  }
  return serialized_config;
 }


int NginxConfig::numPorts(){ // count number of port occurrences in the config file
	int count = 0;

	for(auto statement : statements_){
		if(statement->child_block_.get() == nullptr){
			if(statement->tokens_.size() == 2 && statement->tokens_[0] == "port"){
                              count++;
			}

		}
		else{
	             int c = statement->child_block_.get()->numPorts();
                     count += c;		     
		}
	}

	return count;
}

// extract port number from the config file and return -1 if error encountered
int NginxConfig::get_port_num(){

   // if(config == nullptr) return -1; // bad config, so return -1 to indicate error

    if(numPorts() > 1) return -1; // more than one port number, which is ambiguous

    for(auto statement : statements_){
           if(statement->child_block_.get() == nullptr){ // first visit statements without child blocks
                   if(statement->tokens_.size() == 2 && statement->tokens_[0] == "port"){
                           int portno = atoi(statement->tokens_[1].c_str());
			   int max_port_num = 0xffff; // port numbers cannot exceed 65535.
                           if(portno >= 0 && portno <= max_port_num){
			           pd.portno = portno;	   
                                   return portno;
                           }
                           else{
                                   return -1; // port number out of range
                           }

                   }
           }
    }


    // if we reach here, none of the statements without child blocks have the port number field
    for(auto statement : statements_){
            if(statement->child_block_.get() != nullptr){
                    int portno = statement->child_block_.get()->get_port_num(); // dfs to get child's portno
                    if(portno != -1){
			    pd.portno = portno;
                            return portno;
                    }
            }
    }

    pd.portno = -1;
    return -1; // otherwise, port number not found or invalid, so return -1
}


std::string removeTrailingSlashes(std::string input){

	while(input.length() > 1 && input.back() == '/'){
		input.pop_back();
	}

	return input;
  }




std::vector<path> NginxConfig::getPaths(){ // no longer require config file to have server block, per the common API


	for(auto statement : statements_) {

        if(statement->tokens_[0] == "location"){
            if(statement->tokens_.size() == 3 && statement->tokens_[2] == "EchoHandler" && statement->child_block_.get() != nullptr){
                  path p; 
				  p.endpointType = ECHO_TYPE;
				  p.endpoint = removeTrailingSlashes(statement->tokens_[1]);
				  p.root = "";
				  pd.paths.push_back(p);
            }
            else if(statement->tokens_.size() == 3 && statement->tokens_[2] == "StaticHandler" && statement->child_block_.get() != nullptr){
                  for(auto child : statement->child_block_->statements_){
                       path p;
                       p.endpointType = STATIC;
                       p.endpoint = removeTrailingSlashes(statement->tokens_[1]);
                       p.root = 
                       p.path_map[child->tokens_[0]] = child->tokens_[1]; 
                       pd.paths.push_back(p);
                  }
            }
            else{ 
                Logger* logger = Logger::getLogger();
                logger->log_error("Bad location statement inside the config file.");                
            }
        }
    }
    
    return pd.paths;
}

// returns a map containing NginxConfig pointers that contain handler arguments. Parsing of those arguments will be done in the factory.
std::map<std::string, NginxConfig*> NginxConfig::getArguments(){
    for(auto statement : statements_) {

        if(statement->tokens_[0] == "location"){
            if(statement->tokens_.size() == 3 && statement->child_block_.get() != nullptr){
                  pd.arguments[removeTrailingSlashes(statement->tokens_[1])] = statement->child_block_.get(); // valid location block
            }
            else{ 
                Logger* logger = Logger::getLogger();
                logger->log_error("Bad location statement inside the config file.");                
            }
        }
    }
    
    return pd.arguments;
}

std::map<std::string, RequestHandlerFactory*> NginxConfig::getFactories(){
    for(auto statement : statements_) {

        if(statement->tokens_[0] == "location"){
            if(statement->tokens_.size() == 3 && statement->child_block_.get() != nullptr){
                RequestHandlerFactory * factory = RequestHandlerFactoryFactory::createHandlerFactory(statement->tokens_[2]);
                if(!factory)
                {
                    Logger* logger = Logger::getLogger();
                    logger->log_error("Incorrect handler type in config file: " + statement->tokens_[2]);                
                }
                else
                {
                    pd.factories[removeTrailingSlashes(statement->tokens_[1])] = factory;
                }
            }
            else{ 
                Logger* logger = Logger::getLogger();
                logger->log_error("Bad location statement inside the config file.");                
            }
        }
    }
    
    return pd.factories;
}






std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        // TODO: the end of a quoted token should be followed by whitespace. -> RESOLVED
        // TODO: Maybe also define a QUOTED_STRING token type. -> RESOLVED
        // TODO: handle backslash escaping -> RESOLVED	

        *value += c;

        if(c == '\\' && input->peek() == '\''){ // bug fix: handle escaping of single quote
		*value += (char) (input->get());
		continue;
	}



        if (c == '\'') { // bug fix: require whitespace after single quotes

	  char ch = input->get();
          if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\t' ||
              ch == ';' || ch == '{' || ch == '}') {
          input->unget();
              }
          else{
            return TOKEN_TYPE_ERROR;
          }

          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;

         if(c == '\\' && input->peek() == '"'){ // bug fix: handle escaping of double quote
                *value += (char) (input->get());
                continue;
         }





        if (c == '"') { // bug fix: require whitespace after double quotes

	  char ch = input->get();
          if (ch == ' ' || ch == '\t' || ch == '\n' ||
              ch == ';' || ch == '{' || ch == '}') {
          input->unget();
              }
          else{
            return TOKEN_TYPE_ERROR;
          }
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  printf ("%s\n", TokenTypeAsString(last_token_type));
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) { 
      if (last_token_type != TOKEN_TYPE_STATEMENT_END && last_token_type != TOKEN_TYPE_START_BLOCK && last_token_type != TOKEN_TYPE_END_BLOCK) { // bug fix: allow nested 
        // Error.
        break;
      } 

      if(config_stack.size() == 1) { // bug fix: don't allow extra closing braces
	      break;
      }


      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          // Bug fixed: can now handle an empty config
          last_token_type != TOKEN_TYPE_END_BLOCK && last_token_type != TOKEN_TYPE_START) { 
        // Error.
        break;
      }
      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  printf ("Bad transition from %s to %s\n",
          TokenTypeAsString(last_token_type),
          TokenTypeAsString(token_type));
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    printf ("Failed to open config file: %s\n", file_name);
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}
