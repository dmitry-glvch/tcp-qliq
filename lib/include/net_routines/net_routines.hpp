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