#include "logger.h"
#include <string>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
Logger* Logger::logger = 0;

Logger::Logger() {
    init();
    logging::add_common_attributes();
}

Logger* Logger::getLogger() {
    if (!logger) {
        logger = new Logger();
    }
    return logger;
}

/*
 * Initialize the logger.
 * The logger will log to a file and to the console.
 * The format of the log is: [timestamp] - [thread id] - [severity level] - [message]
 */
void Logger::init() {
    std::string log_format = "[%TimeStamp%] - [%ThreadID%] - [%Severity%] - %Message%\n";
    logging::add_file_log
    (
        keywords::file_name = "../logs/record_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = log_format,
        keywords::auto_flush = true
    );
    logging::add_console_log(std::cout, keywords::format = log_format);
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );
}

void Logger::log_server_init() {
    BOOST_LOG_TRIVIAL(info) << "Server initialized.";
}

void Logger::log_server_start(int port) {
    BOOST_LOG_TRIVIAL(info) << "Server started on port " << std::to_string(port) << ".";
}

void Logger::log_server_stop() {
    BOOST_LOG_TRIVIAL(info) << "Server stopped (user interrupted).";
}

void Logger::log_debug(std::string message) {
    BOOST_LOG_TRIVIAL(debug) << message;
}

void Logger::log_info(std::string message) {
    BOOST_LOG_TRIVIAL(info) << message;
}

void Logger::log_error(std::string message) {
    BOOST_LOG_TRIVIAL(error) << message;
}

void Logger::log_warning(std::string message) {
    BOOST_LOG_TRIVIAL(warning) << message;
}

void Logger::log_fatal(std::string message) {
    BOOST_LOG_TRIVIAL(fatal) << message;
}

void Logger::log_http_request(const HttpRequestParser& request_parser, SessionSocket* socket) {
    std::string request_method = request_parser.get_method();
    std::string request_url = request_parser.get_url();
    std::string request_version = request_parser.get_http_version();
    std::string request_address = socket->get_remote_ip();
    BOOST_LOG_TRIVIAL(info) <<  request_method << " " << request_url << " " << request_version << " - " << request_address;
}

void Logger::log_response_data(int res_code, std::string req_url, std::string handler_name){
    BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics] "
                            << "response_code: "
                            << res_code
                            << " "
                            << "request_path: "
                            << req_url
                            << " "
                            << "matched_handler: "
                            << handler_name;
}