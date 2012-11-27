#include "pin.h"
#include "sreg-impl.h"
#include "wiring_private.h"
#include "pins_arduino.h"

void Pin::set_mode(uint8_t mode) {
  uint8_t bit = digitalPinToBitMask(pin_);
  uint8_t port = digitalPinToPort(pin_);

  if (port == NOT_A_PIN)
    return;

  // JWS: can I let the optimizer do this?
  volatile uint8_t *reg = portModeRegister(port);
  volatile uint8_t *out = portOutputRegister(port);

  if (mode == INPUT) {
    Interrupts::DisableDuring disable_interrupts;
    *reg &= ~bit;
    *out &= ~bit;
  } else if (mode == INPUT_PULLUP) {
    Interrupts::DisableDuring disable_interrupts;
    *reg &= ~bit;
    *out |= bit;
  } else {
    Interrupts::DisableDuring disable_interrupts;
    *reg |= bit;
  }
}

void Pin::disable_pwm(uint8_t timer) {
  switch (timer) {
  case TIMER1A:
    if (eval_cc(ccHasTccr1A) && eval_cc(ccHasCom1A1))
      cbi(TCCR1A, COM1A1);
    break;
  case TIMER1B:
    if (eval_cc(ccHasTccr1A) && eval_cc(ccHasCom1B1))
      cbi(TCCR1A, COM1B1);
    break;
  case  TIMER2:
#if defined(TCCR2) && defined(COM21)
    cbi(TCCR2, COM21);
#endif
    break;
  case  TIMER0A:
#if defined(TCCR0A) && defined(COM0A1)
    cbi(TCCR0A, COM0A1);
#endif
    break;
  case  TIMER0B:
#if defined(TIMER0B) && defined(COM0B1)
    cbi(TCCR0A, COM0B1);
#endif
    break;
  case  TIMER2A:
#if defined(TCCR2A) && defined(COM2A1)
    cbi(TCCR2A, COM2A1);
#endif
    break;
  case  TIMER2B:
#if defined(TCCR2A) && defined(COM2B1)
    cbi(TCCR2A, COM2B1);
#endif
    break;
  case  TIMER3A:
#if defined(TCCR3A) && defined(COM3A1)
    cbi(TCCR3A, COM3A1);
#endif
    break;
  case  TIMER3B:
#if defined(TCCR3A) && defined(COM3B1)
    cbi(TCCR3A, COM3B1);
#endif
    break;
  case  TIMER3C:
#if defined(TCCR3A) && defined(COM3C1)
    cbi(TCCR3A, COM3C1);
#endif
    break;
  case  TIMER4A:
#if defined(TCCR4A) && defined(COM4A1)
    cbi(TCCR4A, COM4A1);
#endif
    break;
  case  TIMER4B:
#if defined(TCCR4A) && defined(COM4B1)
    cbi(TCCR4A, COM4B1);
#endif
    break;
  case  TIMER4C:
#if defined(TCCR4A) && defined(COM4C1)
    cbi(TCCR4A, COM4C1);
#endif
    break;
  case TIMER4D:
#if defined(TCCR4C) && defined(COM4D1)
    cbi(TCCR4C, COM4D1);
#endif
    break;

  case  TIMER5A:
#if defined(TCCR5A)
    cbi(TCCR5A, COM5A1);
#endif
    break;

  case  TIMER5B:
#if defined(TCCR5A)
    cbi(TCCR5A, COM5B1);
#endif
    break;
  case  TIMER5C:
#if defined(TCCR5A)
    cbi(TCCR5A, COM5C1);
#endif
    break;
  }
}

bool DigitalPin::read() {
  uint8_t timer = digitalPinToTimer(pin_);
  uint8_t bit = digitalPinToBitMask(pin_);
  uint8_t port = digitalPinToPort(pin_);

  if (port == NOT_A_PIN)
    return LOW;

  // If the pin that support PWM output, we need to turn it off
  // before getting a digital reading.
  if (timer != NOT_ON_TIMER)
    disable_pwm(timer);

  if (*portInputRegister(port) & bit)
    return HIGH;
  return LOW;
}

