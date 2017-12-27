#include "Arduino.h"

#include <time.h>

void delay(unsigned long ms) {
   timespec req = {(time_t)(ms / 1000), (time_t)((ms % 1000) * 1000)};
   timespec rem;
   nanosleep(&req, &rem);
}

long map(long val, long fromLow, long fromHigh, long toLow, long toHigh) {
  return (val - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
