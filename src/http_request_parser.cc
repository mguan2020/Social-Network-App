#include "http_request_parser.h"

HttpRequestParser::HttpRequestParser() 
    : method_(""), url_(""), http_version_(""), request_("")
{
}

bool HttpRequestParser::parse(const std::string& request_header)
{
    std::istringstream request_stream(request_header);
    std::string line;

    if (!std::getline(request_stream, line)) {
        return false;
    }

    if (!parse_request_line(line)) {
        return false;
    }

    while (std::getline(request_stream, line) && line != "\r") {
        if (!parse_header_line(line)) {
            return false;
        }
    }

    return true;
}

std::string HttpRequestParser::get_request() const
{
    return header_ + body_;
}

std::string HttpRequestParser::get_method() const
{
    return method_;
}


std::string HttpRequestParser::get_url() const 
{
    return url_;
}

std::string HttpRequestParser::get_http_version() const 
{
    return http_version_;
}

std::string HttpRequestParser::get_header(const std::string& header) const 
{
    auto it = headers_.find(header);
    if (it != headers_.end()) {
        return it->second;
    }
    return "";
}

bool HttpRequestParser::parse_request_line(const std::string& line) {
    std::istringstream line_stream(line);
    if (!(line_stream >> method_ >> url_ >> http_version_)) {
        return false;
    }
    return true;
}

bool HttpRequestParser::parse_header_line(const std::string& line) {
    std::size_t colon_pos = line.find(":");
    if (colon_pos == std::string::npos) {
        return false;
    }

    std::string header = line.substr(0, colon_pos);
    std::string value = line.substr(colon_pos + 1);
    headers_[header] = value;
    return true;
}

void HttpRequestParser::set_request_header(std::string header)
{
    header_ = header;
}

void HttpRequestParser::set_request_body(std::string body)
{
    body_ = body;
}