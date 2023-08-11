// SessionSocket Implemented by boost::tcp
#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H
#include "socket_interface.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TCPSocket : public SessionSocket
{
public:
    TCPSocket(boost::asio::io_service & io_service);
    virtual void async_read_some(char * data, size_t max_length, const std::function<void(const boost::system::error_code&, size_t)> & func);
    virtual void async_writing(std::string & str, const std::function<void(const boost::system::error_code&)> & func);
    virtual void shutdown();
    tcp::socket & get_socket();
    std::string get_remote_ip();


private:
    tcp::socket socket_;
};

#endif // TCP_SOCKET_H