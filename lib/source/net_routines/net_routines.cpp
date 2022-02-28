#include "net_routines.hpp"

#include <fstream>


using boost::asio::awaitable;
using boost::asio::use_awaitable;

using boost::asio::ip::tcp;
namespace ip = boost::asio::ip;

namespace { constexpr std::size_t buffer_size { 32 * 1024 }; }


namespace imaqliq::test::net_routines {

awaitable<void> receive_file_contents (
    tcp::socket& socket,
    uint64_t file_size,
    const std::filesystem::path& save_path) {

  std::ofstream os { save_path };
  std::shared_ptr<char> buffer { static_cast<char*>(malloc(buffer_size)), free };
  char* const buffer_ptr = buffer.get();
  std::size_t bytes_received { 0 };

  while (bytes_received < file_size) {
    const std::size_t expected_bytes
        { std::min(file_size - bytes_received, buffer_size) };
    co_await net_routines::send_int<uint8_t>(socket, true);

    co_await net_routines::receive_bytes(socket, buffer_ptr, expected_bytes);
    os.write(buffer_ptr, expected_bytes);

    bytes_received += expected_bytes;
  }

  os.close();
}

awaitable<void> send_file_contents (
    tcp::socket& socket,
    uint64_t file_size,
    const std::filesystem::path& file_path) {

  std::ifstream is { file_path, std::ios::in | std::ios::binary };
  std::shared_ptr<char> buffer { static_cast<char*>(malloc(buffer_size)), free };
  char* const buffer_ptr = buffer.get();
  std::size_t bytes_sent { 0 };

  while (bytes_sent < file_size) {
    const std::size_t expected_bytes
        { std::min(file_size - bytes_sent, buffer_size) };
    co_await get_ack_or_throw(socket);

    is.read(buffer_ptr, expected_bytes);
    co_await send_bytes(socket, buffer_ptr, expected_bytes);

    bytes_sent += expected_bytes;
  }

  is.close ();
}


awaitable<std::string> receive_string (tcp::socket& socket) {
  
  uint64_t length { co_await receive_int<uint64_t>(socket) };

  std::string result (length + 1, '\0');
  co_await receive_bytes(socket, result, length);

  co_return result;

}

awaitable<void> send_string (tcp::socket& socket, std::string_view string) {

  auto length { string.length() };
  if (length > std::numeric_limits<uint64_t>::max())
    throw std::invalid_argument("String is too long to be sent.");

  co_await send_int<uint64_t>(socket, string.length());
  co_await send_bytes(socket, string, length);

}

awaitable<void> get_ack_or_throw (tcp::socket& socket) {

  if (uint8_t ack { co_await receive_int<uint8_t>(socket) }; !ack)
    throw std::runtime_error("Server denial");

}

}