#include <iostream>
#include <fstream>
#include <chrono>

#include <boost/asio.hpp>

#include "clem/clem.hpp"


std::string get_current_timestamp ();


int main (int argc, char* argv[]) {

  constexpr auto usage_example =
      "Usage: daemon <ip> <port>";

  if (argc != 3) {
    std::cerr << "Wrong usage.\n" << usage_example << std::endl;
    return -1;
  }

  try {

    namespace ip  = boost::asio::ip;
    using     tcp = ip::tcp;

    const char* listen_address { argv[1] };
    const auto  listen_port    { 1337 };

    boost::asio::io_context io_context;
    tcp::acceptor acceptor {
      io_context,
      tcp::endpoint {
        ip::make_address(listen_address),
        listen_port
      }
    };
    
    
    int accepted { 0 };
    while (accepted < 1) {

      tcp::socket socket { io_context };
      acceptor.accept(socket);
      ++accepted;

      const std::string file_name { clem::receive_string(socket) };
      
      std::ofstream os { get_current_timestamp() + "_" + file_name };
      os << clem::receive_string(socket);
      os.close();

    }

  } catch (const boost::system::system_error& e) {
    const boost::system::error_code code = e.code();
    std::cerr << "Invalid address.\n"
              << code.value() << ": " << code.message() << std::endl;
    return code.value(); 
  } catch (const std::exception& e) {
    std::cerr << "Unexpected error.\n" << e.what() << std::endl;
    return -1;
  } 

}


std::string get_current_timestamp () {
  using namespace std::chrono;
  time_point current_time { system_clock::now() };
  const auto ticks { current_time.time_since_epoch().count() };
  return std::to_string(ticks);
}