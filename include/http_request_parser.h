#ifndef HTTP_REQUEST_PARSER_H
#define HTTP_REQUEST_PARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <map>

class HttpRequestParser {
public:
    HttpRequestParser();

    // main parse function
    bool parse(const std::string& request_header);

    // getters
    std::string get_method() const;
    std::string get_url() const;
    std::string get_http_version() const;
    std::string get_header(const std::string& header) const;
    std::string get_request() const;
    
    void set_request_header(std::string header);
    void set_request_body(std::string body);

private:
    // parser helpers
    bool parse_request_line(const std::string& line);
    bool parse_header_line(const std::string& line); 

    std::string request_;
    std::string method_;
    std::string url_;
    std::string http_version_;
    std::string header_;
    std::string body_;
    std::map<std::string, std::string> headers_;
};
#endif // HTTP_REQUEST_PARSER_H