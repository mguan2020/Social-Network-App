#include "handler/crud_handler.h"
#include <exception>
#include <sstream>
#include <boost/filesystem.hpp>
#include <string>
#include <type_traits>
#include "logger.h"

CrudHandler::CrudHandler(std::string root, std::map<std::string, std::vector<int>>& entity_ids, std::mutex& mutex)
    : root_(root), entity_ids_(entity_ids), mutex_(mutex) {
    }

void CrudHandler::prepare_created_response(http::response<http::string_body>& res, int id) {
    res.result(http::status::created);
    res.set(http::field::content_type, "text/plain");
    res.body() = "Created Entity {\"id\":" + std::to_string(id) + "}" + "\n";
    res.prepare_payload();
}

void CrudHandler::prepare_bad_response(http::response<http::string_body>& res, std::string body) {
    res.result(http::status::bad_request);
    res.set(http::field::content_type, "text/plain");
    res.body() = body;
    res.prepare_payload();
}

void CrudHandler::prepare_deleted_response(http::response<http::string_body>& res, int id) {
    res.result(http::status::ok);
    res.set(http::field::content_type, "text/plain");
    res.body() = "Deleted Entity {\"id\":" + std::to_string(id) + "}" + "\n";
    res.prepare_payload();
}

void CrudHandler::prepare_update_response(http::response<http::string_body>& res, int id) {
    res.result(http::status::ok);
    res.set(http::field::content_type, "text/plain");
    res.body() = "Update Entity {\"id\":" + std::to_string(id) + "}" + "\n";
    res.prepare_payload();
}

void CrudHandler::prepare_server_error_response(http::response<http::string_body>& res, std::string body) {
    res.result(http::status::internal_server_error);
    res.set(http::field::content_type, "text/plain");
    res.body() = body;
    res.prepare_payload();
}

int CrudHandler::get_available_id(std::string dir) {
    int current_id = 1;
    // check if the Entity exists yet
    if (entity_ids_.find(dir) == entity_ids_.end()) {
        // it doesnt
        return current_id;        
    }
    
    int i = 0;
    while (i < entity_ids_[dir].size()) {
        if (entity_ids_[dir][i] == current_id) {
            current_id++;
        }
        else {
            break;
        }
        i++;
    }
    return current_id;
}

void CrudHandler::add_id_to_entity(std::string dir, int id) {
    // check if the Entity exists yet
    if (entity_ids_.find(dir) == entity_ids_.end()) {
        // it doesnt
        entity_ids_[dir] = {id};
    }
    else {
        if (std::find(entity_ids_[dir].begin(), entity_ids_[dir].end(), id) == entity_ids_[dir].end()) {
            entity_ids_[dir].push_back(id);
            std::sort(entity_ids_[dir].begin(), entity_ids_[dir].end());
        }
        else {
            Logger* logger = Logger::getLogger();
            logger->log_info("Failed to add Entity ID: " + std::to_string(id) + " to Entity directory: " + dir + " ID already exists.");
        }        
    }
}

bool CrudHandler::remove_id_from_entity(std::string dir, int id) {
    Logger* logger = Logger::getLogger();
    // check if the Entity exists
    if (entity_ids_.find(dir) != entity_ids_.end()) {
        // it does
        std::vector<int>::iterator id_it = std::find(entity_ids_[dir].begin(), entity_ids_[dir].end(), id);
        // check if the id exists
        if (id_it != entity_ids_[dir].end()) {
            // it does
            entity_ids_[dir].erase(id_it);
            logger->log_info("Entity named " + dir + "with ID: " + std::to_string(id) + " successfully deleted from entity map.");
            return true;
        }
        else {
            logger->log_info("Entity with ID: " + std::to_string(id) +  " was not found.");
        }
    }
    else {
        logger->log_info("Entity does not exist.");
    }
    return false;
}

