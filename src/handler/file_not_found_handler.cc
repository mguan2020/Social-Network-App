#include "handler/file_not_found_handler.h"
#include "logger.h"

FileNotFoundHandler::FileNotFoundHandler()
{
}

status FileNotFoundHandler::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    // set response header
    status res_status = http::status::not_found;
    int res_version = 11;
    std::string res_content_type = "text/plain";
    
    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, res_content_type);

    // prepare response
    res.prepare_payload();

    Logger* logger = Logger::getLogger();

    logger->log_response_data(404,req.target().to_string(),"FileNotFoundHandler");

    return res_status;
}