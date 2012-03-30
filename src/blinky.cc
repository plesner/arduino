#include <Arduino.h>

#include "pin.h"
#include "main.h"
#include "read-only-impl.h"

struct Point {
  int8_t x;
  int8_t y;
};

void Main::setup() {
  Pin::get(13).set_data_direction(Pin::OUT);
}

static PROGMEM read_only_array_block<Point, 2> points_block = {{
    {2, 3}
}};

void Main::loop() {
  read_only_array<Point> points = points_block;
  Pin pin = Pin::get(13);
  pin.print(points[0].x);
  delay(1000);
}
