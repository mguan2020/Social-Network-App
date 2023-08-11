#ifndef ECHO_HANDLER_NEW_H
#define ECHO_HANDLER_NEW_H

#include "request_handler.h"

class EchoHandler : public RequestHandler
{
public:
    EchoHandler();
    status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) override;
};

#endif // ECHO_HANDLER_NEW_H 