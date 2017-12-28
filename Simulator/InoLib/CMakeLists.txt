add_library(InoLib
  Arduino.cpp
  InoLibUtils.cpp
  SysState.cpp
  Servo.cpp
  )

install (TARGETS InoLib DESTINATION bin)
install (FILES
  Arduino.h
  InoLibUtils.h
  SysState.h
  Servo.h
  DESTINATION include)
