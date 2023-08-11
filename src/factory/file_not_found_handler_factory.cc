#include "factory/file_not_found_handler_factory.h"
#include "handler/file_not_found_handler.h"

RequestHandler * FileNotFoundHandlerFactory::create(std::string location, std::string url, const NginxConfig & config)
{
    return new FileNotFoundHandler();
}