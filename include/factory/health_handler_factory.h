#include "request_handler_factory.h"

class HealthHandlerFactory : public RequestHandlerFactory
{
public:
    virtual RequestHandler * create(std::string location, std::string url, const NginxConfig & config);
};