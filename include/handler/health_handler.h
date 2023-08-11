#ifndef HEALTH_HANDLER_NEW_H
#define HEALTH_HANDLER_NEW_H

#include "request_handler.h"

class HealthHandler : public RequestHandler
{
public:
    HealthHandler(std::string location, std::string url);
    status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) override;

private:
    std::string location_;
    std::string url_;
};

#endif // HEALTH_HANDLER_NEW_H 