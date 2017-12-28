#include "InoLibUtils.h"

#include <stdlib.h>
#include <stdint.h>

unsigned long delayYieldIntervalMs = UINT64_MAX;
// Called every <delayYieldIntervalMs> milliseconds
void (*delayYieldCallback)() = NULL;

boost::circular_buffer<std::string> logBuff(LOG_BUFF_SIZE);

std::ostringstream _logOs;
