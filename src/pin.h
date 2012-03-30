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
  static const int8_t kNotAPin = 0;
  static const int8_t kPb = 2;
  static const int8_t kPc = 3;
  static const int8_t kPd = 4;

  int8_t timer;
  int8_t bit_mask;
  int8_t port;
  int8_t padding;

  static read_only_array<PinInfo> get();
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
  Pin(uint8_t index) : info_(PinInfo::get()[index]) { }

  bool is_pin() { return info().port != PinInfo::kNotAPin; }

  bool is_on_timer() { return info().timer != Timers::kNotOnTimer; }

  // Returns the metadata about this pin.
  inline PinInfo info() { return info_; }

  // The index of the pin.
  PinInfo info_;
};

#endif // _PIN
