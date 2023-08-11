#include "factory/session_factory.h"
#include "session.h"
#include "tcp_socket.h"

session* SessionFactory::create(boost::asio::io_service & io_service, RequestDispatcher& request_dispatcher)
{
    TCPSocket* new_socket = new TCPSocket(io_service);
    return (new session(*new_socket, request_dispatcher)); 
}