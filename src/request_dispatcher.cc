#include "request_dispatcher.h"
#include "get_response.h"
#include "logger.h"

using namespace std;

RequestDispatcher::RequestDispatcher(const std::map<std::string, RequestHandlerFactory*> & handler_factories, const std::map<std::string, NginxConfig*> & path2arguments)
    : handler_factories_(handler_factories), path2arguments_(path2arguments)
{
}

void RequestDispatcher::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    Logger* logger = Logger::getLogger();
    // process the url
    std::string url = req.target().to_string();

    // match the best handler
    const std::string * location_ptr = match(url);

    // no matching handler for requested url
    if (!location_ptr)
    {  
        res.version(11);
        res.result(http::status::not_found);
        res.set(http::field::content_type, "text/plain");

        // set response body
        res.body() = "404 Not Found\r\n\r\n";

        // prepare response
        res.prepare_payload();
        logger->log_response_data(404,url,"N/A");
        return;
    }
    std::string location = *location_ptr;

    // log dispatch info 
    logger->log_info("Dispatch " + url + " to " + location);

    const NginxConfig * arguments = path2arguments_[location];
    RequestHandlerFactory * factory = handler_factories_[location];
    RequestHandler* handler = factory->create(location, url, *arguments);
    // get response
    status http_status = handler->handle_request(req, res);
    logger->log_info("Handler returned status " + std::to_string(static_cast<int>(http_status)));
    // manage status. For now, it seems like handler_request itself already handles all the error status. 
    delete handler;
    switch (http_status)
    {
        case status::not_found:
            res.version(11);
            res.result(http::status::not_found);
            res.set(http::field::content_type, "text/plain");
            res.body() = "404 Not Found\r\n\r\n";
            return;
        case status::internal_server_error:
            res.version(11);
            res.result(http::status::internal_server_error);
            res.set(http::field::content_type, "text/plain");
            res.body() = "500 Internal Server Error\r\n\r\n";
            return;
        case status::bad_request:
            res.version(11);
            res.result(http::status::bad_request);
            res.set(http::field::content_type, "text/plain");
            res.body() = "400 Bad Request\r\n\r\n";
            return;
        default:
            return;
    }
}

const std::string * RequestDispatcher::match(std::string url)
{
    Logger* logger = Logger::getLogger();
    int longest_prefix_match = 0;
    const std::string * best_match = nullptr;
    // find the handler that best matches the url
    for ( const auto &[prefix, handler_factory]: handler_factories_) 
    {
        // match url with prefix
        auto match = std::mismatch(prefix.begin(), prefix.end(), url.begin());
        // check if handler prefix matches the prefix of url
        // and if it is the longest match
        if (match.first == prefix.end() && prefix.length() >= longest_prefix_match)
        {
            // update best handler
            longest_prefix_match = prefix.length();
            best_match = &prefix;
        }
    }

    return best_match;
}