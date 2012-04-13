#ifndef _TIMER
#define _TIMER

#include <avr/io.h>

#if defined(TCCR0)
#define cHasTccr0(T, F) T
#else
#define cHasTccr0(T, F) F
#endif

#if defined(TCCR0A)
#define cHasTccr0A(T, F) T
#else
#define cHasTccr0A(T, F) F
#endif

#if defined(TCCR0B)
#define cHasTccr0B(T, F) T
#else
#define cHasTccr0B(T, F) F
#endif

// Timer/Counter Control Register 0.
class Tccr0 {
public:
  // Does this TCCR exist on this platform?
  static const bool kPresent = IF_ELSE(cHasTccr0, true, false);

  // Sets the prescale factor for this tccr.
  template <uint8_t kFactor> static void set_prescale_factor();
};

// Timer/Counter Control Register 0A.
class Tccr0A {
public:
  // Does this TCCR exist on this platform?
  static const bool kPresent = IF_ELSE(cHasTccr0A, true, false);

  // If it is present, sets the timer in fast PWM.
  static inline void set_fast_pwm();

  // Sets the prescale factor for this tccr.
  template <uint8_t kFactor> static void set_prescale_factor();
};

// Timer/Counter Control Register 0B.
class Tccr0B {
public:
  // Does this TCCR exist on this platform?
  static const bool kPresent = IF_ELSE(cHasTccr0B, true, false);

  // Sets the prescale factor for this tccr.
  template <uint8_t kFactor> static void set_prescale_factor();
};

#endif // _TIMER
