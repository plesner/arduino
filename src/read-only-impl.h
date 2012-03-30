#ifndef _READ_ONLY_IMPL
#define _READ_ONLY_IMPL

#include "read-only.h"

template <size_t kSize>
class ReadOnlyArrayByteReader {
public:
  static const size_t kIndex = kSize - 1;
  static inline void read(uint16_t address, uint8_t *target) {
    ReadOnlyArrayByteReader<kSize - 1>::read(address, target);
    target[kIndex] = pgm_read_byte(address + kIndex);
  }
};

template <>
class ReadOnlyArrayByteReader<0> {
public:
  static inline void read(uint16_t address, uint8_t *target) { }
};

template <typename T>
T read_only_array<T>::operator[](size_t index) {
  union { uint8_t in[sizeof(T)]; T out; } converter;
  uint16_t address = reinterpret_cast<uint16_t>(data_ + index);
  ReadOnlyArrayByteReader<sizeof(T)>::read(address, converter.in);
  return converter.out;
}

#endif // _READ_ONLY_IMPL
