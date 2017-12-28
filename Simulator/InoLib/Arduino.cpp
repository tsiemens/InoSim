#include "Arduino.h"

#include <assert.h>
#include <stdint.h>
#include <time.h>

unsigned long delayYieldIntervalMs = UINT64_MAX;
// Called every <delayYieldIntervalMs> milliseconds
void (*delayYieldCallback)() = NULL;

#define M_TO_N 1000000

unsigned long delayInternal(unsigned long ms) {
  timespec req = {(time_t)(ms / 1000), (time_t)((ms % 1000) * M_TO_N)};
  timespec rem;
  int ret = nanosleep(&req, &rem);
  if (delayYieldCallback) {
    delayYieldCallback();
  }
  unsigned long remaining = 0;
  if (ret == -1) {
    remaining = (rem.tv_sec * 1000) + (rem.tv_nsec / M_TO_N);
  }
  assert(remaining <= ms);
  return remaining;
}

void delay(unsigned long ms) {
  unsigned long remaining = ms;
  while (remaining) {
    if (remaining <= delayYieldIntervalMs) {
      remaining = delayInternal(remaining);
    } else {
      remaining -= delayYieldIntervalMs;
      remaining += delayInternal(delayYieldIntervalMs);
    }
  }
}

long map(long val, long fromLow, long fromHigh, long toLow, long toHigh) {
  return (val - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
