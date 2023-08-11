#include "request_handler_factory.h"

class RequestHandlerFactoryFactory
{
public:
    static RequestHandlerFactory* createHandlerFactory(const std::string& name);
};
