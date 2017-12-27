// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "SimulatorConfig.h"
#include "InoProg.h"

int main (int argc, char *argv[]) {
  if (argc >= 2 && !strcmp(argv[1], "-v")) {
     printf("%s Version %d.%d\n", argv[0],
             Simulator_VERSION_MAJOR,
             Simulator_VERSION_MINOR);
     return 0;
  }

  runIno();
  return 0;
}
