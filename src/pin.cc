#include <Arduino.h>

#include "avr.h"
#include "pin.h"
#include "time.h"
#include "read-only-impl.h"

void Pin::set_data_direction(DataDirection value) {
  if (!is_pin())
    return;

  // JWS: can I let the optimizer do this?
  volatile uint8_t *reg = port_info().mode_register;

  Interrupts::DisableDuring disable_interrupts;
  if (value == IN) {
    *reg &= ~pin_info().bit_mask;
  } else {
    *reg |= pin_info().bit_mask;
  }
}



#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

// Forcing this inline keeps the callers from having to push their own stuff
// on the stack. It is a good performance win and only takes 1 more byte per
// user than calling. (It will take more bytes on the 168.)
//
// But shouldn't this be moved into pinMode? Seems silly to check and do on
// each digitalread or write.
//
// Mark Sproul:
// - Removed inline. Save 170 bytes on atmega1280
// - changed to a switch statment; added 32 bytes but much easier to read and maintain.
// - Added more #ifdefs, now compiles for atmega645
//
//static inline void turnOffPWM(uint8_t timer) __attribute__ ((always_inline));
//static inline void turnOffPWM(uint8_t timer)
void Pin::disable_pwm() {
  if (!is_on_timer())
    return;
  switch (pin_info().timer) {
#if defined(TCCR1A) && defined(COM1A1)
  case TIMER1A:
    cbi(TCCR1A, COM1A1);
    break;
#endif

#if defined(TCCR1A) && defined(COM1B1)
  case TIMER1B:
    cbi(TCCR1A, COM1B1);
    break;
#endif

#if defined(TCCR2) && defined(COM21)
  case TIMER2:
    cbi(TCCR2, COM21);
    break;
#endif

#if defined(TCCR0A) && defined(COM0A1)
  case TIMER0A:
    cbi(TCCR0A, COM0A1);
    break;
#endif

#if defined(TIMER0B) && defined(COM0B1)
  case TIMER0B:
    cbi(TCCR0A, COM0B1);
    break;
#endif

#if defined(TCCR2A) && defined(COM2A1)
  case TIMER2A:
    cbi(TCCR2A, COM2A1);
    break;
#endif

#if defined(TCCR2A) && defined(COM2B1)
  case TIMER2B:
    cbi(TCCR2A, COM2B1);
    break;
#endif

#if defined(TCCR3A) && defined(COM3A1)
  case TIMER3A:
    cbi(TCCR3A, COM3A1);
    break;
#endif

#if defined(TCCR3A) && defined(COM3B1)
  case TIMER3B:
    cbi(TCCR3A, COM3B1);
    break;
#endif

#if defined(TCCR3A) && defined(COM3C1)
  case TIMER3C:
    cbi(TCCR3A, COM3C1);
    break;
#endif

#if defined(TCCR4A) && defined(COM4A1)
  case TIMER4A:
    cbi(TCCR4A, COM4A1);
    break;
#endif

#if defined(TCCR4A) && defined(COM4B1)
  case TIMER4B:
    cbi(TCCR4A, COM4B1);
    break;
#endif

#if defined(TCCR4A) && defined(COM4C1)
  case TIMER4C:
    cbi(TCCR4A, COM4C1);
    break;
#endif

#if defined(TCCR5A)
  case TIMER5A:
    cbi(TCCR5A, COM5A1);
    break;
  case TIMER5B:
    cbi(TCCR5A, COM5B1);
    break;
  case TIMER5C:
    cbi(TCCR5A, COM5C1);
    break;
#endif
  }
}

void Pin::set_high(bool is_on) {
  if (!is_pin())
    return;

  // If the pin that support PWM output, we need to turn it off
  // before doing a digital write.
  disable_pwm();

  volatile uint8_t *reg = port_info().output_register;

  Interrupts::DisableDuring disable_interrupts;
  if (is_on) {
    *reg |= pin_info().bit_mask;
  } else {
    *reg &= ~pin_info().bit_mask;
  }
}

