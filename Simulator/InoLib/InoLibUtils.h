#ifndef INO_LIB_UTILS_H
#define INO_LIB_UTILS_H

#include <sstream>

#include <boost/circular_buffer.hpp>

#define LOG_BUFF_SIZE 40

extern unsigned long delayYieldIntervalMs;
// Called every <delayYieldIntervalMs> milliseconds
extern void (*delayYieldCallback)();

extern boost::circular_buffer<std::string> logBuff;

extern std::ostringstream _logOs;

#define NLOG(__streamExp) \
  _logOs.clear(); \
  _logOs.str(""); \
  _logOs << __streamExp; \
  logBuff.push_back(_logOs.str())

#endif // INO_LIB_UTILS_H
