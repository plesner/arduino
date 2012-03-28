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

template <typename T, size_t size>
struct ReadOnlyArray {
  // Returns the element of this array at the given index.
  T operator[](size_t index);

  // The underlying data.
  T data_[size];

private:

};

enum PinFlags {
  pNone = 0x0,
  pNotOnTimer = 0x1
};

#define FOR_EACH_PIN(F) \
  F(13, 0, _BV(5), 2)

template <uint8_t index>
struct PinInfo { };

#define __DEFINE_PIN__(index, TIMER, BIT, PORT)                      \
  template <>                                                        \
  struct PinInfo<index> {                                            \
    static const uint8_t kTimer = (TIMER);                           \
    static const uint8_t kBit = (BIT);                               \
    static const uint8_t kPort = (PORT);                             \
  };

FOR_EACH_PIN(__DEFINE_PIN__)

template <uint8_t index>
class StaticPin {
public:
  bool is_high();
  void set_high(bool value) { Pin::get(index).set_high(value); }
};

template <uint8_t index>
bool StaticPin<index>::is_high() {
  // static const uint8_t timer = PinInfo<index>::kTimer;
  static const uint8_t bit = PinInfo<index>::kBit;
  static const uint8_t port = PinInfo<index>::kPort;

  if (port == NOT_A_PIN) return false;

  // If the pin that support PWM output, we need to turn it off
  // before getting a digital reading.
  // if (timer != NOT_ON_TIMER) turnOffPWM(timer);

  return (*portInputRegister(port) & bit);
}

class PinUtils {
public:
  template <typename P>
  static void print(P pin, uint32_t value);
};

template <typename P>
void PinUtils::print(P pin, uint32_t value) {
  bool initial = pin.is_high();
  uint32_t current = value;
  do {
    bool is_on = current & 1;
    current = current >> 1;
    pin.set_high(true);
    delay(is_on ? 250 : 100);
    pin.set_high(false);
    delay(350);
  } while (current != 0);
  pin.set_high(initial);
}

namespace progmem {

template <size_t width>
class Reader {
public:
  template <typename T>
  static T read(T *address);
};

template <>
class Reader<1u> {
public:
  template <typename T>
  static T read(T *address) {
    return pgm_read_byte(address);
  }
};

template <>
class Reader<2u> {
public:
  template <typename T>
  static T read(T *address) {
    return pgm_read_word(address);
  }
};

template <>
class Reader<4u> {
public:
  template <typename T>
  static T read(T *address) {
    return pgm_read_dword(address);
  }
};

}

template <typename T, size_t size>
T ReadOnlyArray<T, size>::operator[](size_t index) {
  return progmem::Reader<sizeof(T)>::read(data_ + index);
}

#endif // _AVR
