#include "handler/health_handler.h"
#include "logger.h"

HealthHandler::HealthHandler(std::string location, std::string url) : location_(location), url_(url)
{
}

status HealthHandler::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{

    // set response header
    status res_status = http::status::ok;
    int res_version = 11;
    std::string res_content_type = "text/plain";
    
    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, res_content_type);

    std::string content = "OK";
    res.body() = content;

    // prepare response
    res.prepare_payload();

    Logger* logger = Logger::getLogger();
    logger->log_response_data(200,req.target().to_string(),"HealthHandler");

    return res_status;
}