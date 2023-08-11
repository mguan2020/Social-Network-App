#include <unordered_map>
#include "request_handler_factory.h"
#include "meme_module.h"
#include <memory>
#include <string>
#include "boost_file_system.h"

class MemeHandlerFactory : public RequestHandlerFactory
{
public:
    MemeHandlerFactory();
    virtual RequestHandler * create(std::string location, std::string url, const NginxConfig & config);
private:
    // meme_modules for each root
    std::unordered_map<std::string, std::shared_ptr<MemeModule>> meme_modules_;
    // shared file system access
    std::shared_ptr<FileSystem> fs_;
};