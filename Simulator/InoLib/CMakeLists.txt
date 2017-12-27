add_library(InoLib Servo.cpp)

install (TARGETS InoLib DESTINATION bin)
install (FILES Servo.h DESTINATION include)
