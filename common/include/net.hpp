#include <string_view>

#include <boost/asio.hpp>


namespace imaqliq::test::net {

using string_length_type = uint64_t;

constexpr string_length_type max_string_length
    { std::numeric_limits<string_length_type>::max() };


template <typename T>
concept bufferable = requires (T obj) { boost::asio::buffer(obj); };


void receive_bytes_to (
    boost::asio::ip::tcp::socket& from,
    bufferable auto& to,
    std::size_t byte_count);

void send_bytes_from (
    boost::asio::ip::tcp::socket& to,
    bufferable auto& from,
    std::size_t byte_count);


string_length_type receive_length (boost::asio::ip::tcp::socket& socket);
void send_length (
    boost::asio::ip::tcp::socket& socket,
    string_length_type length);


std::string receive_string (boost::asio::ip::tcp::socket& socket);
void send_string (
    boost::asio::ip::tcp::socket& socket,
    const std::string_view&       string);

}
