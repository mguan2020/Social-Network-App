#include "factory/request_handler_factory_factory.h"
#include "factory/echo_handler_factory.h"
#include "factory/static_handler_factory.h"
#include "factory/file_not_found_handler_factory.h"
#include "factory/crud_handler_factory.h"
#include "factory/health_handler_factory.h"
#include "factory/block_handler_factory.h"
#include "factory/meme_handler_factory.h"


RequestHandlerFactory* RequestHandlerFactoryFactory::createHandlerFactory(const std::string& name)
{
if (name == "StaticHandler")
    return new StaticHandlerFactory();
if (name == "EchoHandler")
    return new EchoHandlerFactory();
if (name == "FileNotFoundHandler")
    return new FileNotFoundHandlerFactory();
if (name == "CrudHandler")
    return new CrudHandlerFactory();
if(name == "HealthHandler")
    return new HealthHandlerFactory();
if (name == "BlockHandler")
    return new BlockHandlerFactory();
if (name == "MemeHandler")
    return new MemeHandlerFactory();
return nullptr;
}