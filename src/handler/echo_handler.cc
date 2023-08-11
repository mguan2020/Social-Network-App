#include "handler/echo_handler.h"
#include "logger.h"

EchoHandler::EchoHandler() {}

status EchoHandler::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    // read request into std::string object
    std::ostringstream oss;
	oss << req;
	std::string req_str = oss.str();
	oss.clear();

    // set response header
    status res_status = http::status::ok;
    int res_version = 11;
    std::string res_content_type = "text/plain";
    
    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, res_content_type);

    // set response body
    res.body() = req_str;

    // prepare response
    res.prepare_payload();

    Logger* logger = Logger::getLogger();
    logger->log_response_data(200, req.target().to_string(),"EchoHandler");



    return res_status;
}
