#include <Arduino.h>

#include "avr.h"

void Pin::set_data_direction(DataDirection value) {
  uint8_t bit = digitalPinToBitMask(index_);
  uint8_t port = digitalPinToPort(index_);
  volatile uint8_t *reg;

  if (port == NOT_A_PIN) return;

  // JWS: can I let the optimizer do this?
  reg = portModeRegister(port);

  if (value == IN) {
    uint8_t oldSREG = SREG;
    cli();
    *reg &= ~bit;
    SREG = oldSREG;
  } else {
    uint8_t oldSREG = SREG;
    cli();
    *reg |= bit;
    SREG = oldSREG;
  }
}

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
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
static void turnOffPWM(uint8_t timer)
{
        switch (timer)
        {
                #if defined(TCCR1A) && defined(COM1A1)
                case TIMER1A:   cbi(TCCR1A, COM1A1);    break;
                #endif
                #if defined(TCCR1A) && defined(COM1B1)
                case TIMER1B:   cbi(TCCR1A, COM1B1);    break;
                #endif

                #if defined(TCCR2) && defined(COM21)
                case  TIMER2:   cbi(TCCR2, COM21);      break;
                #endif

                #if defined(TCCR0A) && defined(COM0A1)
                case  TIMER0A:  cbi(TCCR0A, COM0A1);    break;
                #endif

                #if defined(TIMER0B) && defined(COM0B1)
                case  TIMER0B:  cbi(TCCR0A, COM0B1);    break;
                #endif
                #if defined(TCCR2A) && defined(COM2A1)
                case  TIMER2A:  cbi(TCCR2A, COM2A1);    break;
                #endif
                #if defined(TCCR2A) && defined(COM2B1)
                case  TIMER2B:  cbi(TCCR2A, COM2B1);    break;
                #endif

                #if defined(TCCR3A) && defined(COM3A1)
                case  TIMER3A:  cbi(TCCR3A, COM3A1);    break;
                #endif
                #if defined(TCCR3A) && defined(COM3B1)
                case  TIMER3B:  cbi(TCCR3A, COM3B1);    break;
                #endif
                #if defined(TCCR3A) && defined(COM3C1)
                case  TIMER3C:  cbi(TCCR3A, COM3C1);    break;
                #endif

                #if defined(TCCR4A) && defined(COM4A1)
                case  TIMER4A:  cbi(TCCR4A, COM4A1);    break;
                #endif
                #if defined(TCCR4A) && defined(COM4B1)
                case  TIMER4B:  cbi(TCCR4A, COM4B1);    break;
                #endif
                #if defined(TCCR4A) && defined(COM4C1)
                case  TIMER4C:  cbi(TCCR4A, COM4C1);    break;
                #endif
                #if defined(TCCR5A)
                case  TIMER5A:  cbi(TCCR5A, COM5A1);    break;
                case  TIMER5B:  cbi(TCCR5A, COM5B1);    break;
                case  TIMER5C:  cbi(TCCR5A, COM5C1);    break;
                #endif
        }
}

void Pin::set_high(bool is_on) {
  uint8_t timer = digitalPinToTimer(index_);
  uint8_t bit = digitalPinToBitMask(index_);
  uint8_t port = digitalPinToPort(index_);
  volatile uint8_t *out;

  if (port == NOT_A_PIN) return;

  // If the pin that support PWM output, we need to turn it off
  // before doing a digital write.
  if (timer != NOT_ON_TIMER) turnOffPWM(timer);

  out = portOutputRegister(port);

  uint8_t oldSREG = SREG;
  cli();

  if (is_on) {
    *out |= bit;
  } else {
    *out &= ~bit;
  }

  SREG = oldSREG;
}

bool Pin::is_high() {
  uint8_t timer = digitalPinToTimer(index_);
  uint8_t bit = digitalPinToBitMask(index_);
  uint8_t port = digitalPinToPort(index_);

  if (port == NOT_A_PIN) return false;

  // If the pin that support PWM output, we need to turn it off
  // before getting a digital reading.
  if (timer != NOT_ON_TIMER) turnOffPWM(timer);

  return (*portInputRegister(port) & bit);
}

void Pin::print(uint32_t value) {
  bool initial = is_high();
  uint32_t current = value;
  do {
    bool is_on = current & 1;
    current = current >> 1;
    set_high(true);
    delay(is_on ? 250 : 100);
    set_high(false);
    delay(350);
  } while (current != 0);
  set_high(initial);
}
