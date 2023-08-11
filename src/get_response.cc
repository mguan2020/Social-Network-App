#include "get_response.h"

// TODO: MIME types, corresponding functions

std::string GetResponse::http_echo(const std::string& text)
{
    HttpResponse response(200, "HTTP/1.1");
    response.add_header("Content-Type", "text/plain");
    response.add_header("Content-Length", std::to_string(text.size()));
    response.set_body(text);
    return response.to_string();
}

std::string GetResponse::http_client_side_error()
{
    HttpResponse response(400, "HTTP/1.1");
    return response.to_string();
}

std::string GetResponse::http_not_found()
{
    HttpResponse response(404, "HTTP/1.1");
    return response.to_string();
}

std::string GetResponse::http_server_error()
{
    HttpResponse response(500, "HTTP/1.1");
    return response.to_string();
}

std::string GetResponse::http_file(const std::string& type, const std::string& body) {
    HttpResponse response(200, "HTTP/1.1");
    response.add_header("Content-Type", type);
    response.add_header("Content-Length", std::to_string(body.size()));
    response.set_body(body);
    return response.to_string();
}
