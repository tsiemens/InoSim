#ifndef ARDUINO_H
#define ARDUINO_H

// Based off
// https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/Arduino.h
// These are provided without explicit import in the .ino file.

void delay(unsigned long ms);

long map(long val, long fromLow, long fromHigh, long toLow, long toHigh);

// Define the arduino functions in which everything runs
// These must be defined by the .ino file
void setup();
void loop();

#endif // ARDUINO_H
