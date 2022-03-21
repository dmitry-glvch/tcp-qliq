#include <filesystem>
#include <string_view>
#include <concepts>

#include <boost/asio.hpp>

namespace imaqliq::test::net_routines {

template <typename T>
using awaitable_t = boost::asio::awaitable<T>;
using length_t    = uint64_t;
using socket_t    = boost::asio::ip::tcp::socket;

enum class continuation : uint8_t { OK, REPREAT, DENIAL };


awaitable_t<void>
receive_file_contents (
    socket_t & socket,
    length_t file_size,
    const std::filesystem::path& save_path);

awaitable_t<void>
send_file_contents (
    socket_t & socket,
    length_t file_size,
    const std::filesystem::path& file_path);


awaitable_t<std::string> receive_string (socket_t& socket);
awaitable_t<void> send_string (socket_t& socket, std::string_view string);


awaitable_t<void> get_ack_or_throw (
    socket_t& socket,
    continuation expected_value = continuation::OK);

awaitable_t<continuation> receive_continuation (socket_t& socket);
  
awaitable_t<void> send_continuation (
    socket_t& socket,
    continuation value = continuation::OK);


template<std::integral integral>
awaitable_t<integral> receive_int (socket_t& socket);

template<std::integral integral>
awaitable_t<void> send_int (socket_t& socket, integral value);


template <typename T>
concept bufferable = requires (T obj, std::size_t s) {
  boost::asio::buffer(obj, s);
};

awaitable_t<void> receive_bytes (
    socket_t& from,
    bufferable auto& to,
    std::size_t byte_count);

awaitable_t<void> send_bytes (
    socket_t
    & to,
    bufferable auto& from,
    std::size_t byte_count);
    
}


template<std::integral integral>
boost::asio::awaitable<integral>
imaqliq::test::net_routines::receive_int (
    boost::asio::ip::tcp::socket& socket) {

  integral result;

  integral* buffer_ptr = std::addressof(result);
  co_await receive_bytes(socket, buffer_ptr, sizeof(integral));

  co_return result;
}

template<std::integral integral>
boost::asio::awaitable<void>
imaqliq::test::net_routines::send_int (
    boost::asio::ip::tcp::socket& socket,
    integral value) {

  const integral* buffer_ptr = std::addressof(value);
  co_await send_bytes(socket, buffer_ptr, sizeof(integral));
}

boost::asio::awaitable<void>
imaqliq::test::net_routines::receive_bytes (
    boost::asio::ip::tcp::socket& from,
    imaqliq::test::net_routines::bufferable auto& to,
    std::size_t byte_count) {

  std::size_t received { 0 };
  auto buffer = boost::asio::buffer(to, byte_count);

  while (received < byte_count)
    received += co_await from.async_receive(buffer, boost::asio::use_awaitable);
}

boost::asio::awaitable<void>
imaqliq::test::net_routines::send_bytes (
    boost::asio::ip::tcp::socket& to,
    imaqliq::test::net_routines::bufferable auto& from,
    std::size_t byte_count) {

  auto buffer = boost::asio::buffer(from, byte_count);
  std::size_t sent { 0 };

  while (sent < byte_count)
    sent += co_await to.async_send(buffer, boost::asio::use_awaitable);
}