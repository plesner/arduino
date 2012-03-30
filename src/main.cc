#include <Arduino.h>

#include "main.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void init()
{
        // this needs to be called before setup() or some functions won't
        // work there
        sei();

        // on the ATmega168, timer 0 is also used for fast hardware pwm
        // (using phase-correct PWM would mean that timer 0 overflowed half as often
        // resulting in different millis() behavior on the ATmega8 and ATmega168)
#if defined(TCCR0A) && defined(WGM01)
        sbi(TCCR0A, WGM01);
        sbi(TCCR0A, WGM00);
#endif

        // set timer 0 prescale factor to 64
#if defined(__AVR_ATmega128__)
        // CPU specific: different values for the ATmega128
        sbi(TCCR0, CS02);
#elif defined(TCCR0) && defined(CS01) && defined(CS00)
        // this combination is for the standard atmega8
        sbi(TCCR0, CS01);
        sbi(TCCR0, CS00);
#elif defined(TCCR0B) && defined(CS01) && defined(CS00)
        // this combination is for the standard 168/328/1280/2560
        sbi(TCCR0B, CS01);
        sbi(TCCR0B, CS00);
#elif defined(TCCR0A) && defined(CS01) && defined(CS00)
        // this combination is for the __AVR_ATmega645__ series
        sbi(TCCR0A, CS01);
        sbi(TCCR0A, CS00);
#else
        #error Timer 0 prescale factor 64 not set correctly
#endif

        // enable timer 0 overflow interrupt
#if defined(TIMSK) && defined(TOIE0)
        sbi(TIMSK, TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
        sbi(TIMSK0, TOIE0);
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
        sbi(TCCR1B, CS11);
#if F_CPU >= 8000000L
        sbi(TCCR1B, CS10);
#endif
#elif defined(TCCR1) && defined(CS11) && defined(CS10)
        sbi(TCCR1, CS11);
#if F_CPU >= 8000000L
        sbi(TCCR1, CS10);
#endif
#endif
        // put timer 1 in 8-bit phase correct pwm mode
#if defined(TCCR1A) && defined(WGM10)
        sbi(TCCR1A, WGM10);
#elif defined(TCCR1)
        #warning this needs to be finished
#endif

        // set timer 2 prescale factor to 64
#if defined(TCCR2) && defined(CS22)
        sbi(TCCR2, CS22);
#elif defined(TCCR2B) && defined(CS22)
        sbi(TCCR2B, CS22);
#else
        #warning Timer 2 not finished (may not be present on this CPU)
#endif

        // configure timer 2 for phase correct pwm (8-bit)
#if defined(TCCR2) && defined(WGM20)
        sbi(TCCR2, WGM20);
#elif defined(TCCR2A) && defined(WGM20)
        sbi(TCCR2A, WGM20);
#else
        #warning Timer 2 not finished (may not be present on this CPU)
#endif

#if defined(TCCR3B) && defined(CS31) && defined(WGM30)
        sbi(TCCR3B, CS31);              // set timer 3 prescale factor to 64
        sbi(TCCR3B, CS30);
        sbi(TCCR3A, WGM30);             // put timer 3 in 8-bit phase correct pwm mode
#endif

#if defined(TCCR4B) && defined(CS41) && defined(WGM40)
        sbi(TCCR4B, CS41);              // set timer 4 prescale factor to 64
        sbi(TCCR4B, CS40);
        sbi(TCCR4A, WGM40);             // put timer 4 in 8-bit phase correct pwm mode
#endif

#if defined(TCCR5B) && defined(CS51) && defined(WGM50)
        sbi(TCCR5B, CS51);              // set timer 5 prescale factor to 64
        sbi(TCCR5B, CS50);
        sbi(TCCR5A, WGM50);             // put timer 5 in 8-bit phase correct pwm mode
#endif

#if defined(ADCSRA)
        // set a2d prescale factor to 128
        // 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
        // XXX: this will not work properly for other clock speeds, and
        // this code should use F_CPU to determine the prescale factor.
        sbi(ADCSRA, ADPS2);
        sbi(ADCSRA, ADPS1);
        sbi(ADCSRA, ADPS0);

        // enable a2d conversions
        sbi(ADCSRA, ADEN);
#endif

        // the bootloader connects pins 0 and 1 to the USART; disconnect them
        // here so they can be used as normal digital i/o; they will be
        // reconnected in Serial.begin()
#if defined(UCSRB)
        UCSRB = 0;
#elif defined(UCSR0B)
        UCSR0B = 0;
#endif
}


// Main entry-point.
int main() {
  init();
  Main::setup();
  while (true) {
    Main::loop();
    if (serialEventRun) serialEventRun();
  }
  return 0;
}
