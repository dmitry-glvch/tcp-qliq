#include "deamonize.hpp"

#include <iostream>

#include <cstdio>

#include <unistd.h>


namespace {

void fork_or_exit () {
  if (const pid_t fork_status{ fork() }; fork_status < 0) {
    std::perror("Failed to fork");
    exit(errno);
  } else if (fork_status > 0)
    exit(EXIT_SUCCESS);
}

}


void imaqliq::test::daemon::deamonize () {

  fork_or_exit();

  if (const pid_t setsid_status { setsid() }; setsid_status < 0) {
    std::cerr << "Setsid failure.\n"
                 "Setsid returned " << setsid_status << std::endl;
    exit(EXIT_FAILURE);
  }

  fork_or_exit();  

}
