#include <Arduino.h>
#include "avr.h"

void setup() {
  Pin::get(13).set_data_direction(Pin::OUT);
}

static PROGMEM ReadOnlyArray<int32_t, 4> ints = {{10, 11, 12, 13}};

void loop() {
  Pin pin = Pin::get(13);
  PinUtils::print(pin, ints[0]);
  delay(1000);
}
