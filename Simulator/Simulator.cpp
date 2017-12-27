// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include "SimulatorConfig.h"
#include "InoProg.h"

void sigHandler(int sig) {
  void * bt[10];
  // Get void*'s for all the entries on the stack
  size_t size = backtrace(bt, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(bt, size, STDERR_FILENO);
  exit(1);
}

int main (int argc, char *argv[]) {
  signal(SIGABRT, sigHandler);

  if (argc >= 2 && !strcmp(argv[1], "-v")) {
     printf("%s Version %d.%d\n", argv[0],
             Simulator_VERSION_MAJOR,
             Simulator_VERSION_MINOR);
     return 0;
  }

  runIno();
  return 0;
}
