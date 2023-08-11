#include "factory/block_handler_factory.h"
#include "handler/block_handler.h"

RequestHandler * BlockHandlerFactory::create(std::string location, std::string url, const NginxConfig & config)
{
    return new BlockHandler();
}