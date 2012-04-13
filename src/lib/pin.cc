#include "sreg-impl.h"
#include "pin.h"
#include "platform.h"
#include "read-only-impl.h"
#include "time.h"

#include <avr/io.h>

Pin Pin::open(uint8_t index, DataDirection mode) {
  // Load the metadata from program storage.
  PinInfo pin_info = PinInfo::get()[index];
  PortInfo port_info = PortInfo::get()[pin_info.port];

  // If this isn't a valid index we return an empty pin value.
  if (pin_info.port == PortInfo::kNotAPort)
    return Pin();

  // Hoist the data we need from the metadata.
  uint8_t bit_mask = (1 << pin_info.bit_index);
  volatile uint8_t *io_register = (mode == OUT)
      ? port_info.output_register
      : port_info.input_register;

  // Write the appropriate mode into the mode register.
  volatile uint8_t *mode_register = port_info.mode_register;
  Interrupts::DisableDuring disable_interrupts;
  if (mode == IN) {
    *mode_register &= ~bit_mask;
  } else {
    *mode_register |= bit_mask;
  }

  // Opening a pin for i/o disables pulse width modulation.
  disable_pwm(pin_info.timer);

  return Pin(io_register, bit_mask);
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
void Pin::disable_pwm(uint8_t timer) {
  if (timer == Timers::kNotOnTimer)
    return;
  switch (timer) {
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

  Interrupts::DisableDuring disable_interrupts;
  if (is_on) {
    *io_register_ |= bit_mask_;
  } else {
    *io_register_ &= ~bit_mask_;
  }
}

bool Pin::is_high() {
  if (!is_pin())
    return false;

  return (*io_register_) & bit_mask_;
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
