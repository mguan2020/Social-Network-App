cmake_minimum_required(VERSION 3.10.0)
project(yet-another-web-server)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable test coverage
include(cmake/CodeCoverageReportConfig.cmake)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS filesystem system log_setup log )
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# source lib
add_library(session_lib src/session.cc)
add_library(server_lib src/server.cc)
add_library(config_lib src/config_parser.cc)
add_library(http_request_parser_lib src/http_request_parser.cc)
add_library(http_response_lib src/http_response.cc)
add_library(get_response_lib src/get_response.cc)
add_library(tcp_socket_lib src/tcp_socket.cc)
add_library(session_factory_lib src/factory/session_factory.cc)
add_library(logger_lib src/logger.cc)
add_library(file_system_lib src/file_system.cc)
add_library(boost_file_system_lib src/boost_file_system.cc)
add_library(mime_lib src/mime.cc)
add_library(meme_module_lib src/meme_module.cc)
add_library(meme_lib src/meme_classes/meme.cc)
add_library(comment_lib src/meme_classes/comment.cc)
add_library(request_dispatcher_lib src/request_dispatcher.cc)
add_library(request_handler_factory_lib src/factory/request_handler_factory.cc)
add_library(request_handler_factory_factory_lib src/factory/request_handler_factory_factory.cc)

# handler and handler factories
add_library(echo_handler_lib src/handler/echo_handler.cc)
add_library(echo_handler_factory_lib src/factory/echo_handler_factory.cc)
add_library(static_handler_lib src/handler/static_handler.cc)
add_library(static_handler_factory_lib src/factory/static_handler_factory.cc)
add_library(file_not_found_handler_lib src/handler/file_not_found_handler.cc)
add_library(file_not_found_handler_factory_lib src/factory/file_not_found_handler_factory.cc)
add_library(health_handler_lib src/handler/health_handler.cc)
add_library(health_handler_factory_lib src/factory/health_handler_factory.cc)
add_library(crud_handler_lib src/handler/crud_handler.cc)
add_library(crud_handler_factory_lib src/factory/crud_handler_factory.cc)
add_library(block_handler_lib src/handler/block_handler.cc)
add_library(block_handler_factory_lib src/factory/block_handler_factory.cc)
add_library(meme_handler_lib src/handler/meme_handler.cc)
add_library(meme_handler_factory_lib src/factory/meme_handler_factory.cc)


# link libraries
target_link_libraries(config_lib logger_lib request_handler_factory_factory_lib)
target_link_libraries(session_lib http_request_parser_lib logger_lib request_dispatcher_lib)
target_link_libraries(session_factory_lib session_lib tcp_socket_lib)
target_link_libraries(server_lib session_lib session_factory_lib config_lib tcp_socket_lib request_dispatcher_lib)
target_link_libraries(logger_lib Boost::log_setup Boost::log http_request_parser_lib)
target_link_libraries(request_dispatcher_lib)
target_link_libraries(request_handler_factory_factory_lib echo_handler_factory_lib static_handler_factory_lib health_handler_factory_lib file_not_found_handler_factory_lib crud_handler_factory_lib block_handler_factory_lib meme_handler_factory_lib)
target_link_libraries(get_response_lib http_response_lib)
target_link_libraries(meme_module_lib logger_lib meme_lib comment_lib)
target_link_libraries(boost_file_system_lib file_system_lib Boost::filesystem)
# handler
target_link_libraries(static_handler_lib mime_lib logger_lib)
target_link_libraries(crud_handler_lib logger_lib)
target_link_libraries(echo_handler_lib logger_lib)
target_link_libraries(file_not_found_handler_lib logger_lib)
target_link_libraries(health_handler_lib logger_lib)
target_link_libraries(block_handler_lib logger_lib)
target_link_libraries(meme_handler_lib logger_lib meme_lib comment_lib)


# handler factory
target_link_libraries(file_not_found_handler_factory_lib file_not_found_handler_lib)
target_link_libraries(health_handler_factory_lib health_handler_lib request_handler_factory_lib)
target_link_libraries(echo_handler_factory_lib echo_handler_lib)
target_link_libraries(static_handler_factory_lib static_handler_lib request_handler_factory_lib)
target_link_libraries(crud_handler_factory_lib crud_handler_lib logger_lib request_handler_factory_lib)
target_link_libraries(block_handler_factory_lib block_handler_lib)
target_link_libraries(meme_handler_factory_lib meme_handler_lib meme_module_lib request_handler_factory_lib boost_file_system_lib)


# source executables
add_executable(server src/server_main.cc)
target_link_libraries(server logger_lib server_lib config_lib pthread Boost::system request_dispatcher_lib)

