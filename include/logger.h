#ifndef LOG_H
#define LOG_H
#include <string>
#include "http_request_parser.h"
#include "socket_interface.h"
class Logger
{
public:
    Logger();
    static void init();
    static Logger* logger;
    static Logger* getLogger();
    static void log_server_init();
    static void log_server_start(int port);
    static void log_server_stop();
    static void log_info(std::string message);
    static void log_debug(std::string message);
    static void log_error(std::string message);
    static void log_warning(std::string message);
    static void log_fatal(std::string message);
    static void log_http_request(const HttpRequestParser& request_parser, SessionSocket* socket);
    static void log_response_data(int res_code, std::string req_url, std::string handler_name);
};

#endif