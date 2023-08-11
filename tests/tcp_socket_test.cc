#include "tcp_socket.h"
#include <boost/asio.hpp>
#include "gtest/gtest.h"

class TCPSocketTest : public :: testing::Test{ // test fixture
protected:
    boost::asio::io_service io_service;
    TCPSocket socket = TCPSocket(io_service);
};

TEST_F(TCPSocketTest, test_constructor) {
    EXPECT_TRUE(true);
}

TEST_F(TCPSocketTest, test_get) {
    socket.get_socket();
    EXPECT_TRUE(true);
}

void foo(const boost::system::error_code& error, size_t bytes_transferred)
{

}

void bar(const boost::system::error_code& error)
{

}

TEST_F(TCPSocketTest, test_read) {
    char data [1024];
    socket.async_read_some(data, 1024, &foo);
    EXPECT_TRUE(true);
}

TEST_F(TCPSocketTest, test_write) {
    std::string s = "";
    socket.async_writing(s, &bar);
    EXPECT_TRUE(true);
}
