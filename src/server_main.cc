//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <csignal>
#include "server.h"
#include "config_parser.h"
#include "logger.h"

#include "request_dispatcher.h"

using boost::asio::ip::tcp;

void interrupt_handler(int signum) 
{
  Logger* logger = Logger::getLogger();
  logger->log_server_stop();
  exit(signum);
}

int main(int argc, char* argv[])
{
  try
  {
    Logger* logger = Logger::getLogger();
    signal(SIGINT, interrupt_handler);
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      logger->log_error("Wrong usage of the server.");
      return 1;
    } 

    // check for valid port from the config file
    
    NginxConfigParser parser;
    NginxConfig config;
    logger->log_info("Parsing the config file.");
    if(!parser.Parse(argv[1],&config))
    { // check for syntax issues with the config file
      logger->log_error("Failed to parse the config file.");
      return 1;
    } 

    logger->log_info("Grabbing the port number from the config file.");
    int portno = config.get_port_num(); // no need to pass the config pointer anymore
    if(portno == -1)
    {
      logger->log_error("Config file has invalid port number.");
	    return portno;
    }
    logger->log_info("Parsing completed.");

    std::map<std::string, NginxConfig*> arguments = config.getArguments();
    std::map<std::string, RequestHandlerFactory*> factories = config.getFactories();

    // log config parsing info
    if (arguments.empty())
    {
      logger->log_error("No path in config file.");
	    return 1;
    }
    
    for (const auto &[location, factory] : factories)
    {
      logger->log_info("Found route: " + location);
    }

    RequestDispatcher dispatcher(factories, arguments);
    // create server
    boost::asio::io_service io_service;
    server s(io_service, portno, dispatcher);
     

    logger->log_server_init();
    logger->log_server_start(portno);
    // io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    Logger::getLogger()->log_error("An exception has occurred\n");
    // add logging to indicate error
  }

  return 0;
}
