#include "factory/meme_handler_factory.h"
#include "handler/meme_handler.h"
#include "meme_module.h"

MemeHandlerFactory::MemeHandlerFactory()
{

}

RequestHandler * MemeHandlerFactory::create(std::string location, std::string url, const NginxConfig & config)
{
    // get file system
    BoostFileSystem bfs;
    fs_ = bfs.get_file_system();

    // create handler
    for(auto statement : config.statements_){
        if(statement->tokens_.size() != 2)
        {
            return nullptr;
        }
        if(statement->tokens_[0] != "root")
        {
            return nullptr;           
        }
        std::string root =  removeTrailingSlashes(statement->tokens_[1]);
        // if meme module doesn't exist create new one
        if (meme_modules_.find(root) == meme_modules_.end())
            meme_modules_[root] = std::make_shared<MemeModule>(root, fs_);
        return new MemeHandler(location, root, meme_modules_[root]);
    }
    return nullptr; //No Statement
}