#include "factory/request_handler_factory.h"

std::string RequestHandlerFactory::removeTrailingSlashes(std::string input){

    while(input.length() > 1 && input.back() == '/'){
        input.pop_back();
    }

    return input;
}