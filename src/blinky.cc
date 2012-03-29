#include <Arduino.h>

#include "avr.h"
#include "main.h"
#include "read-only-impl.h"

PROGMEM static ReadOnlyArray<int32_t, 4> ints = {{10, 11, 12, 13}};

void Main::setup() {
  Pin::get(13).set_data_direction(Pin::OUT);
}

void Main::loop() {
  Pin pin = Pin::get(13);
  pin.print(ints[2]);
  delay(1000);
}
