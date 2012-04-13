#include "sreg-impl.h"
#include "main.h"
#include "platform.h"
#include "timer-impl.h"

bool Main::initialize_timers() {
  // This needs to be called before setup() or some functions won't
  // work there
  Interrupts::enable();

  // On the ATmega168 timer 0 is also used for fast hardware pwm
  // (using phase-correct PWM would mean that timer 0 overflowed half as often
  // resulting in different millis() behavior on the ATmega8 and ATmega168)
  if (Tccr0A::kPresent)
    Tccr0A::set_fast_pwm();

  // Set timer 0 prescale factor to 64
  if (Tccr0::kPresent) {
    Tccr0::set_prescale_factor<64>();
  } else if (Tccr0B::kPresent) {
    Tccr0B::set_prescale_factor<64>();
  } else if (Tccr0A::kPresent) {
    Tccr0A::set_prescale_factor<64>();
  } else {
    return false;
  }

  // enable timer 0 overflow interrupt
#if defined(TIMSK) && defined(TOIE0)
  FunctionRegister::set(TIMSK, TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
  FunctionRegister::set(TIMSK0, TOIE0);
#else
  #error  Timer 0 overflow interrupt not set correctly
#endif

  // timers 1 and 2 are used for phase-correct hardware pwm
  // this is better for motors as it ensures an even waveform
  // note, however, that fast pwm mode can achieve a frequency of up
  // 8 MHz (with a 16 MHz clock) at 50% duty cycle

#if defined(TCCR1B) && defined(CS11) && defined(CS10)
  TCCR1B = 0;

  // set timer 1 prescale factor to 64
  FunctionRegister::set(TCCR1B, CS11);
  #if F_CPU >= 8000000L
  FunctionRegister::set(TCCR1B, CS10);
  #endif
#elif defined(TCCR1) && defined(CS11) && defined(CS10)
  FunctionRegister::set(TCCR1, CS11);
  #if F_CPU >= 8000000L
  FunctionRegister::set(TCCR1, CS10);
  #endif
#endif
  // put timer 1 in 8-bit phase correct pwm mode
#if defined(TCCR1A) && defined(WGM10)
  FunctionRegister::set(TCCR1A, WGM10);
#elif defined(TCCR1)
  #warning this needs to be finished
#endif

  // set timer 2 prescale factor to 64
#if defined(TCCR2) && defined(CS22)
  FunctionRegister::set(TCCR2, CS22);
#elif defined(TCCR2B) && defined(CS22)
  FunctionRegister::set(TCCR2B, CS22);
#else
  #warning Timer 2 not finished (may not be present on this CPU)
#endif

  // configure timer 2 for phase correct pwm (8-bit)
#if defined(TCCR2) && defined(WGM20)
  FunctionRegister::set(TCCR2, WGM20);
#elif defined(TCCR2A) && defined(WGM20)
  FunctionRegister::set(TCCR2A, WGM20);
#else
  #warning Timer 2 not finished (may not be present on this CPU)
#endif

#if defined(TCCR3B) && defined(CS31) && defined(WGM30)
  FunctionRegister::set(TCCR3B, CS31);              // set timer 3 prescale factor to 64
  FunctionRegister::set(TCCR3B, CS30);
  FunctionRegister::set(TCCR3A, WGM30);             // put timer 3 in 8-bit phase correct pwm mode
#endif

#if defined(TCCR4B) && defined(CS41) && defined(WGM40)
  FunctionRegister::set(TCCR4B, CS41);              // set timer 4 prescale factor to 64
  FunctionRegister::set(TCCR4B, CS40);
  FunctionRegister::set(TCCR4A, WGM40);             // put timer 4 in 8-bit phase correct pwm mode
#endif

#if defined(TCCR5B) && defined(CS51) && defined(WGM50)
  FunctionRegister::set(TCCR5B, CS51);              // set timer 5 prescale factor to 64
  FunctionRegister::set(TCCR5B, CS50);
  FunctionRegister::set(TCCR5A, WGM50);             // put timer 5 in 8-bit phase correct pwm mode
#endif

#if defined(ADCSRA)
  // set a2d prescale factor to 128
  // 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
  // XXX: this will not work properly for other clock speeds, and
  // this code should use F_CPU to determine the prescale factor.
  FunctionRegister::set(ADCSRA, ADPS2);
  FunctionRegister::set(ADCSRA, ADPS1);
  FunctionRegister::set(ADCSRA, ADPS0);

  // enable a2d conversions
  FunctionRegister::set(ADCSRA, ADEN);
#endif

  // the bootloader connects pins 0 and 1 to the USART; disconnect them
  // here so they can be used as normal digital i/o; they will be
  // reconnected in Serial.begin()
#if defined(UCSRB)
  UCSRB = 0;
#elif defined(UCSR0B)
  UCSR0B = 0;
#endif

  return true;
}

extern void serialEventRun(void) __attribute__((weak));

int Main::main() {
  initialize_timers();
  setup();
  while (true) {
    loop();
    if (serialEventRun)
      serialEventRun();
  }
  return 0;
}

// Main entry-point.
int main() {
  return Main::main();
}
