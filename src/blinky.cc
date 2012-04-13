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
  for (uint16_t i = 0x00; true; i++) {
    main_data->shift().set_value(i);
    Time::sleep(Time::millis(100));
  }
}
