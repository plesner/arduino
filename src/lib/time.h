#ifndef _TIME
#define _TIME

#include <stdint.h>

// A time interval measured in milliseconds.
typedef uint32_t duration_t;

class Time {
public:
  // Suspends execution for the specified number of microseconds.
  static void sleep(duration_t micros);

  // Called when the hardware timer overflows.
  static void on_overflow();

  // Returns a duration value of the given number of microseconds.
  static inline duration_t micros(uint32_t value) { return value; }

  // Returns a duration value of the given number of milliseconds.
  static inline duration_t millis(uint32_t value) { return micros(value * 1000); }

  // Returns a duration value of the given number of seconds.
  static inline duration_t seconds(uint32_t value) { return millis(value * 1000); }

private:
  // Returns the current microsecond count.
  static uint16_t current_time_micros();

  static const uint32_t kCyclesPerSecond = F_CPU;
  static const uint32_t kCyclesPerMilli = kCyclesPerSecond / 1000L;
  static const uint32_t kCyclesPerMicro = kCyclesPerMilli / 1000L;

  static inline uint32_t cycles_to_micros(uint32_t value);

  // These two are volatile since they're written both by interrupts
  // and by normal code.
  static volatile uint32_t timer0_overflow_count;
  static volatile uint32_t timer0_millis;

  // This one is only ever read/written by interrupts so it doesn't
  // have to be volatile.
  static uint8_t timer0_remainders;
};

#endif // _TIME