bool Pin::is_high() {
  if (!is_pin())
    return false;

  // If the pin that support PWM output, we need to turn it off
  // before getting a digital reading.
  disable_pwm();
  volatile uint8_t *reg = port_info().input_register;

  return (*reg & pin_info().bit_mask);
}

void Pin::print(uint32_t value) {
  bool initial = is_high();
  uint32_t current = value;
  do {
    bool is_on = current & 1;
    current = current >> 1;
    set_high(true);
    Time::sleep(Duration::millis(is_on ? 250 : 100));
    set_high(false);
    Time::sleep(Duration::millis(350));
  } while (current != 0);
  set_high(initial);
}

#define cAtMega8(T, F) F

#define IF_ELSE(cCond, T, F) cCond(T, F)

static PROGMEM read_only_array_block<PinInfo, 20> pins = {{
  /* 00 */ {
      Timers::kNotOnTimer,
      1 << 0,
      PortInfo::kPd
  },
  /* 01 */ {
      Timers::kNotOnTimer,
      1 << 1,
      PortInfo::kPd
  },
  /* 02 */ {
      Timers::kNotOnTimer,
      1 << 2,
      PortInfo::kPd
  },
  /* 03 */ {
      IF_ELSE(cAtMega8, Timers::kTimer2B, Timers::kNotOnTimer),
      1 << 3,
      PortInfo::kPd
  },
  /* 04 */ {
      Timers::kNotOnTimer,
      1 << 4,
      PortInfo::kPd
  },
  /* 05 */ {
      IF_ELSE(cAtMega8, Timers::kTimer0B, Timers::kNotOnTimer),
      1 << 5,
      PortInfo::kPd
  },
  /* 06 */ {
      IF_ELSE(cAtMega8, Timers::kTimer0A, Timers::kNotOnTimer),
      1 << 6,
      PortInfo::kPd
  },
  /* 07 */ {
      Timers::kNotOnTimer,
      1 << 7,
      PortInfo::kPd
  },
  /* 08 */ { // ---
      Timers::kNotOnTimer,
      1 << 0,
      PortInfo::kPd
  },
  /* 09 */ {
      Timers::kTimer1A,
      1 << 1,
      PortInfo::kPb
  },
  /* 10 */ {
      Timers::kTimer1B,
      1 << 2,
      PortInfo::kPb
  },
  /* 11 */ {
      IF_ELSE(cAtMega8, Timers::kTimer2, Timers::kTimer2A),
      1 << 3,
      PortInfo::kPb
  },
  /* 12 */ {
      Timers::kNotOnTimer,
      1 << 4,
      PortInfo::kPb
  },
  /* 13 */ {
      Timers::kNotOnTimer,
      1 << 5,
      PortInfo::kPb
  },
  /* 14 */ { // ---
      Timers::kNotOnTimer,
      1 << 0,
      PortInfo::kPd
  },
  /* 15 */ {
      Timers::kNotOnTimer,
      1 << 1,
      PortInfo::kPd
  },
  /* 16 */ {
      Timers::kNotOnTimer,
      1 << 2,
      PortInfo::kPd
  },
  /* 17 */ {
      Timers::kNotOnTimer,
      1 << 3,
      PortInfo::kPd
  },
  /* 18 */ {
      Timers::kNotOnTimer,
      1 << 4,
      PortInfo::kPd
  },
  /* 19 */ {
      Timers::kNotOnTimer,
      1 << 5,
      PortInfo::kPd
  },
}};

read_only_array<PinInfo> PinInfo::get() {
  return pins;
}

static PROGMEM read_only_array_block<PortInfo, 5> ports = {{
  /* - */ { 0, 0, 0 },
  /* - */ { 0, 0, 0 },
  /* B */ { &DDRB, &PORTB, &PINB },
  /* C */ { &DDRB, &PORTC, &PINC },
  /* D */ { &DDRB, &PORTD, &PIND },
}};

read_only_array<PortInfo> PortInfo::get() {
  return ports;
}
