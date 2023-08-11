#ifndef REQUEST_HANDLER_FACTORY_H
#define REQUEST_HANDLER_FACTORY_H

#include "../config_parser.h"
#include "../handler/request_handler.h"
#include <string>

class RequestHandlerFactory
{
public:
    virtual RequestHandler * create(std::string location, std::string url, const NginxConfig & config) = 0;
    std::string removeTrailingSlashes(std::string input);
};

#endif //REQUEST_HANDLER_FACTORY_H