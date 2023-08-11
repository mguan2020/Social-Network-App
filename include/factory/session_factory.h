#ifndef SESSION_FACTORY_H
#define SESSION_FACTORY_H
#include "session.h"
#include "tcp_socket.h"
#include "request_dispatcher.h"

class SessionFactory {
public:
    virtual session* create(boost::asio::io_service & io_service, RequestDispatcher& request_dispatcher);
};

#endif // SESSION_FACTORY_H


