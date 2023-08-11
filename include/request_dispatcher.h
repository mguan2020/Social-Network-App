#ifndef REQUEST_DISPATCHER_H
#define REQUEST_DISPATCHER_H

#include "http_request_parser.h"
#include "factory/request_handler_factory.h"
#include <map>
#include <string>
#include "config_parser.h"

class RequestDispatcher
{
public:
    RequestDispatcher(const std::map<std::string, RequestHandlerFactory*> & handler_factories, const std::map<std::string, NginxConfig*> & path2arguments);
    void handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res);
protected:
    std::string get_response_header();
    const std::string* match(std::string url);
    std::map<std::string, RequestHandlerFactory*> handler_factories_;
    std::map<std::string, NginxConfig*> path2arguments_;
};

#endif // REQUEST_DISPATCHER_H