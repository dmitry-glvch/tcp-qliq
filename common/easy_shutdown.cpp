#include "easy_shutdown.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <sys/socket.h>
#include <unistd.h>


int easy_shutdown (const char* message, int socket_descriptor, bool should_shutdow) {

  const bool has_message { message != nullptr && std::strlen(message) > 0 };

  if (has_message)
    std::perror (message);

  if (socket_descriptor != -1) {
    if (should_shutdow)
      shutdown (socket_descriptor, SHUT_RDWR);

    close (socket_descriptor);
  }

  return has_message ? errno : 0;

}

int easy_shutdown (int socket_descriptor) {
  return easy_shutdown (nullptr, socket_descriptor);
}