#ifndef REQUEST_HANDLER_NEW_H
#define REQUEST_HANDLER_NEW_H

#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

typedef http::status status;

class RequestHandler {
 public:
  virtual status handle_request(const http::request<http::string_body>& req, http::response<http::string_body>& res) = 0;
};

#endif