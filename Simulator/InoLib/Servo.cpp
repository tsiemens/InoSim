#include "Servo.h"

#include <assert.h>
#include <sstream>

#include "Arduino.h"
#include "SysState.h"

// All servos
static servo_t servos[MAX_SERVOS];
static int servoPulseWidths[MAX_SERVOS];

int nextServo = 0;

Servo::Servo() {
  // Invalid
  servoIndex = INVALID_SERVO;
}

uint8_t
Servo::attach(int pin) {
  return attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t
Servo::attach(int pin, int min, int max) {
  assert(!attached());
  if (nextServo >= MAX_SERVOS) {
    // Can't get another servo
    return 0;
  }

  servoIndex = nextServo++;
  servos[servoIndex].Pin = {(uint8_t)pin, 1};

  this->min = min;
  this->max = max;
  return servoIndex + 1;
}

void
Servo::detach() {
  if (!attached()) {
    return;
  }
  servos[servoIndex].Pin = {0, 0};
  servoIndex = INVALID_SERVO;
}

void
Servo::write(int value) {
  setSysState(name(), value);
  if(value < MIN_PULSE_WIDTH) {
    // treat values less than 544 as angles in degrees
    // (valid values in microseconds are handled as microseconds)
    if (value < 0) value = 0;
    if (value > 180) value = 180;
    value = map(value, 0, 180, min, max);
  }
  this->writeMicroseconds(value);
}

void
Servo::writeMicroseconds(int value) {
  servoPulseWidths[servoIndex] = value;
}

int
Servo::read() {
  return  map(readMicroseconds() + 1, min, max, 0, 180);
}

int
Servo::readMicroseconds() {
  return servoPulseWidths[servoIndex];
}

bool
Servo::attached() {
  return servoIndex != INVALID_SERVO;
}

std::string Servo::name() {
  std::ostringstream os;
  os << "servo" << (int)servos[servoIndex].Pin.nbr;
  return os.str();
}