status CrudHandler::handle_creation(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::lock_guard<std::mutex> lock(mutex_);
    Logger* logger = Logger::getLogger();
    std::string target = std::string(req.target());
    size_t last_slash = target.find_last_of("/");
    std::string target_dir = "";

    if (last_slash != std::string::npos && last_slash + 1 < target.length()) {
        target_dir = target.substr(last_slash + 1);
    }
    else {
        std::string reason = "Invalid path format for Create Entity request.";
        prepare_bad_response(res, reason);
        logger->log_info(reason);
         logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }
    std::string path = root_ + "/" + target_dir;
    logger->log_info("Creating Entity in directory: " + path);

    if (!boost::filesystem::exists(path) || !boost::filesystem::is_directory(path)) {
        try {
            boost::filesystem::create_directory(path);
        } catch (const boost::filesystem::filesystem_error& e) {
            std::string reason = "Creation of directory for Entity failed.";
            prepare_bad_response(res, reason);
            logger->log_info(reason);
            logger->log_response_data(400,req.target().to_string(), "CrudHandler");
            return http::status::bad_request;
        }
    }

    // prepare path for new entity
    path += "/";
    int id = get_available_id(target_dir);
    path += std::to_string(id);

    // prepare content to be writen to the entity
    std::ostringstream oss;
    oss << req.body();
    std::string req_body = oss.str();
    oss.clear();

    std::ofstream entity_file(path);
    if (entity_file) {
        entity_file << req_body;
        entity_file.close();
        prepare_created_response(res, id);
        add_id_to_entity(target_dir, id);
        logger->log_info("Entity created successfully at: " + path + " with id: " + std::to_string(id) + ".");
        logger->log_response_data(201, req.target().to_string(), "CrudHandler");
        return http::status::created;
    }
    std::string reason = "Failed to write to file during Entity creation at : " + path;
    prepare_bad_response(res, reason);
    logger->log_info(reason);
    logger->log_response_data(400,req.target().to_string(), "CrudHandler");
    return http::status::bad_request;    
}

status CrudHandler::handle_deletion(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::lock_guard<std::mutex> lock(mutex_);
    Logger* logger = Logger::getLogger();
    std::string target = std::string(req.target());
    size_t last_slash = target.find_last_of("/");
    size_t entity_slash = std::string::npos;
    std::string target_entity_and_id = "";
    std::string target_entity = "";
    std::string target_id = "";

    if (last_slash != std::string::npos && last_slash + 1 < target.length()) {
        entity_slash = target.find_last_of("/", last_slash - 1);
    }
    else {
        std::string reason = "Invalid path format for Delete Entity request.";
        prepare_bad_response(res, reason);
        logger->log_info(reason);
        logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }


    if (entity_slash != std::string::npos && entity_slash + 1 < target.length()) {
        target_entity_and_id = target.substr(entity_slash + 1);
        target_entity = target.substr(entity_slash + 1, last_slash - entity_slash - 1);
        target_id = target.substr(last_slash + 1);
    }
    else {
        std::string reason = "Invalid path format for Delete Entity request.";
        prepare_bad_response(res, reason);
        logger->log_info(reason);
        logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }

    std::string path = root_ + "/" + target_entity_and_id;

    if (boost::filesystem::exists(path) && !boost::filesystem::is_directory(path)) {
        int id;
        try {
            id = std::stoi(target_id);
        } catch (const std::invalid_argument& e) {
            std::string reason = "Expected a number for Entity ID, did not get a number.";
            prepare_bad_response(res, reason);
            logger->log_info(reason);
            logger->log_response_data(400,req.target().to_string(), "CrudHandler");
            return http::status::bad_request;
        }
        if (remove_id_from_entity(target_entity, id)) {
            try {
                boost::filesystem::remove(path);
                logger->log_info("Respective entity file was deleted as well.");
            } catch (const boost::filesystem::filesystem_error& e) {
                std::string reason = "Could not delete file assosciated with deleted Entity";
                prepare_server_error_response(res, reason);
                logger->log_info(reason);
                logger->log_response_data(500,req.target().to_string(), "CrudHandler");
                return http::status::internal_server_error;
            }
            prepare_deleted_response(res, id);
            logger->log_response_data(200, req.target().to_string(), "CrudHandler");
            return http::status::ok;
        }
        else {
            std::string reason = "Failed to Delete Entity at : " + path;
            prepare_bad_response(res, reason);
            logger->log_info(reason);
            logger->log_response_data(400,req.target().to_string(), "CrudHandler");
            return http::status::bad_request;
        }
    }
    std::string reason = "Failed to find file during Entity Deletion at : " + path;
    prepare_bad_response(res, reason);
    logger->log_info(reason);
    logger->log_response_data(400,req.target().to_string(), "CrudHandler");
    return http::status::bad_request;
}

