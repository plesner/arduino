#include "lib/atl.h"
#include "lib/main.h"
#include "lib/pin.h"
#include "lib/read-only-impl.h"
#include "lib/time.h"

class MainData {
public:
  void initialize();

  Pin data_pin_;
  Pin latch_pin_;
  Pin clock_pin_;
};

static uninitialized<MainData> main_data;

void MainData::initialize() {
  data_pin_ = Pin::open(9, Pin::OUT);
  latch_pin_ = Pin::open(11, Pin::OUT);
  clock_pin_ = Pin::open(12, Pin::OUT);
}

void shiftOut(Pin &dataPin, Pin &clockPin, uint8_t val) {
  elements<uint8_t, 8> map = {{0, 1, 2, 3, 4, 5, 6, 7}};
  uint8_t i;
  for (i = 0; i < 8; i++)  {
    dataPin.set_high(val & (1 << map[i]));
    clockPin.set_high(true);
    clockPin.set_high(false);
  }
}

void Main::setup() {
  main_data->initialize();
}

void Main::loop() {
  for (uint8_t i = 0x00; true; i++) {
    main_data->latch_pin_.set_high(false);
    shiftOut(main_data->data_pin_, main_data->clock_pin_, i);
    main_data->latch_pin_.set_high(true);
    Time::sleep(Time::millis(100));
  }
}
