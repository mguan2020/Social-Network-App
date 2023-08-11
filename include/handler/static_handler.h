#ifndef FILE_HANDLER_NEW_H
#define FILE_HANDLER_NEW_H

#include <string>
#include "request_handler.h"

class StaticHandler : public RequestHandler
{
public:
    StaticHandler(std::string file_name);
    status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) override;
private:
    std::string file_name_;
};

#endif // FILE_HANDLER_NEW_H