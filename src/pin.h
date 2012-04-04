#ifndef _PIN
#define _PIN

#include "read-only.h"

// Rudimentary timer control class. For now just contains constants.
struct Timers {
public:
  static const uint8_t kNotOnTimer = 0;
  static const uint8_t kTimer0A = 1;
  static const uint8_t kTimer0B = 2;
  static const uint8_t kTimer1A = 3;
  static const uint8_t kTimer1B = 4;
  static const uint8_t kTimer2  = 5;
  static const uint8_t kTimer2A = 6;
  static const uint8_t kTimer2B = 7;
  static const uint8_t kTimer3A = 8;
  static const uint8_t kTimer3B = 9;
  static const uint8_t kTimer3C = 10;
  static const uint8_t kTimer4A = 11;
  static const uint8_t kTimer4B = 12;
  static const uint8_t kTimer4C = 13;
  static const uint8_t kTimer4D = 14;
  static const uint8_t kTimer5A = 15;
  static const uint8_t kTimer5B = 16;
  static const uint8_t kTimer5C = 17;
};

// All the relevant metadata about a pin.
struct PinInfo {
  // The index of this pin's timer.
  int8_t timer : 5;

  // The mask used to set or unset this pin in the in/out registers.
  int8_t bit_index : 3;

  // The index of the port that controls this pin.
  int8_t port : 3;

  // Returns the array of pin info for the current platform.
  static read_only_vector<PinInfo> get();
};

// All the relevant metadata about a port.
struct PortInfo {
  // Symbolic names for the ports.
  static const int8_t kNotAPort = 0;
  static const int8_t kPb = 2;
  static const int8_t kPc = 3;
  static const int8_t kPd = 4;

  // Address of the mode register.
  volatile uint8_t *mode_register;

  // Address of the output register.
  volatile uint8_t *output_register;

  // Address of the input register.
  volatile uint8_t *input_register;

  // Returns the array of port info for the current platform.
  static read_only_vector<PortInfo> get();
};

// A single input/output pin. Create an instance by using the static
// constructor 'get'.
class Pin {
public:
  // Initialize an empty pin. Trying to use an empty pin will lead to
  // unpredictable results.
  Pin() { }

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

  Pin(uint8_t index)
      : pin_info_(PinInfo::get()[index])
      , port_info_(PortInfo::get()[pin_info().port]){ }

  // Returns true iff this is a valid pin.
  bool is_pin() { return pin_info().port != PortInfo::kNotAPort; }

  // Returns true if this pin is on a timer.
  bool is_on_timer() { return pin_info().timer != Timers::kNotOnTimer; }

  // Disables pulse-width modulation for this pin. If this pin is not
  // on a timer does nothing.
  void disable_pwm();

  // Returns the metadata about this pin.
  inline PinInfo &pin_info() { return pin_info_; }

  // Returns the metadata about the port this pin belongs to.
  inline PortInfo &port_info() { return port_info_; }

  // Metadata about this specific pin.
  PinInfo pin_info_;

  // Metadata about the port this pin belongs to.
  PortInfo port_info_;
};

#endif // _PIN
