#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "logger.h"
#include "session.h"
#include "get_response.h"

// add
using boost::asio::ip::tcp;

session::session(SessionSocket& socket, RequestDispatcher& request_dispatcher)
  : request_dispatcher_(request_dispatcher)
{
    socket_ = &socket;
}

void session::start()
{
    socket_->async_read_some(
        buffer_, max_length,
        boost::bind(&session::handle_read_header, this,
            boost::placeholders::_1,
            boost::placeholders::_2));
}

void session::handle_read_header(const boost::system::error_code& error, size_t bytes_transferred)
{
  Logger* logger = Logger::getLogger();
  if (!error)
    { 
        std::string str(buffer_, bytes_transferred);
        request_string_ += str;

        // check if finished reading header
        std::size_t header_pos = request_string_.find("\r\n\r\n");
        if (header_pos != std::string::npos) {
            request_header_ = request_string_.substr(0, header_pos+4);
            request_body_ = request_string_.substr(header_pos+4, bytes_transferred);
            // process header
            if (request_parser_.parse(request_header_)) 
            {
              logger->log_info("process header");
              logger->log_http_request(request_parser_, socket_);
              // needs to read body
              if (request_parser_.get_method() == "POST" || request_parser_.get_method() == "PUT") 
              {
                logger->log_info("read body");
                // parse header content length
                std::string content_length_str = request_parser_.get_header("Content-Length");
                if (content_length_str != "")
                  content_length_ = std::stoi(content_length_str);

                // check if entire body is read
                if (request_body_.size() >= content_length_) 
                {
                  logger->log_info("finish reading body");
                  respond_to_request();
                }
                else 
                {
                  // read the body
                  socket_->async_read_some(
                      buffer_, max_length,
                      boost::bind(&session::handle_read_body, this,
                          boost::placeholders::_1,
                          boost::placeholders::_2));
                }
              }
              else // no body
              {
                // respond normally
                respond_to_request();
              }
            } 
            else 
            {
                // respond error message
                logger->log_warning("Bad Request: " + request_header_);
                respond_to_request_client_error();
            }
        } else {
            // continue reading
            socket_->async_read_some(
                buffer_, max_length,
                boost::bind(&session::handle_read_header, this,
                    boost::placeholders::_1,
                    boost::placeholders::_2));
        }
    }
    else
    {
      logger->log_warning("Server error handling: " + request_header_);
      respond_to_request_server_error();
    }
}

void session::handle_read_body(const boost::system::error_code& error, size_t bytes_transferred)
{
  Logger* logger = Logger::getLogger();
  if (!error)
    {
        // append new data
        std::string str(buffer_, bytes_transferred);
        request_body_ += str;

        // check if entire body is read
        if (request_body_.size() >= content_length_) 
        {
          logger->log_info("Finished reading body");
          respond_to_request();
        } 
        else 
        {
          // continue to read
          socket_->async_read_some(
            buffer_, max_length,
            boost::bind(&session::handle_read_body, this,
                boost::placeholders::_1,
                boost::placeholders::_2));
        }
    }
    else
    {
      Logger* logger = Logger::getLogger();
      logger->log_warning("Server error handling: " + request_header_);
      respond_to_request_server_error();
    }
}

void session::respond_to_request()
{
  Logger* logger = Logger::getLogger();
  logger->log_info("Respond to request");
  logger->log_info("Parse request");
  // parse request_string_ into request object
  http::request_parser<http::string_body> parser;
  boost::beast::error_code error_code;
  parser.eager(true);
  parser.put(boost::asio::buffer(std::string(request_header_ + request_body_)), error_code);
  
  // request parsing error
  if (error_code)
  {
    logger->log_warning("Request parsing error");
    // generate bad request response
    response_.version(11);
    response_.result(http::status::bad_request);
    response_.set(http::field::content_type, "text/plain");

    // set response body
    response_.body() = "400 Bad Request\r\n\r\n";

    logger->log_response_data(400,request_string_,"N/A");
  }
  else
  {
    // get request object
    request_ = parser.release();
    //parser.release() drops the new lines at the end of body
    if(request_.body()!="")
      request_.body()+= "\r\n\r\n";
    // generate response_
    logger->log_info("Generate response");
    request_dispatcher_.handle_request(request_, response_);
  }

  logger->log_info("Write response back to client");
  // convert response_ object to response_string_
  std::ostringstream oss;
  oss << response_;
  response_string_ = oss.str();
  oss.clear();
  // write response
  socket_->async_writing(
        response_string_,
        boost::bind(&session::handle_write, this,
            boost::placeholders::_1));
}

void session::respond_to_request_client_error()
{

    // generate bad request response
    response_.version(11);
    response_.result(http::status::bad_request);
    response_.set(http::field::content_type, "text/plain");

    // set response body
    response_.body() = "400 Bad Request\r\n\r\n";
    std::ostringstream oss;
    oss << response_;
    response_string_ = oss.str();
    oss.clear();

    Logger* logger = Logger::getLogger();
    logger->log_response_data(400,request_header_,"N/A");

    // write response
    socket_->async_writing(
          response_string_,
          boost::bind(&session::handle_write, this,
              boost::placeholders::_1));    
}

void session::respond_to_request_server_error()
{
    // generate bad request response
    response_.version(11);
    response_.result(http::status::internal_server_error);
    response_.set(http::field::content_type, "text/plain");

    // set response body
    response_.body() = "500 Internal Server Error\r\n\r\n";
    
    std::ostringstream oss;
    oss << response_;
    response_string_ = oss.str();
    oss.clear();

    Logger* logger = Logger::getLogger();
    logger->log_response_data(500,request_header_,"N/A");    

    // write response
    socket_->async_writing(
          response_string_,
          boost::bind(&session::handle_write, this,
              boost::placeholders::_1));    
}

void session::handle_write(const boost::system::error_code& error)
{
  if (!error)
  {
    socket_->shutdown();
  }
  delete socket_;
  delete this;
}

SessionSocket* session::get_socket()
{
  return socket_;
}