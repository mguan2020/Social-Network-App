//interface for sockets
#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H

#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class SessionSocket
{
public:
    // Read some data and then execute the function, based on the error code and size which will be automatically generated
    virtual void async_read_some(char * data, size_t max_length, const std::function<void(const boost::system::error_code&, size_t)>&func) = 0;
    // Write to the socket and then execute the function, based on the error code automatically generated
    virtual void async_writing(std::string & str, const std::function<void(const boost::system::error_code&)>&func) = 0;
    // Shutdown
    virtual void shutdown() = 0;
    // get socket
    virtual tcp::socket & get_socket() = 0;
    // get remote ip
    virtual std::string get_remote_ip() = 0;
    // destructor
    virtual ~SessionSocket(){}
};

#endif // SOCKET_INTERFACE_H
