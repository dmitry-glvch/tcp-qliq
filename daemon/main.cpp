#include <iostream>
#include <fstream>
#include <chrono>

#include <csignal>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "deamonize.hpp"
#include "clem/clem.hpp"


std::string get_current_timestamp ();
void service_routine (boost::asio::ip::tcp::socket& socket) noexcept;

void shutdown_worker (int signal);


int main (int argc, char* argv[]) {

  try {

    deamonize();
    umask(0);

    std::signal(SIGHUP, shutdown_worker);
    std::signal(SIGTERM, shutdown_worker);

    if (argc != 3) {
      std::cerr << "Wrong usage.\nUsage: daemon <ip> <port>" << std::endl;
      return -1;
    }

    namespace ip  = boost::asio::ip;
    using     tcp = boost::asio::ip::tcp;

    const char* listen_address { argv[1] };
    const auto  listen_port    { boost::lexical_cast<ip::port_type>(argv[2]) };

    boost::asio::io_context io_context;
    tcp::acceptor acceptor {
      io_context,
      tcp::endpoint {
        ip::make_address(listen_address),
        listen_port
      }
    };
    
    for (unsigned short accepted { 0 }; accepted < 1; ++accepted) {
      tcp::socket socket { io_context };
      acceptor.accept(socket);
      service_routine(socket);
    }

  } catch (const boost::system::system_error& e) {
    const boost::system::error_code& code = e.code();
    std::cerr << code.value() << ": " << code.message() << std::endl;
    return code.value(); 
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  } 

}

void service_routine (boost::asio::ip::tcp::socket& socket) noexcept {
  try {

    const std::string file_name { clem::receive_string(socket) };
      
    std::ofstream os { get_current_timestamp() + '_' + file_name };
    os << clem::receive_string(socket);
    os.close();

  } catch (...) {
    std::cerr << "Service routine error." << std::endl;
  }
}

std::string get_current_timestamp () {
  using namespace std::chrono;
  time_point current_time { system_clock::now() };
  const auto ticks { current_time.time_since_epoch().count() };
  return std::to_string(ticks);
}

void shutdown_worker (int signal) {
  std::cout << "Received signal " << signal << ".\n"
            << "Gracefully shutting down." << std::endl; 
}