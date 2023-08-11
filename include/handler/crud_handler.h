#ifndef CRUD_HANDLER_NEW_H
#define CRUD_HANDLER_NEW_H

#include "request_handler.h"
#include <string>

class CrudHandler : public RequestHandler
{
public:
    CrudHandler(std::string root, std::map<std::string, std::vector<int>>& entity_ids, std::mutex& mutex);
    status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) override;
    status handle_get(const http::request<http::string_body>& req, http::response<http::string_body>& res);
private:
    void prepare_created_response(http::response<http::string_body>& res, int id);
    void prepare_bad_response(http::response<http::string_body>& res, std::string body);
    void prepare_deleted_response(http::response<http::string_body>& res, int id);
    void prepare_update_response(http::response<http::string_body>& res, int id);
    void prepare_server_error_response(http::response<http::string_body>& res, std::string body);
    int get_available_id(std::string dir);
    void add_id_to_entity(std::string dir, int id);
    bool remove_id_from_entity(std::string dir, int id);
    status handle_creation(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_deletion(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    bool entity_exists(std::string dir, int id);
    bool ends_with_id(std::string target);
    status retrieve_entity(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status list_entities(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_update(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    std::string root_;
    std::map<std::string, std::vector<int>>& entity_ids_;
    std::mutex& mutex_;
};

#endif // CRUD_HANDLER_NEW_H