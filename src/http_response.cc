#include "http_response.h"

HttpResponse::HttpResponse(int statusCode, const std::string& httpVersion = "HTTP/1.1")
: statusCode_(statusCode), httpVersion_(httpVersion) 
{
}

void HttpResponse::add_header(const std::string& header, const std::string& value) 
{
    headers_[header] = value;
}

void HttpResponse::set_body(const std::string& body) 
{
    body_ = body;
}

std::string HttpResponse::to_string() const 
{
    std::stringstream response;
    response << httpVersion_ << " " << statusCode_ << "\r\n";
    for (const auto& header : headers_) {
        response << header.first << ": " << header.second << "\r\n";
    }
    response << "\r\n" << body_;
    return response.str();
}
