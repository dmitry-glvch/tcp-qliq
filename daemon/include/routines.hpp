#pragma once

#include <boost/asio.hpp>


namespace imaqliq::test::daemon::routines {

boost::asio::awaitable<void>
listen(
  const boost::asio::ip::address& listen_address,
  boost::asio::ip::port_type listen_port
);

boost::asio::awaitable<void>
service (boost::asio::ip::tcp::socket&& socket);

boost::asio::awaitable<std::string>
receive_string (boost::asio::ip::tcp::socket& socket);

boost::asio::awaitable<void>
send_go_on (boost::asio::ip::tcp::socket& socket, uint8_t go_on = 1);

boost::asio::awaitable<uint64_t>
receive_length (boost::asio::ip::tcp::socket& socket);

template <typename T>
concept bufferable = requires (T obj) {
  boost::asio::buffer(obj, std::declval<std::size_t>());
};

boost::asio::awaitable<void>
receive_bytes (
    boost::asio::ip::tcp::socket& from,
    bufferable auto& to,
    std::size_t byte_count);

}