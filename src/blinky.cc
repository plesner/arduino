#include <Arduino.h>

#include "pin.h"
#include "time.h"
#include "main.h"
#include "read-only-impl.h"

struct Point {
  int8_t x;
  int8_t y;
};

void Main::setup() {
  Pin::get(8).set_data_direction(Pin::OUT);
  Pin::get(9).set_data_direction(Pin::OUT);
  Pin::get(10).set_data_direction(Pin::OUT);
  Pin::get(11).set_data_direction(Pin::OUT);
  Pin::get(12).set_data_direction(Pin::OUT);
  Pin::get(13).set_data_direction(Pin::OUT);
}

class Digit {
public:
  Digit(uint8_t *pinv, uint8_t pinc)
    : pinv_(pinv)
    , pinc_(pinc) { }
  void show(uint8_t value);
private:
  uint8_t *pinv_;
  uint8_t pinc_;
};

void Digit::show(uint8_t value) {
  for (uint8_t i = 0; i < pinc_; i++) {
    Pin::get(pinv_[i]).set_high((value & (1 << i)) != 0);
  }
}

void Main::loop() {
  static const uint8_t kDigits = 6;
  uint8_t digits[kDigits] = {13, 11, 12, 10, 9, 8};
  Digit digit(digits, kDigits);
  for (int8_t value = -5; value < 5; value++) {
    digit.show(1 << (value < 0 ? -value : value));
    Time::sleep(Duration::millis(100));
  }
}
