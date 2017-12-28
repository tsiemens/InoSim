#include "InoLibUtils.h"

#include <chrono>
#include <stdlib.h>
#include <stdint.h>

unsigned long delayYieldIntervalMs = UINT64_MAX;
// Called every <delayYieldIntervalMs> milliseconds
void (*delayYieldCallback)() = NULL;

boost::circular_buffer<std::string> logBuff(LOG_BUFF_SIZE);
uint64_t lastLogN = 0;
std::ostringstream _logOs;

uint64_t nowMs() {
  using namespace std::chrono;
  return (uint64_t)duration_cast< milliseconds >(
      system_clock::now().time_since_epoch()
    ).count();
}
