#include "handler/static_handler.h"
#include "mime.h"
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include "logger.h"

StaticHandler::StaticHandler(std::string file_name) 
    : file_name_(file_name)
{}

status StaticHandler::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    Logger* logger = Logger::getLogger();
    logger->log_info("Opening " + file_name_);
    
    // read file
    std::ifstream file(file_name_.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        logger->log_warning("File " + file_name_ + " not found");
        logger->log_response_data(404,req.target().to_string(),"StaticHandler");
        return http::status::not_found;
    }
    std::string content;
    char c;
    while (file.get(c)) content += c;
    file.close();

    // set response header
    status res_status = http::status::ok;
    int res_version = 11;
    std::string res_content_type = MIME::get_mime_type(file_name_);

    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, res_content_type);

    // set response body
    res.body() = content;
    logger->log_response_data(200,req.target().to_string(),"StaticHandler");

    // prepare response
    res.prepare_payload();

    

    return res_status;
}
