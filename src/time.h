#ifndef _TIME
#define _TIME

#include <stdint.h>

// A time interval measured in milliseconds.
typedef uint32_t duration_t;

class Time {
public:
  static void sleep(duration_t millis);

  static void tick();

private:
  static uint16_t micros();

  static volatile unsigned long timer0_overflow_count;
  static volatile unsigned long timer0_millis;
  static unsigned char timer0_fract;
};

class Duration {
public:
  static inline duration_t millis(uint32_t value) { return value; }
  static inline duration_t seconds(uint32_t value) { return value * 1000; }
};

#endif // _TIME
