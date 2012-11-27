#ifndef _PLATFORM
#define _PLATFORM

#include <Arduino.h>

// This file transforms the #define style conditional symbols into condition
// codes that can be used to the same effect in pure C, without preprocessor
// magic. For each #define (say XXX) this file introduces a condition code,
// ccXxx, which can be used either as an argument to eval_cc which returns a
// static boolean if the condition is true, or as a function with two arguments
// which evaluates statically to the first if the condition is true and the
// second otherwise.

// Evaluates a condition code to true or false
#define eval_cc(CC) CC(true, false)

// Timer/Counter1 Control Register A
#if defined(TCCR1A)
#define ccHasTccr1A(IF_TRUE, IF_FALSE) IF_TRUE
#else
#define ccHasTccr1A(IF_TRUE, IF_FALSE) IF_FALSE
#define TCCR1A __ERROR__
#endif

#if defined(COM1A1)
#define ccHasCom1A1(IF_TRUE, IF_FALSE) IF_TRUE
#else
#define ccHasCom1A1(IF_TRUE, IF_FALSE) IF_FALSE
#define COM1A1 __ERROR__
#endif

#if defined(COM1B1)
#define ccHasCom1B1(IF_TRUE, IF_FALSE) IF_TRUE
#else
#define ccHasCom1B1(IF_TRUE, IF_FALSE) IF_FALSE
#define COM1B1 __ERROR__
#endif

#if defined(__AVR_ATmega1280__)
#define ccATmega1280(IF_TRUE, IF_FALSE) IF_TRUE
#else
#define ccATmega1280(IF_TRUE, IF_FALSE) IF_FALSE
#endif

#if defined(__AVR_ATmega2560__)
#define ccATmega2560(IF_TRUE, IF_FALSE) IF_TRUE
#else
#define ccATmega2560(IF_TRUE, IF_FALSE) IF_FALSE
#endif

#if defined(__AVR_ATmega32U4__)
#define ccATmega32U4(IF_TRUE, IF_FALSE) IF_TRUE
#else
#define ccATmega32U4(IF_TRUE, IF_FALSE) IF_FALSE
#endif

#if defined(__AVR_ATmega1284__)
#define ccATmega1284(IF_TRUE, IF_FALSE) IF_TRUE
#else
#define ccATmega1284(IF_TRUE, IF_FALSE) IF_FALSE
#endif

#endif // _PLATFORM
