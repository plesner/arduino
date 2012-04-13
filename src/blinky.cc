#include "lib/atl.h"
#include "lib/main.h"
#include "lib/read-only-impl.h"
#include "lib/shift.h"
#include "lib/time.h"

class MainData {
public:
  void initialize();

  ShiftRegister &shift() { return shift_; }
private:
  ShiftRegister shift_;
};

static uninitialized<MainData> main_data;

void MainData::initialize() {
  shift_ = ShiftRegister(9, 11, 12);
}

void Main::setup() {
  main_data->initialize();
}

void Main::loop() {
  uint8_t total = 16;
  uint8_t fraction = 8;
  for (uint8_t i = 0x00; true; i++) {
    for (uint8_t i = 0; i != 0xFF; i++) {
      if (i % total < fraction) {
        main_data->shift().set_value(i);
      } else {
        main_data->shift().set_value(0);
      }
    }
  }
}
