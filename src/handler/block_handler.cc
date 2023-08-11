#include "handler/block_handler.h"
#include "logger.h"

BlockHandler::BlockHandler()
{
}

status BlockHandler::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    // sleep for 5 sec
    sleep(5);

    Logger* logger = Logger::getLogger();
    logger->log_response_data(100, req.target().to_string(),"BlockHandler");


    return http::status::continue_;
}