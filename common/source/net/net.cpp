#include "net.hpp"

#include <span>
#include <iostream>


namespace imaqliq::test::net {

void send_bytes_from (
    boost::asio::ip::tcp::socket& to,
    bufferable auto& from,
    std::size_t byte_count) {

  std::cout << "[+] send_bytes" << std::endl;

  auto buffer { boost::asio::buffer(from, byte_count) };
  std::size_t sent_bytes { 0 };

  while (sent_bytes < byte_count)
    sent_bytes += to.send(buffer);

  std::cout << "[_] send_bytes" << std::endl;
}


void send_length (
    boost::asio::ip::tcp::socket& socket,
    string_length_type length) {

  std::cout << "[+] send_length" << std::endl;

  auto buffer { boost::asio::buffer(&length, sizeof(string_length_type)) };
  send_bytes_from(socket, buffer, sizeof(string_length_type));

  std::cout << "[_] send_length" << std::endl;
}


void send_string (
    boost::asio::ip::tcp::socket& socket,
    const std::string_view& string) {

  std::cout << "[+] send_string" << std::endl;

  const auto length { string.length() };
  if (length > max_string_length)
    throw std::invalid_argument { "String is too long to be sent." };

  send_length(socket, length);
  send_bytes_from(socket, string, length);

  std::cout << "[_] send_string" << std::endl;

}

}
