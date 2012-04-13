#include "shift.h"

void ShiftRegister::set_value(uint8_t value) {
  latch_.set_high(false);
  for (uint8_t i = 0; i < 8; i++) {
    data_.set_high(value & (1 << i));
    clock_.set_high(true);
    clock_.set_high(false);
  }
  latch_.set_high(true);
}
