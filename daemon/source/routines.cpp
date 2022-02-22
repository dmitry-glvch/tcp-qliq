#include "routines.hpp"

#include <span>
#include <string>
#include <iostream>
#include <chrono>
#include <fstream>


using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coroutine = boost::asio::this_coro;

using boost::asio::ip::tcp;
namespace ip = boost::asio::ip;


namespace {
  std::string get_current_timestamp () {
    using namespace std::chrono;
    time_point current_time { system_clock::now() };
    const auto ticks { current_time.time_since_epoch().count() };
    return std::to_string(ticks);
  }
}


namespace imaqliq::test::daemon::routines {

awaitable<void>
listen (const ip::address& listen_address, ip::port_type listen_port) {

  std::cout << "[+] listen" << std::endl;

  auto executor { co_await this_coroutine::executor };
  tcp::acceptor acceptor { executor, { listen_address, listen_port } };

  while (true) {
    tcp::socket socket { co_await acceptor.async_accept (use_awaitable) };
    co_spawn (executor, service (socket), detached);
  }

}

awaitable<void> service (tcp::socket& socket) {

  std::cout << "[+] service" << std::endl;

  const std::string file_name { co_await receive_string(socket) };
  std::cout << file_name;

  std::cout << "[_] service" << std::endl;

}

awaitable<std::string> receive_string (tcp::socket& socket) {

  std::cout << "[+] receive_string" << std::endl;

  const uint64_t string_length { co_await receive_length(socket) };
  std::string result (string_length + 1, '\0');

  co_await receive_bytes(socket, result, string_length);
  std::cout << "[_] receive_string" << std::endl;
  co_return result;

}

awaitable<uint64_t> receive_length (tcp::socket& socket) {

  std::cout << "[+] receive_length" << std::endl;

  uint64_t length;
  auto buffer { boost::asio::buffer(&length, sizeof(uint64_t)) };
  co_await receive_bytes (socket, buffer, sizeof(uint64_t));

  std::cout << "[_] receive_length " << length << std::endl;
  co_return length;

}

awaitable<void> receive_bytes (
    tcp::socket& from,
    bufferable auto& to,
    std::size_t byte_count) {

  std::cout << "[+] receive_bytes " << byte_count << std::endl;

  std::size_t bytes_received { 0 };
  auto buffer { boost::asio::buffer(to, byte_count) };

  while (bytes_received < byte_count) {
    std::size_t received { co_await from.async_receive(buffer, use_awaitable) };
    bytes_received += received;
    if (received > 0) {
      std::cout << static_cast<const char*>(buffer.data()) << std::endl;
    }
  }

  std::cout << "[_] receive_bytes" << std::endl;

}

}
