#ifndef BLOCK_HANDLER_NEW_H
#define BLOCK_HANDLER_NEW_H

#include "request_handler.h"

class BlockHandler : public RequestHandler
{
public:
    BlockHandler();
    status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) override;
};

#endif // BLOCK_HANDLER_NEW_H 