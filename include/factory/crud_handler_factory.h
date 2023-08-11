#include "request_handler_factory.h"

class CrudHandlerFactory : public RequestHandlerFactory
{
public:
    virtual RequestHandler * create(std::string location, std::string url, const NginxConfig & config);
private:
    void initialize_entity_map(std::string root);
    std::map<std::string, std::vector<int>> entity_ids_;
    std::mutex mutex_;
};