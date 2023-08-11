#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include "http_request_parser.h"
#include "socket_interface.h"
#include "request_dispatcher.h"

using boost::asio::ip::tcp;

class session
{
public:
    session(SessionSocket& socket, RequestDispatcher& request_dispatcher);
    virtual void start();
    virtual SessionSocket* get_socket();
private:
    // read request
    void handle_read_header(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_read_body(const boost::system::error_code& error, size_t bytes_transferred);
    
    // generate response
    void respond_to_request();
    void respond_to_request_client_error();
    void respond_to_request_server_error();
    
    // write
    void handle_write(const boost::system::error_code& error);

    // socket
    SessionSocket * socket_;

    // request handler
    HttpRequestParser request_parser_;
    
    // request parser
    RequestDispatcher& request_dispatcher_;

    // numeric variables
    enum { max_length = 1024 }; 
    char buffer_[max_length];
    int content_length_ = 0;

    // request text
    std::string request_header_;
    std::string request_body_;
    std::string request_string_;
    std::string response_string_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};
#endif // SESSION_H