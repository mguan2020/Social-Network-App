#ifndef HTTP_RESPONSE
#define HTTP_RESPONSE

#include <iostream>
#include <sstream>
#include <string>
#include <map>

class HttpResponse {
public:
    HttpResponse(int statusCode, const std::string& httpVersion);
        
    void add_header(const std::string& header, const std::string& value);

    void set_body(const std::string& body);

    std::string to_string() const;

private:
    int statusCode_;
    std::string httpVersion_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};

#endif