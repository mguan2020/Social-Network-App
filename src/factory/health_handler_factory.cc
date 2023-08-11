#include "factory/health_handler_factory.h"
#include "handler/health_handler.h"

  
RequestHandler * HealthHandlerFactory::create(std::string location, std::string url, const NginxConfig & config)
{
    return new HealthHandler(location,url);
}