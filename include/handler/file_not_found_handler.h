#ifndef FILE_NOT_FOUND_HANDLER_NEW_H
#define FILE_NOT_FOUND_HANDLER_NEW_H

#include "request_handler.h"

class FileNotFoundHandler : public RequestHandler
{
public:
    FileNotFoundHandler();
    status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) override;
};

#endif // FILE_NOT_FOUND_HANDLER_NEW_H 