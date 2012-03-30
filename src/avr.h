#ifndef _AVR
#define _AVR

#include <stdint.h>

// A time interval measured in milliseconds.
typedef uint32_t delay_t;

// Helper class for interacting with interrupts.
class Interrupts {
public:
  // Disables all interrupts.
  static void disable();

  // Enables all interrupts.
  static void enable();
};

#endif // _AVR
