#include <string_view>

#include <boost/asio.hpp>


namespace imaqliq::test::net {

using string_length_type = uint64_t;

constexpr string_length_type max_string_length
    { std::numeric_limits<string_length_type>::max() };

void send_length (
    boost::asio::ip::tcp::socket& socket,
    string_length_type length);


void send_string (
    boost::asio::ip::tcp::socket& socket,
    const std::string_view& string);


template <typename T>
concept bufferable = requires (T obj) { boost::asio::buffer(obj, std::declval<uint64_t>()); };

void send_bytes_from (
    boost::asio::ip::tcp::socket& to,
    bufferable auto& from,
    std::size_t byte_count);

}
