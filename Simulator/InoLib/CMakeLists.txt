add_library(InoLib Arduino.cpp Servo.cpp)

install (TARGETS InoLib DESTINATION bin)
install (FILES Arduino.h Servo.h DESTINATION include)
