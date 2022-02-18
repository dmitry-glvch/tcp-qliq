#include "../common/easy_shutdown.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <string_view>
#include <unistd.h>


int main (int argc, char* argv[]) {

  if (argc >= 2) {

    namespace fs = std::filesystem;

    const fs::path         file_path { fs::path (argv[3]) };
    const std::string_view file_name { file_path.filename ().string () };
    const uintmax_t        file_size { fs::file_size (file_path) };

    std::ifstream     is (file_path, std::ios::in | std::ios::binary);
    const std::string file_contents { std::istreambuf_iterator { is }, {} };
    is.close ();

    const int   port { 1337 };
    const char* server { "192.168.0.3" };

    sockaddr_in socket_config { .sin_family = AF_INET,
                                .sin_port   = htons (port),
                                .sin_zero   = {} };

    if (-1 == inet_aton (server, std::addressof (socket_config.sin_addr)))
      return easy_shutdown ("Invalid server address");

    const int socket_desscriptor { socket (PF_INET, SOCK_STREAM, IPPROTO_TCP) };
    if (socket_desscriptor == -1) { }


    if (-1
        == connect (
            socket_desscriptor,
            reinterpret_cast<const sockaddr*> (&socket_config),
            sizeof (sockaddr_in)))
      return easy_shutdown ("Connection failed", socket_desscriptor);

    uint32_t l = file_contents.length ();
    std::cout << "Sending " << l << std::endl;
    ssize_t sent_bytes_count;

    do {
      sent_bytes_count
          = send (socket_desscriptor, std::addressof (l), sizeof (uint32_t), 0);
    } while (sent_bytes_count < sizeof (uint32_t));

    // do {
    //   sent_bytes_count = send (
    //       socket_desscriptor, file_name.data (), file_name.length (), 0);
    // } while (sent_bytes_count < file_name.length ());

    do {
      sent_bytes_count = send (
          socket_desscriptor, file_contents.c_str (), file_contents.length (),
          0);
    } while (sent_bytes_count < file_contents.length ());

    std::ofstream os (std::to_string (
        std::chrono::system_clock::now ().time_since_epoch ().count ()));
    os.write (file_contents.c_str (), file_contents.length ());
    os.close ();

    easy_shutdown (socket_desscriptor);
  }

}