bool CrudHandler::entity_exists(std::string dir, int id) {
    auto iter = entity_ids_.find(dir);
    if (iter == entity_ids_.end()) {
        return false;
    }
    //get ids from map
    std::vector<int>& ids = iter->second;
    return std::find(ids.begin(), ids.end(), id) != ids.end();
}

bool CrudHandler::ends_with_id(std::string target) {
    std::size_t lastSlashPosition = target.find_last_of("/");
    std::string possible_id = target.substr(lastSlashPosition + 1);
    try {
        std::stoi(possible_id);
        return true;
    } catch (const std::invalid_argument& ia) {
        return false;
    }
}

status CrudHandler::list_entities(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    Logger* logger = Logger::getLogger();
    //get entity id from request
    std::string target = std::string(req.target());
    std::size_t lastSlashPosition = target.find_last_of("/");

    //list
    //return list of files in directory
    std::string dir = target.substr(lastSlashPosition + 1);
    std::string path = root_ + "/" + dir;

    if (entity_ids_.find(dir) == entity_ids_.end()) {
        std::string reason = "Entity dir: " + dir + " does not exist.";
        prepare_bad_response(res, reason);
        logger->log_info(reason);
        logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }

    std::vector<int>& ids = entity_ids_[dir];
    std::string json_array = "[";
    for (int i = 0; i < ids.size(); i++) {
        json_array += std::to_string(ids[i]);
        if (i != ids.size() - 1) {
            json_array += ",";
        }
    }
    json_array += "]";

    res.set(http::field::content_type, "application/json");
    res.body() = json_array;
    res.prepare_payload();
    logger->log_info("Entity list retrieved successfully for dir: " + dir);
    logger->log_response_data(200,req.target().to_string(), "CrudHandler");
    return http::status::ok;
}

status CrudHandler::retrieve_entity(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    Logger* logger = Logger::getLogger();
    //get entity id from request
    std::string target = std::string(req.target());
    std::size_t lastSlashPosition = target.find_last_of("/");

    //retrieve
    //get dir (substring between last two slashes)
    std::string entity_id = target.substr(lastSlashPosition + 1);
    logger->log_info("Retrieving entity with id: " + entity_id);
    int entity_id_int = std::stoi(entity_id);
    std::string dir = "";
    if (lastSlashPosition != std::string::npos && lastSlashPosition > 0) {
        std::size_t secondLastSlashPosition = target.rfind('/', lastSlashPosition - 1);
        if (secondLastSlashPosition != std::string::npos) {
            dir = target.substr(secondLastSlashPosition + 1, lastSlashPosition - secondLastSlashPosition - 1);
        }
    }

    if (!entity_exists(dir, entity_id_int)) {
        std::string reason = "Entity with id: " + entity_id + " does not exist in dir " + dir;
        prepare_bad_response(res, reason);
        logger->log_info(reason);
        logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }    
    logger->log_info("Found entity with id: " + entity_id + " in directory: " + dir);
    std::string full_path = root_ + "/" + dir + "/" + entity_id;

    std::ifstream entity_file(full_path);
    if (!entity_file) {
        std::string reason = "Failed to open entity file at: " + full_path;
        prepare_bad_response(res, reason);
        logger->log_info(reason);
         logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }

    std::string entity_contents((std::istreambuf_iterator<char>(entity_file)), std::istreambuf_iterator<char>());

    res.set(http::field::content_type, "application/json");
    res.body() = entity_contents;
    res.prepare_payload();

    logger->log_info("Successfully retrieved entity with id: " + entity_id + " from directory: " + dir);
    logger->log_response_data(200,req.target().to_string(), "CrudHandler");
    return http::status::ok;

}

status CrudHandler::handle_get(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::string target = std::string(req.target());

    if (!ends_with_id(target)) {
        return list_entities(req, res);
    }
    else {
        return retrieve_entity(req, res);
    }
}

