#include <iostream>
#include <fstream>
#include <filesystem>

#include <boost/asio.hpp>

#include "net.hpp"

boost::asio::awaitable<void>
send_string (
  boost::asio::ip::tcp::socket& socket,
  const std::string& string,
  auto& endpoint);

boost::asio::awaitable<unsigned>
receive_go_on (boost::asio::ip::tcp::socket& socket);

boost::asio::awaitable<void>
send_length (boost::asio::ip::tcp::socket& socket, uint64_t length);

boost::asio::awaitable<void>
send_bytes (
    boost::asio::ip::tcp::socket& socket,
    imaqliq::test::net::bufferable auto& b,
    std::size_t byte_count);

boost::asio::awaitable<void>
receive_bytes (
    boost::asio::ip::tcp::socket& socket,
    imaqliq::test::net::bufferable auto& b,
    std::size_t byte_count);


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
    const std::string file_name { file_path.filename ().string () };


    using tcp = boost::asio::ip::tcp;
    boost::asio::io_context io_context { 1 };
    auto endpoint { tcp::resolver{ io_context }.resolve(server, port) };

    tcp::socket socket { io_context };
    
    boost::asio::co_spawn (
      io_context,
      send_string(socket, file_name, endpoint),
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
send_string (
    boost::asio::ip::tcp::socket& socket,
    const std::string& string,
    auto& endpoint) {

  try {
    co_await boost::asio::async_connect(socket, endpoint, boost::asio::use_awaitable);
        socket.set_option(boost::asio::ip::tcp::no_delay{ true });
    socket.set_option(boost::asio::socket_base::keep_alive{ true });
    co_await receive_go_on(socket);
    std::cout << "Go on received" << std::endl;
    co_await send_length (socket, string.length());
    std::cout << "Length sent" << std::endl;
    co_await receive_go_on(socket);
    std::cout << "Go on received" << std::endl;
    co_await send_bytes(socket, string, string.length());
  } catch (const std::exception& e) {
    std::cout << socket.is_open() << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

boost::asio::awaitable<unsigned>
receive_go_on (boost::asio::ip::tcp::socket& socket) {

  unsigned result;
  auto buffer { boost::asio::buffer(std::addressof(result), sizeof(unsigned)) };
  co_await receive_bytes(socket, buffer, sizeof(unsigned));
  co_return result;

}

boost::asio::awaitable<void>
send_length (boost::asio::ip::tcp::socket& socket, uint64_t length) {

  auto buffer { boost::asio::buffer(std::addressof(length), sizeof(uint64_t)) };
  co_await send_bytes(socket, buffer, sizeof(uint64_t));

}

boost::asio::awaitable<void>
send_bytes (
    boost::asio::ip::tcp::socket& socket,
    imaqliq::test::net::bufferable auto& b,
    std::size_t byte_count) {

  auto buffer { boost::asio::buffer(b, byte_count) };
  std::size_t bytes_received { 0 };
  std::cout << "Sending bytes" << std::endl;
  try { 
while (bytes_received < byte_count)
    bytes_received += co_await socket.async_send(buffer, boost::asio::use_awaitable);
  } catch (...) { std::cout << "SASI" << std::endl; } 
  
  std::cout << "Sending bytes" << std::endl;
}

boost::asio::awaitable<void>
receive_bytes (
    boost::asio::ip::tcp::socket& socket,
    imaqliq::test::net::bufferable auto& b,
    std::size_t byte_count) {

  auto buffer { boost::asio::buffer(b, byte_count) };
  std::size_t bytes_received { 0 };
  while (bytes_received < byte_count)
    bytes_received += co_await socket.async_receive(buffer, boost::asio::use_awaitable);

}