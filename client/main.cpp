#include <iostream>
#include <fstream>
#include <filesystem>

#include <boost/asio.hpp>

#include "clem/clem.hpp"


int main (int argc, char* argv[]) {

  constexpr auto usage_example =
      "Usage: client <server-ip> <port> <path-to-file>";

  if (argc != 4) {
    std::cerr << "Wrong usage.\n" << usage_example << std::endl;
    return -1;
  }

  const char* server { argv[1] };
  const char* port   { argv[2] };


  namespace fs = std::filesystem;
  const fs::path    file_path { fs::path (argv[3]) };
  const std::string file_name { file_path.filename ().string () };


  using tcp = boost::asio::ip::tcp;
  boost::asio::io_context io_context;
  auto endpoint { tcp::resolver{io_context}.resolve(server, port) };


  tcp::socket socket { io_context };
  boost::asio::connect(socket, endpoint);


  clem::send_string(socket, file_name);


  std::ifstream     is (file_path, std::ios::in | std::ios::binary);
  const std::string file_contents { std::istreambuf_iterator { is }, { } };
  is.close ();

  clem::send_string(socket, file_contents);

}