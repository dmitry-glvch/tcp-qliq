#include <string_view>
#include <concepts>
#include <iostream>

#include <boost/asio.hpp>

namespace imaqliq::test::net_routines {

boost::asio::awaitable<void>
receive_file (
    boost::asio::ip::tcp::socket& socket,
    uint64_t length,
    std::ostream& os);

boost::asio::awaitable<void>
send_file (
    boost::asio::ip::tcp::socket& socket,
    uint64_t length,
    std::istream& is);


boost::asio::awaitable<std::string>
receive_string (boost::asio::ip::tcp::socket& socket);

boost::asio::awaitable<void>
send_string (boost::asio::ip::tcp::socket& socket, std::string_view string);


boost::asio::awaitable<void>
get_ack_or_throw (boost::asio::ip::tcp::socket& socket);


template<std::integral integral>
boost::asio::awaitable<integral>
receive_int (boost::asio::ip::tcp::socket& socket);

template<std::integral integral>
boost::asio::awaitable<void>
send_int (boost::asio::ip::tcp::socket& socket, integral value);


template <typename T>
concept bufferable = requires (T obj) {
  boost::asio::buffer(obj, std::declval<std::size_t>());
};

boost::asio::awaitable<void>
receive_bytes (
    boost::asio::ip::tcp::socket& from,
    bufferable auto& to,
    std::size_t byte_count);

boost::asio::awaitable<void>
send_bytes (
    boost::asio::ip::tcp::socket& to,
    bufferable auto& from,
    std::size_t byte_count);

    

}


template<std::integral integral>
boost::asio::awaitable<integral>
imaqliq::test::net_routines::receive_int (boost::asio::ip::tcp::socket& socket) {

  constexpr auto int_size { sizeof(integral) };

  integral result;
  auto buffer = boost::asio::buffer(std::addressof(result), int_size);
  co_await receive_bytes(socket, buffer, int_size);

  co_return result;

}

template<std::integral integral>
boost::asio::awaitable<void>
imaqliq::test::net_routines::send_int (
    boost::asio::ip::tcp::socket& socket,
    integral value) {

  constexpr auto int_size { sizeof(integral) };
  auto buffer = boost::asio::buffer(std::addressof(value), int_size);

  co_await send_bytes(socket, buffer, int_size);

}

boost::asio::awaitable<void>
imaqliq::test::net_routines::receive_bytes (
    boost::asio::ip::tcp::socket& from,
    imaqliq::test::net_routines::bufferable auto& to,
    std::size_t byte_count) {

  std::size_t bytes_received { 0 };
  auto buffer = boost::asio::buffer(to, byte_count);

  while (bytes_received < byte_count)
    bytes_received += co_await from.async_receive(buffer, boost::asio::use_awaitable);

}

boost::asio::awaitable<void>
imaqliq::test::net_routines::send_bytes (
    boost::asio::ip::tcp::socket& to,
    imaqliq::test::net_routines::bufferable auto& from,
    std::size_t byte_count) {

  auto buffer = boost::asio::buffer(from, byte_count);
  std::size_t bytes_received { 0 };

  while (bytes_received < byte_count)
    bytes_received += co_await to.async_send(buffer, boost::asio::use_awaitable);

}