#ifndef _TIME
#define _TIME

#include <stdint.h>

// A time interval measured in milliseconds.
typedef uint32_t duration_t;

class Time {
public:
  // Suspends execution for the specified duration.
  static void sleep(duration_t millis);

  // Called when the hardware timer ticks.
  static void tick();

private:
  // Returns the current microsecond count.
  static uint16_t micros();

  static volatile unsigned long timer0_overflow_count;
  static volatile unsigned long timer0_millis;
  static unsigned char timer0_fract;
};

// Factory class for producing durations.
class Duration {
public:
  // Returns a duration value of the given number of milliseconds.
  static inline duration_t millis(uint32_t value) { return value; }

  // Returns a duration value of the given number of seconds.
  static inline duration_t seconds(uint32_t value) { return value * 1000; }
};

#endif // _TIME
