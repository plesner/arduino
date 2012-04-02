#ifndef _PIN
#define _PIN

#include "read-only.h"

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

struct PinInfo {
  int8_t timer;
  int8_t bit_mask;
  int8_t port;
  int8_t padding;

  static read_only_array<PinInfo> get();
};

struct PortInfo {
  static const int8_t kNotAPort = 0;
  static const int8_t kPb = 2;
  static const int8_t kPc = 3;
  static const int8_t kPd = 4;

  volatile uint8_t *mode_register;
  volatile uint8_t *output_register;
  volatile uint8_t *input_register;

  static read_only_array<PortInfo> get();
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
