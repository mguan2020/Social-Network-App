#include "factory/echo_handler_factory.h"
#include "handler/echo_handler.h"

RequestHandler * EchoHandlerFactory::create(std::string location, std::string url, const NginxConfig & config)
{
    return new EchoHandler();
}