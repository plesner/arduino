#include "pin.h"
#include "time.h"
#include <Arduino.h>

extern "C" void delay(unsigned long millis) {
  Time::sleep_millis(millis);
}

extern "C" void pinMode(uint8_t pin, uint8_t mode) {
  Pin(pin).set_mode(mode);
}

extern "C" void digitalWrite(uint8_t pin, uint8_t value) {
  Pin(pin).set_high(value == HIGH);
}

extern "C" int analogRead(uint8_t pin) {
  return AnalogPin(pin).read();
}
