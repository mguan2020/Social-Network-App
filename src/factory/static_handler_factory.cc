#include "factory/static_handler_factory.h"
#include "handler/static_handler.h"



RequestHandler * StaticHandlerFactory::create(std::string location, std::string url, const NginxConfig & config)
{
    for(auto statement : config.statements_){
        if(statement->tokens_.size() != 2)
        {
            return nullptr;
        }
        if(statement->tokens_[0] != "root")
        {
            return nullptr;           
        }
        std::string file_dir =  removeTrailingSlashes(statement->tokens_[1]);
        if(!(url.substr(0, location.length()) == location))// if url does not contain location as a substring
        {
            return nullptr;
        }
        url.replace(0, location.length(), file_dir);
        return new StaticHandler(url);
    }
    return nullptr; //No Statement
}