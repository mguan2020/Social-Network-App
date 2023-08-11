#include "factory/crud_handler_factory.h"
#include "handler/crud_handler.h"
#include "logger.h"
#include <boost/filesystem.hpp>


RequestHandler * CrudHandlerFactory::create(std::string location, std::string url, const NginxConfig & config)
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

        std::string root_dir = removeTrailingSlashes(statement->tokens_[1]);
        initialize_entity_map(root_dir);
        return new CrudHandler(root_dir, entity_ids_, mutex_);
    }
    return nullptr; //No Statement
}

void CrudHandlerFactory::initialize_entity_map(std::string root) {
    Logger* logger = Logger::getLogger();
    
    if (!boost::filesystem::exists(root) || !boost::filesystem::is_directory(root)) {
        logger->log_info("Could not find root directory for CRUD handler.");
    }
    else {
        logger->log_info("Filling entity map with current entities.");
        boost::filesystem::directory_iterator directory_it(root);
        // default constructed iterator represents the end interator
        boost::filesystem::directory_iterator end_it;
        // loop for the root directory to find all entity types
        for (;directory_it != end_it; directory_it++) {
            boost::filesystem::path directory_path = directory_it->path();

            if (boost::filesystem::is_directory(directory_path)) {

                boost::filesystem::directory_iterator entity_it(directory_path);
                // loop for each entity type to find all related ids
                for (;entity_it != end_it; entity_it++) {
                    boost::filesystem::path entity_path = entity_it->path();
                    int id;
                    try {
                        id = std::stoi(entity_path.filename().string());
                    } catch (const std::invalid_argument& e) {
                        logger->log_warning("Unexpected file name, not a number.");
                    }
                    std::string dir = directory_path.filename().string();
                    if (entity_ids_.find(dir) == entity_ids_.end()) {
                        // it doesnt
                        entity_ids_[dir] = {id};
                    } else {
                        entity_ids_[dir].push_back(id);
                        std::sort(entity_ids_[dir].begin(), entity_ids_[dir].end());
                    }
                }
            }
        }
        logger->log_info("Entity map initialized.");
    }
}