void Pin::set_high(bool high) {
  uint8_t timer = digitalPinToTimer(pin_);
  uint8_t bit = digitalPinToBitMask(pin_);
  uint8_t port = digitalPinToPort(pin_);

  if (port == NOT_A_PIN)
    return;

  // If the pin that support PWM output, we need to turn it off
  // before doing a digital write.
  if (timer != NOT_ON_TIMER)
    disable_pwm(timer);

  volatile uint8_t *out = portOutputRegister(port);

  Interrupts::DisableDuring disable_interrupts;

  if (!high) {
    *out &= ~bit;
  } else {
    *out |= bit;
  }

}

uint8_t analog_reference = DEFAULT;

int AnalogPin::read() {
  uint8_t pin = pin_;

  if (eval_cc(ccATmega1280) || eval_cc(ccATmega1280)) {
    // allow for channel or pin numbers
    if (pin >= 54)
      pin -= 54;
  } else if (eval_cc(ccATmega32U4)) {
    // allow for channel or pin numbers
    if (pin >= 18)
      pin -= 18;
  } else if (eval_cc(ccATmega1284)) {
    // allow for channel or pin numbers
    if (pin >= 24)
      pin -= 24;
  } else {
    // allow for channel or pin numbers
    if (pin >= 14)
      pin -= 14;
  }

#if defined(__AVR_ATmega32U4__)
  pin = analogPinToChannel(pin);
  ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
  // the MUX5 bit of ADCSRB selects whether we're reading from channels
  // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
  ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

  // set the analog reference (high two bits of ADMUX) and select the
  // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
  // to 0 (the default).
#if defined(ADMUX)
  ADMUX = (analog_reference << 6) | (pin & 0x07);
#endif

#if defined(ADCSRA) && defined(ADCL)
  // start the conversion
  sbi(ADCSRA, ADSC);

  // ADSC is cleared when the conversion finishes
  while (bit_is_set(ADCSRA, ADSC)) {
    // busy waiting
  }

  // we have to read ADCL first; doing so locks both ADCL
  // and ADCH until ADCH is read.  reading ADCL second would
  // cause the results of each conversion to be discarded,
  // as ADCL and ADCH would be locked when it completed.
  uint8_t low  = ADCL;
  uint8_t high = ADCH;
#else
  // we dont have an ADC, return 0
  uint8_t low  = 0;
  uint8_t high = 0;
#endif

  // combine the two bytes
  return (high << 8) | low;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void AnalogPin::write(int val) {
  int pin = pin_;
  // We need to make sure the PWM output is enabled for those pins
  // that support it, as we turn it off when digitally reading or
  // writing with them.  Also, make sure the pin is in output mode
  // for consistenty with Wiring, which doesn't require a pinMode
  // call for the analog output pins.
  set_mode(OUTPUT);
  if (val == 0) {
    set_high(false);
  } else if (val == 255) {
    set_high(true);
  } else {
    switch(digitalPinToTimer(pin)) {
    // XXX fix needed for atmega8
#if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
    case TIMER0A:
      // connect pwm to pin on timer 0
      sbi(TCCR0, COM00);
      OCR0 = val; // set pwm duty
      break;
#endif

#if defined(TCCR0A) && defined(COM0A1)
    case TIMER0A:
      // connect pwm to pin on timer 0, channel A
      sbi(TCCR0A, COM0A1);
      OCR0A = val; // set pwm duty
      break;
#endif

#if defined(TCCR0A) && defined(COM0B1)
    case TIMER0B:
      // connect pwm to pin on timer 0, channel B
      sbi(TCCR0A, COM0B1);
      OCR0B = val; // set pwm duty
      break;
#endif

#if defined(TCCR1A) && defined(COM1A1)
    case TIMER1A:
      // connect pwm to pin on timer 1, channel A
      sbi(TCCR1A, COM1A1);
      OCR1A = val; // set pwm duty
      break;
#endif

#if defined(TCCR1A) && defined(COM1B1)
    case TIMER1B:
      // connect pwm to pin on timer 1, channel B
      sbi(TCCR1A, COM1B1);
      OCR1B = val; // set pwm duty
      break;
#endif

#if defined(TCCR2) && defined(COM21)
    case TIMER2:
      // connect pwm to pin on timer 2
      sbi(TCCR2, COM21);
      OCR2 = val; // set pwm duty
      break;
#endif

#if defined(TCCR2A) && defined(COM2A1)
    case TIMER2A:
      // connect pwm to pin on timer 2, channel A
      sbi(TCCR2A, COM2A1);
      OCR2A = val; // set pwm duty
      break;
#endif

#if defined(TCCR2A) && defined(COM2B1)
    case TIMER2B:
      // connect pwm to pin on timer 2, channel B
      sbi(TCCR2A, COM2B1);
      OCR2B = val; // set pwm duty
      break;
#endif

#if defined(TCCR3A) && defined(COM3A1)
    case TIMER3A:
      // connect pwm to pin on timer 3, channel A
      sbi(TCCR3A, COM3A1);
      OCR3A = val; // set pwm duty
      break;
#endif

#if defined(TCCR3A) && defined(COM3B1)
    case TIMER3B:
      // connect pwm to pin on timer 3, channel B
      sbi(TCCR3A, COM3B1);
      OCR3B = val; // set pwm duty
      break;
#endif

#if defined(TCCR3A) && defined(COM3C1)
    case TIMER3C:
      // connect pwm to pin on timer 3, channel C
      sbi(TCCR3A, COM3C1);
      OCR3C = val; // set pwm duty
      break;
#endif

#if defined(TCCR4A)
    case TIMER4A:
      //connect pwm to pin on timer 4, channel A
      sbi(TCCR4A, COM4A1);
#if defined(COM4A0)     // only used on 32U4
      cbi(TCCR4A, COM4A0);
#endif
      OCR4A = val;    // set pwm duty
      break;
#endif

#if defined(TCCR4A) && defined(COM4B1)
    case TIMER4B:
      // connect pwm to pin on timer 4, channel B
      sbi(TCCR4A, COM4B1);
      OCR4B = val; // set pwm duty
      break;
#endif

#if defined(TCCR4A) && defined(COM4C1)
    case TIMER4C:
      // connect pwm to pin on timer 4, channel C
      sbi(TCCR4A, COM4C1);
      OCR4C = val; // set pwm duty
      break;
#endif

#if defined(TCCR4C) && defined(COM4D1)
    case TIMER4D:
      // connect pwm to pin on timer 4, channel D
      sbi(TCCR4C, COM4D1);
#if defined(COM4D0)     // only used on 32U4
      cbi(TCCR4C, COM4D0);
#endif
      OCR4D = val;    // set pwm duty
      break;
#endif

#if defined(TCCR5A) && defined(COM5A1)
    case TIMER5A:
      // connect pwm to pin on timer 5, channel A
      sbi(TCCR5A, COM5A1);
      OCR5A = val; // set pwm duty
      break;
#endif

#if defined(TCCR5A) && defined(COM5B1)
    case TIMER5B:
      // connect pwm to pin on timer 5, channel B
      sbi(TCCR5A, COM5B1);
      OCR5B = val; // set pwm duty
      break;
#endif

#if defined(TCCR5A) && defined(COM5C1)
    case TIMER5C:
      // connect pwm to pin on timer 5, channel C
      sbi(TCCR5A, COM5C1);
      OCR5C = val; // set pwm duty
      break;
#endif

    case NOT_ON_TIMER:
    default:
      if (val < 128) {
        set_high(false);
      } else {
        set_high(true);
      }
    }
  }
}

