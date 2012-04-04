#include <Arduino.h>

#include "atl.h"
#include "pin.h"
#include "time.h"
#include "main.h"
#include "read-only-impl.h"

class Main::Data {
public:
  void initialize();
  vector<Pin> pins() { return pins_; }
private:
  elements<Pin, 6> pins_;
};

static uninitialized<Main::Data> main_data;

void Main::Data::initialize() {
  elements<uint8_t, 6> map = {{13, 11, 12, 10, 9, 8}};
  for (uint8_t i = 0; i < map.length(); i++) {
    pins_[i] = Pin::get(map[i]);
    pins_[i].set_data_direction(Pin::OUT);
  }
}

Main::Data &Main::setup() {
  Main::Data &result = *main_data;
  result.initialize();
  return result;
}

class Digit {
public:
  Digit(vector<Pin> pins)
    : pins_(pins) { }
  void show(uint8_t value);
private:
  vector<Pin> &pins() { return pins_; }
  vector<Pin> pins_;
};

void Digit::show(uint8_t value) {
  for (uint8_t i = 0; i < pins().length(); i++) {
    pins()[i].set_high((value & (1 << i)) != 0);
  }
}

void Main::loop(Main::Data &data) {
  Digit digit(data.pins());
  for (int8_t value = -5; value < 5; value++) {
    digit.show(1 << (value < 0 ? -value : value));
    Time::sleep(Duration::millis(100));
  }
}
