#ifndef _TIME
#define _TIME

#include <inttypes.h>

typedef unsigned long duration_t;

// Utility for timekeeping.
class Time {
public:
  // Sleep the specified number of milliseconds.
  static void sleep_millis(duration_t millis);

  // Sleep the specified number of microseconds.
  static void sleep_micros(duration_t micros);

  // Initialize timekeeping. Must be called once immediately after the controller
  // has started.
  static void initialize();
};

#endif // _TIME
