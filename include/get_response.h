#ifndef GET_RESPONSE_H
#define GET_RESPONSE_H

#include "http_response.h"

class GetResponse{
public:
    static std::string http_echo(const std::string& text);
    static std::string http_file(const std::string& type, const std::string& body);
    static std::string http_client_side_error();
    static std::string http_not_found();
    static std::string http_server_error();

};
#endif // GET_RESPONSE_H