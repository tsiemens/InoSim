#include "SysState.h"
#include <sstream>

std::map<std::string, std::string> sysStateMap;

void (*onSysStateChanged)(const std::string & state) = NULL;

void
setSysState(const std::string & state, int val) {
  std::ostringstream os;
  os << val;
  sysStateMap[state] = os.str();
  if (onSysStateChanged) {
    onSysStateChanged(state);
  }
}
