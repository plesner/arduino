#ifndef _PIN
#define _PIN

#include "a.h"

// A generic pin.
class Pin {
public:
  Pin() : pin_(0xFF) { }
  Pin(uint8_t pin) : pin_(pin) { }

  // Sets the input/output mode of this pin.
  void set_mode(uint8_t mode);

  // Sets high/low voltage on this pin.
  void set_high(bool value);

  // Is this a valid connected pin? Pins created using the no-arg constructor
  // are not valid.
  bool is_valid() { return pin_ != 0xFF; }

protected:
  uint8_t pin_;

  // Turns off pulse width modulation for a given timer.
  static void disable_pwm(uint8_t timer);
};

// A digital pin.
class DigitalPin : public Pin {
public:
  DigitalPin() : Pin() { }
  DigitalPin(uint8_t pin) : Pin(pin) { }
  bool read();
private:
};

// An analog digital pin.
class AnalogPin : public Pin {
public:
  AnalogPin(uint8_t pin) : Pin(pin) { }
  int read();
  void write(int value);
private:
};

#endif // _PIN
