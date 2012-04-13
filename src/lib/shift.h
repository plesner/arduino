#ifndef _SHIFT
#define _SHIFT

#include "pin.h"

class ShiftRegister {
public:
  ShiftRegister(Pin data, Pin latch, Pin clock)
    : data_(data)
    , latch_(latch)
    , clock_(clock) { }
  ShiftRegister(uint8_t data, uint8_t latch, uint8_t clock)
    : data_(Pin::open(data, Pin::OUT))
    , latch_(Pin::open(latch, Pin::OUT))
    , clock_(Pin::open(clock, Pin::OUT)) { }
  ShiftRegister() { }

  // Sets the 8 bits of this shift register.
  void set_value(uint8_t value);

private:
  Pin data_;
  Pin latch_;
  Pin clock_;
};

#endif // _SHIFT
