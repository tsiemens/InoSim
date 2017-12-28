add_library(InoLib
  Arduino.cpp
  SysState.cpp
  Servo.cpp
  )

install (TARGETS InoLib DESTINATION bin)
install (FILES
  Arduino.h
  SysState.h
  Servo.h
  DESTINATION include)
