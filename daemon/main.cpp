#include <iostream>
#include <fstream>
#include <chrono>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../common/easy_shutdown.hpp"


int main (int argc, char* argv[]) {

  const auto listen_address { "192.168.0.3" };
  uint16_t   listen_port    { 1337 };


  sockaddr_in listen_socket_config {
    .sin_family = AF_INET,
    .sin_port   = htons (1337),
    .sin_zero   = { }
  };

  if (-1 == inet_aton (
      listen_address,
      std::addressof (listen_socket_config.sin_addr)
  ))
    return easy_shutdown ("Invalid listen address");


  int listen_socket_descriptor { socket (AF_INET, SOCK_STREAM, IPPROTO_TCP) };

  if (-1 == listen_socket_descriptor)
    return easy_shutdown ("Socket creation failed");


  if (-1 == bind (
      listen_socket_descriptor,
      reinterpret_cast<sockaddr*> (std::addressof (listen_socket_config)),
      sizeof (sockaddr_in)
  ))
    return easy_shutdown ("Socket binding failed", listen_socket_descriptor);


  if (-1 == listen (listen_socket_descriptor, 0))
    return easy_shutdown ("Listeting failed", listen_socket_descriptor);


  const int file_reception_socket_descriptor
      = accept (listen_socket_descriptor, nullptr, nullptr);

  if (file_reception_socket_descriptor == -1)
    return easy_shutdown ("Accepting connection failed", listen_socket_descriptor, true);


  uint32_t file_size { };
  ssize_t received_bytes_count;

  do
    received_bytes_count = read (
      file_reception_socket_descriptor,
      std::addressof(file_size),
      sizeof (uint32_t)
    );
  while (received_bytes_count < sizeof (uint32_t));


  char* buffer { static_cast<char*>(malloc(file_size)) };

  
  do
    received_bytes_count = read (
      file_reception_socket_descriptor,
      buffer,
      file_size
    );
  while (received_bytes_count < file_size);


  std::ofstream os(std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
  os.write(buffer, file_size);
  os.close();
  

  easy_shutdown (listen_socket_descriptor);
  easy_shutdown (file_reception_socket_descriptor);

}