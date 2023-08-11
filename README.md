# Source Code Layout

- conf: stores the configuration files for both development and testing
- docker: stores docker files for docker image generation
- include: contains header files
     - handler: contains the files for EchoHandler, StaticHandler, and any additional handlers that will be added.
     - handler_factory: contains the factory classes used to generate each handler
- src: contains all source code files
- log: contains the logging file
- data: contains the static files used to test the server's static handling functionality
- tests: contains all the unit and integration tests
     - configs: contains example configuration files for testing
     - http_requests: example http requests used to test server
     - static_test_files: example static files for unit tests
     - integration_test: contains the integration test script and correct outputs for each integration test case
  CMakeLists.txt: contains the list of dependencies necessary to generate build files

# How to build, test, and run the code.

- How to build the server
1. Inside the top level of the yet-another-web-server directory, Create build directory using mkdir build.
2. cd into the build directory, run cmake .. to generate the build files.
3. Run make to build the program


- How to run the server
1. Go to the build directory.
2. Run ./bin/server ../conf/test_config


- How to test the server
After running make in the build directory, then run make test to run the test cases.


# How to add a request handler.

- Example of existing request handler

echo_handler.h

#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H

#include "request_handler.h"

class EchoHandler : public RequestHandler
{
public:
    EchoHandler();
    status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) override;
};

#endif // ECHO_HANDLER_H 


echo_handler.cc

#include "handler/echo_handler.h"

EchoHandler::EchoHandler() {}

status EchoHandler::handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res)
{
    // read request into std::string object
    std::ostringstream oss;
	oss << req;
	std::string req_str = oss.str();
	oss.clear();

    // set response header
    status res_status = http::status::ok;
    int res_version = 11;
    std::string res_content_type = "text/plain";
    
    res.version(res_version);
    res.result(res_status);
    res.set(http::field::content_type, res_content_type);

    // set response body
    res.body() = req_str;

    // prepare response
    res.prepare_payload();

    return res_status;
}


The EchoHandler implements the RequestHandler interface and thus implements the handle_request function.
The handle_request function takes in two parameters, a http_request object and a http_response object passed by reference.
It uses the data inside the http_request to modify the http_response object that was passed by reference.
The handle_request function returns a status indicating whether the request handling was successful.

- How to create a new request handler

There are several componenents to adding a new request handler:

1. Define the class, which implements RequestHandler
2. Define the factory for that class
3. Linking your files in Makefile
4. Add an entry to RequestHandlerFactoryFactory::createHandlerFactory

After these, you have successfully created a new type of request handler. You can add entries to the config files to actually associate a path to your request handler. 

The server reads the config file and generates the factories for each path, and those factories will generate the corresponding short-lived handlers when recieving requests. When creating handlers, the factories takes a NginxConfig object, which is guaranteed to be the arguments (child blocks in the config file) corresponding to the matching path of the request url. The factory is responsible to parse the arguments that you need from the NginxConfig object. 

Those steps are explained in detail below. 

To add another request handler, create a header file include/handler directory. The header file must include request_handler_interface.h.
In addition, the class that is created in the header file must implement the RequestHandler interface.

Now proceed to create the source file corresponding to the header file. The source file is in the src/handler folder.
Make sure to include the header file. Then implement the constructor and the handle_request function.

Each request handler must come with a corresponding request handler factory class that constructs the handler. After writing your new handler source code,
write the factory class that goes along with it. For an example of what the factory class should look like, refer to the section below on
"format of the requesthandlerfactory"

Then you need to add an entry to our static function in RequestHandlerFactoryFactory, which generates a factory from a string, if the string matches a keyword (of your choice). This keyword is the one you use in your config file to specify this type of request handlers. 

Don't forget to update the CMakeLists.txt file with the appropriate dependencies before you build the server to test your changes.

The config file you use for testing is test_config. The config file that goes into the docker image is deploy_config. Please feel free to reference the Assignment 6 spec for the specifics of the config file format.


# Format of the requesthandlerfactory

An example request handler factory for echo_handler (echo_handler_factory.cc).

// begin source code

#include "handler_factory/echo_handler_factory.h"
#include "handler/echo_handler.h"

RequestHandler * EchoHandlerFactory::create(std::string location, std::string url, const NginxConfig & config)
{
    return new EchoHandler();
}

// end source code


The EchoHandlerFactory implements the RequestHandlerFactory interface. In fact, any handler factory must implement RequestHandlerFactory.
Thus, any handler factory must provide its own implementation of the create() function, which gives instructions on how to construct the
appropriate request handler.


# Format of the config file

- Example config file (test_config):


port 8080;

location /static/ StaticHandler {
    root /usr/bin;
}



location /echo EchoHandler {
}

location / EchoHandler {
}

// end config file

To add a new handler to the config file, create a new location block in test_config. The format of a config file location block
is

location [request URI path] [name of handler] {
    [optional handler arguments]
}

