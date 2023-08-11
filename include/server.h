#ifndef SERVER_H
#define SERVER_H
#include "session.h"
#include "factory/session_factory.h"
#include "request_dispatcher.h"

class server {
public:
    server(boost::asio::io_service& io_service, short port, RequestDispatcher& request_dispatcher);

private:
    void start_accept();
    void handle_accept(session* new_session,
    const boost::system::error_code& error);
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    SessionFactory session_factory_;
    RequestDispatcher& request_dispatcher_;
};
#endif // SERVER_H

