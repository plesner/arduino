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

// A single input/output pin. Create an instance by using the static
// constructor 'get'.
class Pin {
public:
  // Which direction data flows, in or out of a pin.
  enum DataDirection { IN, OUT };

  // The current value of a pin, high or low.
  enum Value { OFF = 0, ON = 1 };

  // Returns the index'th pin.
  static Pin get(uint8_t index) { return Pin(index); }

  // Sets the data direction of this pin.
  void set_data_direction(DataDirection value);

  // Sets the value of this pin.
  void set_high(bool is_on);

  // Returns true iff this pin is high.
  bool is_high();

  // Displays the given value on this pin in binary.
  void print(uint32_t value);

private:
  Pin(uint8_t index) : index_(index) { }

  // The index of the pin.
  uint8_t index_;
};

#endif // _AVR
