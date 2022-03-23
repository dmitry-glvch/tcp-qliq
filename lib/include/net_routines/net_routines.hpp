#pragma once

#include <filesystem>
#include <string_view>
#include <concepts>
#include <span>

#include <boost/asio.hpp>


namespace imaqliq::test::net_routines {

using boost::asio::awaitable;
using socket_t = boost::asio::ip::tcp::socket;

using length_t = std::uint64_t;
constexpr length_t max_string_length { 200 };

enum class continuation : std::uint_fast8_t { OK, REPEAT, DENIAL };


awaitable<void>
receive_file_contents (
    socket_t& socket,
    length_t file_size,
    const std::filesystem::path& save_path);

awaitable<void>
send_file_contents (
    socket_t& socket,
    length_t file_size,
    const std::filesystem::path& file_path);


awaitable<std::string> receive_string (socket_t& socket);
awaitable<void> send_string (socket_t& socket, std::string_view string);


awaitable<continuation> receive_continuation (socket_t& socket);

awaitable<void> send_continuation (
    socket_t& socket,
    continuation value = continuation::OK);


template <std::integral integral_t>
awaitable<integral_t> receive_int (socket_t& socket);

template <std::integral integral_t>
awaitable<void> send_int (socket_t& socket, integral_t value);


template <typename T, std::size_t extent>
awaitable<void> receive_bytes (socket_t& socket, std::span<T, extent> bytes);

template <typename T, std::size_t extent>
awaitable<void> send_bytes (socket_t& socket, std::span<T, extent> bytes);
 
}


template <std::integral integral_t>
boost::asio::awaitable<integral_t>
imaqliq::test::net_routines::receive_int (
    imaqliq::test::net_routines::socket_t& socket) {

  integral_t result;
  co_await receive_bytes (
    socket,
    std::span<integral_t, 1> { std::addressof (result), 1 });
  co_return result;
}

template <std::integral integral_t>
boost::asio::awaitable<void>
imaqliq::test::net_routines::send_int (
    imaqliq::test::net_routines::socket_t& socket,
    integral_t value) {

  co_await send_bytes (
      socket,
      std::span<integral_t, 1> { std::addressof (value), 1 });
}

template <typename T, std::size_t extent>
boost::asio::awaitable<void>
imaqliq::test::net_routines::receive_bytes (
    boost::asio::ip::tcp::socket& socket,
    std::span<T, extent> bytes) {

  const std::size_t byte_count { bytes.size_bytes () };
  boost::asio::mutable_buffer b { bytes.data (), byte_count };

  std::size_t received { 0 };
  while (received < byte_count)
    received += co_await socket.async_receive (b, boost::asio::use_awaitable);
}

template <typename T, std::size_t extent>
boost::asio::awaitable<void>
imaqliq::test::net_routines::send_bytes (
    boost::asio::ip::tcp::socket& socket,
    std::span<T, extent> bytes) {

  const std::size_t byte_count { bytes.size_bytes () };
  boost::asio::const_buffer b { bytes.data (), byte_count };

  std::size_t sent { 0 };
  while (sent < byte_count)
    sent += co_await socket.async_send (b, boost::asio::use_awaitable);
}