# test executables
# add_executable(server_test tests/server_test.cc)
add_executable(config_parser_test tests/config_parser_test.cc)
add_executable(get_response_test tests/get_response_test.cc)
add_executable(http_request_parser_test tests/http_request_parser_test.cc)
add_executable(session_test tests/session_test.cc)
add_executable(tcp_socket_test tests/tcp_socket_test.cc)
add_executable(request_dispatcher_test tests/request_dispatcher_test.cc)
add_executable(boost_file_system_test tests/boost_file_system_test.cc)
add_executable(meme_module_test tests/meme_module_test.cc)
add_executable(meme_test tests/meme_test.cc)
add_executable(meme_handler_test tests/meme_handler_test.cc)
# handler and handler factory test
add_executable(echo_handler_test tests/echo_handler_test.cc)
add_executable(echo_handler_factory_test tests/echo_handler_factory_test.cc)
add_executable(static_handler_test tests/static_handler_test.cc)
add_executable(static_handler_factory_test tests/static_handler_factory_test.cc)
add_executable(file_not_found_handler_test tests/file_not_found_handler_test.cc)
add_executable(file_not_found_handler_factory_test tests/file_not_found_handler_factory_test.cc)
add_executable(health_handler_test tests/health_handler_test.cc)
add_executable(health_handler_factory_test tests/health_handler_factory_test.cc)
add_executable(crud_handler_test tests/crud_handler_test.cc)
add_executable(crud_handler_factory_test tests/crud_handler_factory_test.cc)

# target_link_libraries(server_test server_lib session_factory_lib  gtest_main)
target_link_libraries(get_response_test get_response_lib gtest_main)
target_link_libraries(config_parser_test config_lib gtest_main)
target_link_libraries(http_request_parser_test http_request_parser_lib gtest_main)
target_link_libraries(session_test session_lib config_lib gtest_main)
target_link_libraries(tcp_socket_test tcp_socket_lib gtest_main)
target_link_libraries(request_dispatcher_test request_dispatcher_lib config_lib gtest_main)
target_link_libraries(boost_file_system_test boost_file_system_lib Boost::filesystem gtest_main)
target_link_libraries(meme_module_test meme_module_lib meme_lib comment_lib boost_file_system_lib gtest_main)
target_link_libraries(meme_test meme_lib meme_module_lib gtest_main)
target_link_libraries(meme_handler_test meme_handler_lib meme_module_lib boost_file_system_lib gtest_main)
target_link_libraries(echo_handler_test echo_handler_lib gtest_main)
target_link_libraries(echo_handler_factory_test echo_handler_factory_lib gtest_main)
target_link_libraries(static_handler_test static_handler_lib gtest_main)
target_link_libraries(static_handler_factory_test static_handler_factory_lib config_lib gtest_main)
target_link_libraries(file_not_found_handler_test file_not_found_handler_lib gtest_main)
target_link_libraries(file_not_found_handler_factory_test file_not_found_handler_factory_lib gtest_main)
target_link_libraries(health_handler_test health_handler_lib gtest_main)
target_link_libraries(health_handler_factory_test health_handler_factory_lib gtest_main)
target_link_libraries(crud_handler_test crud_handler_lib gtest_main)
target_link_libraries(crud_handler_factory_test crud_handler_factory_lib gtest_main)

# test binary
# gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(get_response_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests) 
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests) 
gtest_discover_tests(http_request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests) 
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests) 
gtest_discover_tests(tcp_socket_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_dispatcher_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(boost_file_system_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(meme_module_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(meme_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(meme_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(static_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(static_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(file_not_found_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(file_not_found_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(health_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(health_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(crud_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(crud_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# integration test
add_test(NAME integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test/integration_test.sh)

# test coverage
generate_coverage_report(TARGETS server session_lib session_factory_lib server_lib config_lib http_request_parser_lib
                         http_response_lib tcp_socket_lib logger_lib mime_lib request_dispatcher_lib get_response_lib 
                         static_handler_lib static_handler_factory_lib 
                         request_handler_factory_factory_lib file_not_found_handler_lib
                         echo_handler_lib echo_handler_factory_lib
                         file_not_found_handler_factory_lib meme_module_lib meme_lib meme_handler_lib
                         health_handler_lib health_handler_factory_lib boost_file_system_lib
                         TESTS config_parser_test http_request_parser_test get_response_test
                         tcp_socket_test request_dispatcher_test session_test
                         echo_handler_test echo_handler_factory_test
                         static_handler_test static_handler_factory_test
                         file_not_found_handler_test file_not_found_handler_factory_test 
                         health_handler_test health_handler_factory_test
                         crud_handler_test crud_handler_factory_test boost_file_system_test
                         meme_module_test meme_test meme_handler_test)
