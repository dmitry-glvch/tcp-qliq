#include <iostream>
#include <filesystem>

#include <boost/asio.hpp>

#include "net_routines.hpp"


boost::asio::awaitable<void>
client_routine (
    boost::asio::ip::tcp::socket&& socket,
    const std::filesystem::path& file_path);

boost::asio::awaitable<void> get_ack_or_throw (
    boost::asio::ip::tcp::socket& socket,
    imaqliq::test::net_routines::continuation expected_value
         = imaqliq::test::net_routines::continuation::OK);


int main (int argc, char* argv[])
try {

  if (argc != 4) {
    std::cerr << "Wrong usage.\n"
              << "Usage: " << argv[0]
              << " <server-ip> <port> <path-to-file>" << std::endl;
    return EXIT_FAILURE;
  }

  const char* server { argv[1] };
  const char* port   { argv[2] };

  const std::filesystem::path file_path { argv[3] };


  using tcp = boost::asio::ip::tcp;

  boost::asio::io_context io_context;
  auto endpoint = tcp::resolver { io_context }.resolve (server, port);

  tcp::socket socket { io_context };
  boost::asio::connect (socket, endpoint);
  
  boost::asio::co_spawn (
    io_context,
    client_routine (std::move (socket), file_path),
    boost::asio::detached
  );

  io_context.run ();

} catch (const boost::system::system_error& e) {

  const boost::system::error_code& code = e.code ();
  std::cerr << code.value () << ": " << code.message () << std::endl;
  return code.value (); 

} catch (const std::exception& e) {

  std::cerr << e.what () << std::endl;
  return EXIT_FAILURE;

}


boost::asio::awaitable<void> get_ack_or_throw (
    boost::asio::ip::tcp::socket& socket,
    imaqliq::test::net_routines::continuation expected_value) {

  imaqliq::test::net_routines::continuation ack
     { co_await imaqliq::test::net_routines::receive_continuation (socket) };
  if (ack != expected_value)
    throw std::runtime_error ("Unexpected answer from server");
}

boost::asio::awaitable<void>
client_routine (
    boost::asio::ip::tcp::socket&& s,
    const std::filesystem::path& file_path)
try {

  namespace r = imaqliq::test::net_routines;

  r::socket_t socket { std::forward<r::socket_t> (s) };
  co_await get_ack_or_throw (socket);

  const auto file_size { std::filesystem::file_size (file_path) };
  co_await r::send_int<r::length_t> (socket, file_size);
  co_await get_ack_or_throw (socket);

  const std::string file_name { file_path.filename ().string () };
  co_await r::send_string (socket, file_name);

  co_await r::send_file_contents (socket, file_size, file_path);
  co_await get_ack_or_throw (socket);
  
} catch (const boost::system::system_error& e) {

  const boost::system::error_code& code { e.code () };
  std::cerr << code.value () << ": " << code.message () << std::endl;

} catch (const std::exception& e) {

  std::cerr << e.what () << std::endl;

}
