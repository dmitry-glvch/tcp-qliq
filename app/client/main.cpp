#include <iostream>
#include <fstream>
#include <filesystem>

#include <boost/asio.hpp>

#include "net_routines.hpp"


boost::asio::awaitable<void>
client_routine (
    boost::asio::ip::tcp::socket&& socket,
    const std::filesystem::path& file_path);


int main (int argc, char* argv[]) {

  try {

    if (argc != 4) {
      std::cerr << "Wrong usage.\n"
                << "Usage: " << argv[0]
                << " <server-ip> <port> <path-to-file>" << std::endl;
      return EXIT_FAILURE;
    }

    const char* server { argv[1] };
    const char* port   { argv[2] };

    namespace fs = std::filesystem;
    const fs::path    file_path { fs::path (argv[3]) };


    using tcp = boost::asio::ip::tcp;
    boost::asio::io_context io_context { 1 };
    auto endpoint { tcp::resolver{ io_context }.resolve(server, port) };

    tcp::socket socket { io_context };
    boost::asio::connect(socket, endpoint);
    
    boost::asio::co_spawn (
      io_context,
      client_routine(std::move(socket), file_path),
      boost::asio::detached
    );

    io_context.run ();


  } catch (const boost::system::system_error& e) {

    const boost::system::error_code& code = e.code();
    std::cerr << code.value() << ": " << code.message() << std::endl;
    return code.value(); 

  } catch (const std::exception& e) {

    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
    
  }

}


boost::asio::awaitable<void>
client_routine (
    boost::asio::ip::tcp::socket&& s,
    const std::filesystem::path& file_path) {

  using tcp = boost::asio::ip::tcp;
  namespace routines = imaqliq::test::net_routines;

  try {

    tcp::socket socket { std::forward<tcp::socket>(s) };
    co_await routines::get_ack_or_throw(socket);

    const auto file_size { std::filesystem::file_size(file_path) };
    co_await routines::send_int<uint64_t>(socket, file_size);

    co_await routines::get_ack_or_throw(socket);

    const std::string file_name { file_path.filename ().string () };
    co_await routines::send_string(socket, file_name);

    co_await routines::get_ack_or_throw(socket);

    std::ifstream is { file_path, std::ios::in | std::ios::binary };
    const std::string file_contents { std::istreambuf_iterator { is }, { } };
    is.close ();

    co_await routines::send_string(socket, file_contents);
    co_await routines::get_ack_or_throw(socket);
    
  } catch (const boost::system::system_error& e) {
    const boost::system::error_code& code = e.code();
    std::cerr << code.value() << ": " << code.message() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

}