status CrudHandler::handle_update(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    std::lock_guard<std::mutex> lock(mutex_);
    Logger* logger = Logger::getLogger();
    std::string target = std::string(req.target());
    // find target id
    size_t last_slash = target.find_last_of("/");
    int target_id;
    if (last_slash != std::string::npos && last_slash + 1 < target.length()) {
        std::string id = target.substr(last_slash + 1);
        try {
            target_id = std::stoi(id);
        } catch (const std::invalid_argument& e) {
            std::string reason = "Expected a number for Entity ID, did not get a number.";
            prepare_bad_response(res, reason);
            logger->log_info(reason);
             logger->log_response_data(400,req.target().to_string(), "CrudHandler");
            return http::status::bad_request;
        }
    }
    else {
        std::string reason = "Invalid path format for Create Entity request.";
        prepare_bad_response(res, reason);
        logger->log_info(reason);
         logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }
    // find target dir
    std::string target_url_dir = target.substr(0, target.length()-std::to_string(target_id).length()-1);
    last_slash = target_url_dir.find_last_of("/");
    std::string target_dir = "";
    if (last_slash != std::string::npos && last_slash + 1 < target_url_dir.length()) {
        target_dir = target_url_dir.substr(last_slash + 1);
    }
    else {
        std::string reason = "Invalid path format for Create Entity request.";
        prepare_bad_response(res, reason);
        logger->log_info(reason);
        logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }
    // construct the complete path
    std::string path = root_ + "/" + target_dir + "/" + std::to_string(target_id);
    if (!boost::filesystem::exists(path) || 
        !boost::filesystem::exists(root_ + "/" + target_dir) || 
        (boost::filesystem::exists(root_ + "/" + target_dir) && !boost::filesystem::is_directory(root_ + "/" + target_dir))) {
        // file does not exist, create a new one
        logger->log_info("Update a non-exist file, creating a new entity.");
        try {
            boost::filesystem::create_directory(root_ + "/" + target_dir);
        } catch (const boost::filesystem::filesystem_error& e) {
            std::string reason = "Creation of directory for Entity failed.";
            prepare_bad_response(res, reason);
            logger->log_info(reason);
            logger->log_response_data(400,req.target().to_string(), "CrudHandler");
            return http::status::bad_request;
        }
        std::ostringstream oss;
        oss << req.body();
        std::string req_body = oss.str();
        oss.clear();
        // overwriting the existing file
        std::ofstream entity_file(path, std::ofstream::trunc);
        if (entity_file) {
            entity_file << req_body;
            entity_file.close();
            prepare_created_response(res, target_id);
            add_id_to_entity(target_dir, target_id);
            logger->log_info("Entity updated successfully at: " + path);
            logger->log_response_data(201, req.target().to_string(), "CrudHandler");
            return http::status::created;
        }
        // target source faield to open and write
        std::string reason = "Failed to overwrite the file at : " + path;
        prepare_bad_response(res, reason);
        logger->log_info(reason);
        logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    } else {
        // prepare content to be writen to the entity
        std::ostringstream oss;
        oss << req.body();
        std::string req_body = oss.str();
        oss.clear();
        // overwriting the existing file
        std::ofstream entity_file(path, std::ofstream::trunc);
        if (entity_file) {
            entity_file << req_body;
            entity_file.close();
            prepare_update_response(res, target_id);
            logger->log_info("Entity updated successfully at: " + path);    
            logger->log_response_data(200,req.target().to_string(), "CrudHandler");        
            return http::status::ok;
        }
        // target source faield to open and write
        std::string reason = "Failed to overwrite the file at : " + path;
        prepare_bad_response(res, reason);
        logger->log_info(reason);
        logger->log_response_data(400,req.target().to_string(), "CrudHandler");
        return http::status::bad_request;
    }
}

status CrudHandler::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    Logger* logger = Logger::getLogger();
    logger->log_info("Starting CRUD handler");

    http::verb method = req.method();

    if (method == http::verb::post) {
        return handle_creation(req, res);
    }
    else if (method == http::verb::get){
        return handle_get(req, res);
    }
    else if (method == http::verb::put){
        return handle_update(req, res);
    }
    else if (method == http::verb::delete_){
        return handle_deletion(req, res);
    }
    else{
        logger->log_debug("Unknown HTTP method.");
    }

    // placeholder to guarantee a return

    logger->log_response_data(400,req.target().to_string(), "CrudHandler");
    return http::status::bad_request;
}