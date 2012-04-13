#include "sreg-impl.h"
#include "pin.h"
#include "platform.h"
#include "read-only-impl.h"
#include "time.h"

#include <avr/io.h>

void Pin::set_data_direction(DataDirection value) {
  if (!is_pin())
    return;

  // JWS: can I let the optimizer do this?
  volatile uint8_t *reg = port_info().mode_register;

  Interrupts::DisableDuring disable_interrupts;
  uint8_t bit_mask = 1 << pin_info().bit_index;
  if (value == IN) {
    *reg &= ~bit_mask;
  } else {
    *reg |= bit_mask;
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
  case Timers::kTimer1A:
    cbi(TCCR1A, COM1A1);
    break;
#endif

#if defined(TCCR1A) && defined(COM1B1)
  case Timers::kTimer1B:
    cbi(TCCR1A, COM1B1);
    break;
#endif

#if defined(TCCR2) && defined(COM21)
  case Timers::kTimer2:
    cbi(TCCR2, COM21);
    break;
#endif

#if defined(TCCR0A) && defined(COM0A1)
  case Timers::kTimer0A:
    cbi(TCCR0A, COM0A1);
    break;
#endif

#if defined(TIMER0B) && defined(COM0B1)
  case Timers::kTimer0B:
    cbi(TCCR0A, COM0B1);
    break;
#endif

#if defined(TCCR2A) && defined(COM2A1)
  case Timers::kTimer2A:
    cbi(TCCR2A, COM2A1);
    break;
#endif

#if defined(TCCR2A) && defined(COM2B1)
  case Timers::kTimer2B:
    cbi(TCCR2A, COM2B1);
    break;
#endif

#if defined(TCCR3A) && defined(COM3A1)
  case Timers::kTimer3A:
    cbi(TCCR3A, COM3A1);
    break;
#endif

#if defined(TCCR3A) && defined(COM3B1)
  case Timers::kTimer3B:
    cbi(TCCR3A, COM3B1);
    break;
#endif

#if defined(TCCR3A) && defined(COM3C1)
  case Timers::kTimer3C:
    cbi(TCCR3A, COM3C1);
    break;
#endif

#if defined(TCCR4A) && defined(COM4A1)
  case Timers::kTimer4A:
    cbi(TCCR4A, COM4A1);
    break;
#endif

#if defined(TCCR4A) && defined(COM4B1)
  case Timers::kTimer4B:
    cbi(TCCR4A, COM4B1);
    break;
#endif

#if defined(TCCR4A) && defined(COM4C1)
  case Timers::kTimer4C:
    cbi(TCCR4A, COM4C1);
    break;
#endif

#if defined(TCCR5A)
  case Timers::kTimer5A:
    cbi(TCCR5A, COM5A1);
    break;
  case Timers::kTimer5B:
    cbi(TCCR5A, COM5B1);
    break;
  case Timers::kTimer5C:
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
  uint8_t bit_mask = 1 << pin_info().bit_index;
  if (is_on) {
    *reg |= bit_mask;
  } else {
    *reg &= ~bit_mask;
  }
}

bool Pin::is_high() {
  if (!is_pin())
    return false;

  // If the pin that support PWM output, we need to turn it off
  // before getting a digital reading.
  disable_pwm();
  volatile uint8_t *reg = port_info().input_register;
  uint8_t bit_mask = 1 << pin_info().bit_index;

  return (*reg & bit_mask);
}

static read_only<PinInfo, 20> pins = {{
  /* 00 */ {
      Timers::kNotOnTimer,
      PIND0,
      PortInfo::kPd
  },
  /* 01 */ {
      Timers::kNotOnTimer,
      PIND1,
      PortInfo::kPd
  },
  /* 02 */ {
      Timers::kNotOnTimer,
      PIND2,
      PortInfo::kPd
  },
  /* 03 */ {
      IF_ELSE(cAtMega8, Timers::kTimer2B, Timers::kNotOnTimer),
      PIND3,
      PortInfo::kPd
  },
  /* 04 */ {
      Timers::kNotOnTimer,
      PIND4,
      PortInfo::kPd
  },
  /* 05 */ {
      IF_ELSE(cAtMega8, Timers::kTimer0B, Timers::kNotOnTimer),
      PIND5,
      PortInfo::kPd
  },
  /* 06 */ {
      IF_ELSE(cAtMega8, Timers::kTimer0A, Timers::kNotOnTimer),
      PIND6,
      PortInfo::kPd
  },
  /* 07 */ {
      Timers::kNotOnTimer,
      PIND7,
      PortInfo::kPd
  },
  /* 08 */ { // ---
      Timers::kNotOnTimer,
      PINB0,
      PortInfo::kPb
  },
  /* 09 */ {
      Timers::kTimer1A,
      PINB1,
      PortInfo::kPb
  },
  /* 10 */ {
      Timers::kTimer1B,
      PINB2,
      PortInfo::kPb
  },
  /* 11 */ {
      IF_ELSE(cAtMega8, Timers::kTimer2, Timers::kTimer2A),
      PINB3,
      PortInfo::kPb
  },
  /* 12 */ {
      Timers::kNotOnTimer,
      PINB4,
      PortInfo::kPb
  },
  /* 13 */ {
      Timers::kNotOnTimer,
      PINB5,
      PortInfo::kPb
  },
  /* 14 */ { // ---
      Timers::kNotOnTimer,
      PINC0,
      PortInfo::kPc
  },
  /* 15 */ {
      Timers::kNotOnTimer,
      PINC1,
      PortInfo::kPc
  },
  /* 16 */ {
      Timers::kNotOnTimer,
      PINC2,
      PortInfo::kPc
  },
  /* 17 */ {
      Timers::kNotOnTimer,
      PINC3,
      PortInfo::kPc
  },
  /* 18 */ {
      Timers::kNotOnTimer,
      PINC4,
      PortInfo::kPc
  },
  /* 19 */ {
      Timers::kNotOnTimer,
      PINC5,
      PortInfo::kPc
  },
}};

read_only_vector<PinInfo> PinInfo::get() {
  return pins;
}

static read_only<PortInfo, 5> ports = {{
  /* - */ { 0, 0, 0 },
  /* - */ { 0, 0, 0 },
  /* B */ { &DDRB, &PORTB, &PINB },
  /* C */ { &DDRC, &PORTC, &PINC },
  /* D */ { &DDRD, &PORTD, &PIND },
}};

read_only_vector<PortInfo> PortInfo::get() {
  return ports;
}
