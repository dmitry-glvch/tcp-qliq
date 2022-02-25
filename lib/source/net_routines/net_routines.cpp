#include "net_routines.hpp"


using boost::asio::awaitable;
using boost::asio::use_awaitable;

using boost::asio::ip::tcp;
namespace ip = boost::asio::ip;


namespace imaqliq::test::net_routines {

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