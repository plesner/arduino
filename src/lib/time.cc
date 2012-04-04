#include <Arduino.h>
#include "avr.h"
#include "time.h"

volatile unsigned long Time::timer0_overflow_count = 0;
volatile unsigned long Time::timer0_millis = 0;
unsigned char Time::timer0_fract = 0;

uint16_t Time::micros() {
  uint16_t m;
  uint8_t t;

  {
    Interrupts::DisableDuring disable_interrupts;
    m = timer0_overflow_count;

#if defined(TCNT0)
    t = TCNT0;
#elif defined(TCNT0L)
    t = TCNT0L;
#else
    #error TIMER 0 not defined
#endif


#ifdef TIFR0
    if ((TIFR0 & _BV(TOV0)) && (t < 255))
      m++;
#else
    if ((TIFR & _BV(TOV0)) && (t < 255))
      m++;
#endif
  }

  return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void Time::sleep(duration_t duration) {
  uint16_t start = static_cast<uint16_t>(micros());
  while (duration > 0) {
    if ((static_cast<uint16_t>(micros()) - start) >= 1000) {
      duration--;
      start += 1000;
    }
  }
}


// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

SIGNAL(TIMER0_OVF_vect) {
  Time::tick();
}

void Time::tick() {
  // copy these to local variables so they can be stored in registers
  // (volatile variables must be read from memory on every access)
  uint16_t m = timer0_millis;
  uint8_t f = timer0_fract;

  m += MILLIS_INC;
  f += FRACT_INC;
  if (f >= FRACT_MAX) {
          f -= FRACT_MAX;
          m += 1;
  }

  timer0_fract = f;
  timer0_millis = m;
  timer0_overflow_count++;
}
