#include "tcp_socket.h"
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;

TCPSocket::TCPSocket(boost::asio::io_service & io_service)
    : socket_(io_service)
{    
}

void TCPSocket::async_read_some(char * data, size_t max_length, const std::function<void(const boost::system::error_code&, size_t)> & func)
{
  socket_.async_read_some(boost::asio::buffer(data, max_length),
      boost::bind(func,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TCPSocket::async_writing(std::string & str, const std::function<void(const boost::system::error_code&)> & func)
{
    async_write(
        socket_,
        boost::asio::buffer(str),
        boost::bind(func,
            boost::asio::placeholders::error));    
}

void TCPSocket::shutdown()
{
    socket_.shutdown(tcp::socket::shutdown_both);
}

tcp::socket & TCPSocket::get_socket()
{
    return socket_;
}

std::string TCPSocket::get_remote_ip()
{
    return socket_.remote_endpoint().address().to_string();
}