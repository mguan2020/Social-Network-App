#ifndef MEME_HANDLER_NEW_H
#define MEME_HANDLER_NEW_H

#include "request_handler.h"
#include "meme_module.h"
#include "logger.h"
#include <memory>
#include <boost/filesystem.hpp>
#include <unordered_map>

class MemeModule;
class MemeHandler : public RequestHandler
{
public:
    MemeHandler(std::string location, std::string root, std::shared_ptr<MemeModule> memes);
    status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) override;
    status handle_post_comment_form(const http::request<http::string_body>& req, http::response<http::string_body>& res);

private:
    status handle_post(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_get(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_creation_form(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_get_random_meme(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_get_meme_by_id(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_get_all_memes(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_get_meme_by_views(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_get_meme_by_rating(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_post_meme(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_post_comment(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    status handle_get_other_info_by_id(const http::request<http::string_body>& req, http::response<http::string_body>& res);
    
    bool check_format(const std::string& target, const std::string& url);
    bool check_format_floating(const std::string& target, const std::string& url);
    std::string location_;
    std::string root_;
    std::string form_;
    std::string comment_form_;
    std::shared_ptr<MemeModule> memes_;
    std::unordered_map<std::string, std::string> template_to_url_;
};

#endif // HEALTH_HANDLER_NEW_H 