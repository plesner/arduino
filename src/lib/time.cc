#include "sreg-impl.h"
#include "time.h"

#include <avr/interrupt.h>
#include <avr/io.h>

// F_CPU is configured externally by the build script and is expressed
// in cycles per second.

volatile uint32_t Time::timer0_overflow_count = 0;
volatile uint32_t Time::timer0_millis = 0;
uint8_t Time::timer0_remainders = 0;

uint32_t Time::cycles_to_micros(uint32_t cycles) {
  return (cycles * 1000L) / kCyclesPerMilli;
}

uint16_t Time::current_time_micros() {
  uint16_t overflow_count;
  uint8_t time;

  {
    // We have to disable interrupts while reading these variables and
    // registers.
    Interrupts::DisableDuring disable_interrupts;

    overflow_count = timer0_overflow_count;

    // Read out the current time from the time counter.
#if defined(TCNT0)
    time = TCNT0;
#elif defined(TCNT0L)
    time = TCNT0L;
#else
    #error TIMER 0 not defined
#endif

    // If the timer overflow flag is set we increment the overflow
    // count to take that into account.
#ifdef TIFR0
    if ((TIFR0 & _BV(TOV0)) && (time < 255))
      overflow_count++;
#else
    if ((TIFR & _BV(TOV0)) && (time < 255))
      overflow_count++;
#endif
  }

  // There is an overflow per 256 ticks, plus the ticks currently
  // counted.
  uint16_t tick_count = ((overflow_count << 8) + time);

  // There are 64 cycles per tick.
  return tick_count * (64 / kCyclesPerMicro);
}

void Time::sleep(duration_t duration) {
  static const uint16_t kGranularity = 500;
  // This is the last time we updated the variables.
  uint16_t checkpoint = current_time_micros();
  // This is how long we have left to wait.
  duration_t remaining = duration;
  while (remaining > 0) {
    uint16_t now = current_time_micros();
    uint16_t elapsed_since_checkpoint = now - checkpoint;
    if (elapsed_since_checkpoint >= kGranularity) {
      // If we've moved more than the granularity ahead of the checkpoint
      // we move it forward and decrease the time that's left to wait.
      remaining -= kGranularity;
      checkpoint += kGranularity;
    }
  }
}

SIGNAL(TIMER0_OVF_vect) {
  Time::on_overflow();
}

void Time::on_overflow() {
  // The prescaler ensure that we only tick every 64 clock cycles.
  uint32_t ticks_per_cycle = 64;
  // The timer's overflow handler is called every 256 ticks.
  uint32_t overflows_per_tick = 256;
  // There's this many micros between overflows.
  uint32_t micros_per_overflow = cycles_to_micros(ticks_per_cycle * overflows_per_tick);
  // Which corresponds to this many whole milliseconds.
  uint32_t whole_millis_per_overflow = micros_per_overflow / 1000L;
  // And a remainder of this much. We discard the top three bits so it
  // will fit into a byte which doesn't lose precision for 8 and 16MHz.
  uint32_t remainder_per_overflow = (micros_per_overflow % 1000) >> 3;
  // The remainers add up to a whole milli after this long.
  uint32_t remainders_per_milli = 1000 >> 3;

  // We read these values into locals since they're volatile so re-
  // reading them will go to memory.
  uint16_t millis = timer0_millis;
  uint8_t remainder = timer0_remainders;

  millis += whole_millis_per_overflow;
  remainder += remainder_per_overflow;
  if (remainder >= remainders_per_milli) {
    remainder -= remainders_per_milli;
    millis += 1;
  }

  timer0_remainders = remainder;
  timer0_millis = millis;
  timer0_overflow_count++;
}
