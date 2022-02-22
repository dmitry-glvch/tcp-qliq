#include "net.hpp"

#include <span>


namespace imaqliq::test::net {

void receive_bytes_to (
    boost::asio::ip::tcp::socket& from,
    bufferable auto& to,
    std::size_t byte_count) {

  auto buffer { boost::asio::buffer(to) };
  std::size_t bytes_received { 0 };

  while (bytes_received < byte_count) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    bytes_received += from.receive(buffer);
  }
}

void send_bytes_from (
    boost::asio::ip::tcp::socket& to,
    bufferable auto& from,
    std::size_t byte_count) {

  auto buffer { boost::asio::buffer(from) };
  std::size_t sent_bytes { 0 };

  while (sent_bytes < byte_count)
    sent_bytes += to.send(buffer);
}


string_length_type receive_length (boost::asio::ip::tcp::socket& socket) {

  std::array<std::byte, sizeof(string_length_type)> length_bytes;
  receive_bytes_to(socket, length_bytes, sizeof(string_length_type));

  return *reinterpret_cast<const string_length_type*>
      (std::as_bytes(std::span{ length_bytes }).data());

}

void send_length (
    boost::asio::ip::tcp::socket& socket,
    string_length_type length) {

  auto buffer { boost::asio::buffer(&length, sizeof(string_length_type)) };
  send_bytes_from(socket, buffer, sizeof(string_length_type));

}


std::string receive_string (boost::asio::ip::tcp::socket& socket) {
  string_length_type string_length { receive_length(socket) };
  std::string result (string_length, '\0');
  receive_bytes_to(socket, result, string_length);
  return result;
}

void send_string (
    boost::asio::ip::tcp::socket& socket,
    const std::string_view& string) {

  const auto length { string.length() };
  if (length > max_string_length)
    throw std::invalid_argument { "String is too long to be sent." };

  send_length(socket, length);
  send_bytes_from(socket, string, length);

}

}
