#ifndef _TIMER_IMPL
#define _TIMER_IMPL

#include "timer.h"

class FunctionRegister {
public:
  static inline void set(volatile uint8_t &reg, uint8_t bit) {
    reg |= (1 << bit);
  }
  static inline void clear(volatile uint8_t &reg, uint8_t bit) {
    reg &= ~(1 << bit);
  }
};

template <>
void Tccr0::set_prescale_factor<64>() {
#if defined(__AVR_ATmega128__)
  // CPU specific: different values for the ATmega128
  FunctionRegister::set(TCCR0, CS02);
#elif defined(TCCR0) && defined(CS01) && defined(CS00)
  // this combination is for the standard atmega8
  FunctionRegister::set(TCCR0, CS01);
  FunctionRegister::set(TCCR0, CS00);
#endif
}

void Tccr0A::set_fast_pwm() {
  #if defined(TCCR0A) && defined(WGM01)
    FunctionRegister::set(TCCR0A, WGM01);
    FunctionRegister::set(TCCR0A, WGM00);
  #endif
}

template <>
void Tccr0A::set_prescale_factor<64>() {
#if defined(TCCR0A) && defined(CS01) && defined(CS00)
  // this combination is for the __AVR_ATmega645__ series
  FunctionRegister::set(TCCR0A, CS01);
  FunctionRegister::set(TCCR0A, CS00);
#endif
}

template <>
void Tccr0B::set_prescale_factor<64>() {
#if defined(TCCR0B) && defined(CS01) && defined(CS00)
  // this combination is for the standard 168/328/1280/2560
  FunctionRegister::set(TCCR0B, CS01);
  FunctionRegister::set(TCCR0B, CS00);
#endif
}

#endif // _TIMER_IMPL
