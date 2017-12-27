#include "InoBuiltins.h"

#include <time.h>

void delay(long ms) {
   timespec req = {ms / 1000, (ms % 1000) * 1000};
   timespec rem;
   nanosleep(&req, &rem);
}
