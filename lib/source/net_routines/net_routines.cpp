#include "net_routines.hpp"


using boost::asio::awaitable;
using boost::asio::use_awaitable;

using boost::asio::ip::tcp;
namespace ip = boost::asio::ip;


namespace imaqliq::test::net_routines {

awaitable<std::string> receive_string (tcp::socket& socket) {
  
  uint64_t length { co_await receive_int<uint64_t>(socket) };

  std::string result (length + 1, '\0');
  co_await send_int<uint8_t>(socket, true);

  co_await receive_bytes(socket, result, length);
  co_return result;

}

awaitable<void> send_string (tcp::socket& socket, std::string_view string) {

  auto length { string.length() };
  if (length > std::numeric_limits<uint64_t>::max())
    throw std::invalid_argument("String is too long to be sent.");

  co_await send_int<uint64_t>(socket, string.length());
  co_await get_ack_or_throw(socket);
  co_await send_bytes(socket, string, length);

}

awaitable<void> get_ack_or_throw (tcp::socket& socket) {

  if (uint8_t ack { co_await receive_int<uint8_t>(socket) }; !ack)
    throw std::runtime_error("Server denial");

}


template<std::integral integral>
awaitable<integral> receive_int (tcp::socket& socket) {

  constexpr auto int_size { sizeof(integral) };

  integral result;
  auto buffer { boost::asio::buffer(std::addressof(result), int_size) };
  co_await receive_bytes(socket, buffer, int_size);

  co_return result;

}

template<std::integral integral>
awaitable<void> send_int (tcp::socket& socket, integral value) {

  constexpr auto int_size { sizeof(integral) };
  auto buffer { boost::asio::buffer(std::addressof(value), int_size) };

  co_await send_bytes(socket, buffer, int_size);

}

awaitable<void> receive_bytes (
    tcp::socket& from,
    bufferable auto& to,
    std::size_t byte_count) {

  std::size_t bytes_received { 0 };
  auto buffer { boost::asio::buffer(to, byte_count) };

  while (bytes_received < byte_count)
    bytes_received += co_await from.async_receive(buffer, use_awaitable);

}

awaitable<void> send_bytes (
    tcp::socket& to,
    bufferable auto& from,
    std::size_t byte_count) {

  auto buffer { boost::asio::buffer(from, byte_count) };
  std::size_t bytes_received { 0 };

  while (bytes_received < byte_count)
    bytes_received += co_await to.async_send(buffer, use_awaitable);

}

}