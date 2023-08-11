#include <cstdlib>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include "server.h"

server::server(boost::asio::io_service& io_service, short port, RequestDispatcher& request_dispatcher)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    request_dispatcher_(request_dispatcher)
{
  // bind to session
  start_accept();

  // create threads
  const int thread_count = std::thread::hardware_concurrency();
  std::vector<boost::shared_ptr<boost::thread>> threads;
  for (std::size_t i = 0; i < thread_count; i++)
  {
      boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &io_service_)));
      threads.push_back(thread);
  }

  // wait for threads to exit
  for (auto& thread : threads)
    thread->join();
}

void server::start_accept()
{
  session* new_session = session_factory_.create(io_service_, request_dispatcher_);
  acceptor_.async_accept(new_session->get_socket()->get_socket(),
      boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
    const boost::system::error_code& error)
{
  if (!error)
  {
    new_session->start();
  }
  else
  {
    delete new_session;
  }

  start_accept();
}
