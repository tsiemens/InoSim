#ifndef SYS_STATE_H
#define SYS_STATE_H

#include <string>
#include <map>

extern std::map<std::string, std::string> sysStateMap;

extern void (*onSysStateChanged)(const std::string & state);

void setSysState(const std::string & state, int val);

#endif // SYS_STATE_H
