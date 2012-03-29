#ifndef _READ_ONLY_IMPL
#define _READ_ONLY_IMPL

#include "read-only.h"

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

} // progmem

template <typename T, size_t size>
T ReadOnlyArray<T, size>::operator[](size_t index) {
  return progmem::Reader<sizeof(T)>::read(data_ + index);
}

#endif // _READ_ONLY_IMPL
