#include <cstring>
#include <fstream>

#include "session.h"
#include "config_parser.h"
#include "gtest/gtest.h"

using boost::asio::ip::tcp;

class EasySocket : public SessionSocket
{
public:
    EasySocket(std::string in, std::string & out, boost::system::error_code err_write, boost::system::error_code err_read, int bytes_to_have_error = 0)
        : input_(in),  err_write_(err_write), err_read_(err_read), bytes_to_have_error_(bytes_to_have_error)
    {
        output_ = &out;
    }

    virtual ~EasySocket()
    {
        if (err_write_)
            *output_ = "Successfully Terminated.";
    } 

    virtual void async_read_some(char * data, size_t max_length, const std::function<void(const boost::system::error_code&, size_t)>&func)
    {
        int length = input_.length();
        if (length <= max_length)
        {
            std::strncpy(data, input_.c_str(), max_length);
            input_ = "";
            bytes_transferred_ = length;
        }
        else 
        {
            std::strncpy(data, input_.substr(0,max_length).c_str(),max_length);
            input_ = input_.substr(max_length, length - max_length);
            bytes_transferred_ = max_length;
        }
        if(bytes_to_have_error_<=0)
            func(err_read_, bytes_transferred_);
        else //force success
        {
            bytes_to_have_error_-=bytes_transferred_;
            func(boost::system::errc::make_error_code(boost::system::errc::success), bytes_transferred_);
        }
    }
    
    virtual void async_writing(std::string & str, const std::function<void(const boost::system::error_code&)>&func)
    {
        if(!err_write_)
            *output_ += str;
        func(err_write_);
    }
    
    virtual void shutdown()
    {        
    }

    tcp::socket & get_socket()
    {
        boost::asio::io_service io_service;
        tcp::socket* socket = new tcp::socket(io_service);
        return *socket;
    }

    virtual std::string get_remote_ip()
    {
        return "127.0.0.1";
    }

private:
    int bytes_to_have_error_;//Number of bytes that must have been read before meeting an error.
    std::string input_;
    std::string * output_;
    boost::system::error_code err_write_;
    boost::system::error_code err_read_;
    size_t bytes_transferred_;
};

class SessionTest : public ::testing::TestWithParam<std::string>
{ // test fixture
protected:
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    std::string out = "";
    std::string in;
    boost::system::error_code err_write;
    boost::system::error_code err_read;
    EasySocket * socket;
    session * sess;
    RequestDispatcher * rd;
    void SetUp()
    {
        in = (GetParam());
        err_write = boost::system::errc::make_error_code(boost::system::errc::success);
        err_read = boost::system::errc::make_error_code(boost::system::errc::success);
        socket = new EasySocket(in, out, err_write, err_read);
        NginxConfigParser parser;
        NginxConfig config;
        parser.Parse("../conf/test_config", &config);
        std::map<std::string, NginxConfig*> arguments = config.getArguments();
        std::map<std::string, RequestHandlerFactory*> factories = config.getFactories();
        rd = new RequestDispatcher(factories, arguments);
        sess = new session(*socket, *rd);
    }
    void TearDown()
    {
        //socket and sess should have deleted themselves
        delete rd;
    }

};

class SessionTestWriteFail : public SessionTest
{ // test fixture
protected:
    void SetUp() override
    {
        in = (GetParam());
        err_write = boost::system::errc::make_error_code(boost::system::errc::timed_out);
        err_read = boost::system::errc::make_error_code(boost::system::errc::success);
        socket = new EasySocket(in, out, err_write, err_read);
        NginxConfigParser parser;
        NginxConfig config;
        parser.Parse("../conf/test_config", &config);
        std::map<std::string, NginxConfig*> arguments = config.getArguments();
        std::map<std::string, RequestHandlerFactory*> factories = config.getFactories();
        rd = new RequestDispatcher(factories, arguments);
        sess = new session(*socket, *rd);
    }
};

class SessionTestReadFail : public ::testing::TestWithParam<std::tuple<std::string, int>>
{
protected:
 // test fixture
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    std::string out = "";
    std::string in;
    boost::system::error_code err_write;
    boost::system::error_code err_read;
    EasySocket * socket;
    session * sess;
    RequestDispatcher * rd;
    void SetUp() override
    {
        in = std::get<0>(GetParam());
        int bytes_to_have_error = std::get<1>(GetParam());        
        err_write = boost::system::errc::make_error_code(boost::system::errc::success);
        err_read = boost::system::errc::make_error_code(boost::system::errc::timed_out);
        socket = new EasySocket(in, out, err_write, err_read, bytes_to_have_error);
        NginxConfigParser parser;
        NginxConfig config;
        parser.Parse("../conf/test_config", &config);
        std::map<std::string, NginxConfig*> arguments = config.getArguments();
        std::map<std::string, RequestHandlerFactory*> factories = config.getFactories();
        rd = new RequestDispatcher(factories, arguments);
        sess = new session(*socket, *rd);
    }
};

class SessionTestParseFail : public SessionTest
{
public:
    SessionTestParseFail() {}
};

TEST_P(SessionTestWriteFail, output_test_response_write_fail) {
    sess->start();
    // EasySocket is Synchronous
    ASSERT_EQ(out, "Successfully Terminated.");
}

TEST_P(SessionTestReadFail, output_test_response_read_fail) {
    sess->start();
    // EasySocket is Synchronous
    bool error = out.find("500") != std::string::npos;
    ASSERT_TRUE(error);
    // ASSERT_EQ(out, response_string_);
}

TEST_P(SessionTestParseFail, output_test_response_parse_fail) {
    sess->start();
    
    // EasySocket is Synchronous
    bool error = out.find("400") != std::string::npos;
    ASSERT_TRUE(error);
    
}

TEST_P(SessionTest, output_test_response) {
    sess->start();
    // EasySocket is Synchronous
    bool sucess = out.find("200") != std::string::npos;
    ASSERT_TRUE(sucess);
}


// Some test parameters for successful cases
INSTANTIATE_TEST_SUITE_P(Success, SessionTest,
    ::testing::Values(
        "POST /echo HTTP/1.0\r\nHost: example.com\r\nContent-Length: 30\r\n\r\n123456789012345678901234567890", 
        "POST /echo HTTP/1.0\r\nHost: example.com\r\nContent-Length: 0\r\n\r\n",
        "POST /echo HTTP/1.0\r\nHost: example.com\r\n\r\n" 
    ));


INSTANTIATE_TEST_SUITE_P(WriteFail, SessionTestWriteFail,
    ::testing::Values(
        "GET /hello.htm HTTP/1.1 User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT) Host: www.tutorialspoint.com Accept-Language: en-us Accept-Encoding: gzip, deflate Connection: Keep-Alive\r\n\r\n"
    ));


INSTANTIATE_TEST_SUITE_P(ReadFail, SessionTestReadFail,
    ::testing::Values(
        std::make_tuple("GET /hello.htm HTTP/1.1 User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT) Host: www.tutorialspoint.com Accept-Language: en-us Accept-Encoding: gzip, deflate Connection: Keep-Alive\r\n\r\n", 0)    ));


INSTANTIATE_TEST_SUITE_P(ParseFail, SessionTestParseFail,
    ::testing::Values(
        "GET \r\nHost: example.com\r\n\r\n\r\n"
    ));
