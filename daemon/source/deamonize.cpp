#include "deamonize.hpp"

#include <iostream>

#include <cstdio>

#include <unistd.h>


void deamonize () {

  const auto& try_to_fork { [] () {
    if (const pid_t fork_status{ fork() }; fork_status < 0) {
      std::perror("Failed to fork");
      exit(errno);
    } else if (fork_status > 0)
      exit(EXIT_SUCCESS);
  }};

  try_to_fork();

  if (const pid_t setsid_status { setsid() }; setsid_status < 0) {
    std::cerr << "Setsid failure." << std::endl;
    exit(EXIT_FAILURE);
  }

  try_to_fork();  

}