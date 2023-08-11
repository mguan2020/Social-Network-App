
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/asio.hpp>
#include "server.h"
#include "factory/session_factory.h"
#include "tcp_socket.h"

// Using fixture for mock test
//class ServerFixture : public ::testing::Test{
//protected:
//    mock_session mock_s;
//    short port = 8080;
//    boost::asio::io_service io_service;
//};

class MockSession: public session
{
public:
    MockSession(boost::asio::io_service& io_service, RequestHandler& rh) : session(*(new TCPSocket(io_service)), rh) {}
    void start() {}
    SessionSocket* get_socket() 
    {
        boost::asio::io_service io_service;
        return (new TCPSocket(io_service));
    }
};

class MockSessionFactory: public SessionFactory
{
public:
    session* create(boost::asio::io_service & io_service, RequestHandler& request_handler)
    {
        return (new MockSession(io_service, request_handler));
    }
};

class ServerTestFixture : public :: testing::Test{ // test fixture
public:
    ServerTestFixture()
    {
        session_factory = new MockSessionFactory();
    }
protected:
    MockSessionFactory* session_factory;
    short portno = 8080;
    boost::asio::io_service io_service;
};

TEST_F(ServerTestFixture, START_SESSION) {
    RequestHandler rh = RequestHandler();
    server s(io_service, portno, *session_factory, rh);
    EXPECT_TRUE(true);
}

TEST_F(ServerTestFixture, SESSION_FACTORY) {
    SessionFactory sf = SessionFactory();
    boost::asio::io_service io_service;
    RequestHandler rh = RequestHandler();
    session* s = sf.create(io_service, rh);
    EXPECT_TRUE(typeid(session*) == typeid(s));
}
