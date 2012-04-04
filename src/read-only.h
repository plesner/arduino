#ifndef _READ_ONLY
#define _READ_ONLY

#include "atl.h"

// A literal set of read-only elements that converts easily to a read
// only vector. This can be declared inline as, for example:
//
// read_only_elements<uint8_t, 3> elms = {{1, 2, 3}};
template <typename T, size_t kLength>
struct read_only_elements {
  // Returns the number of elements in this set.
  size_t length() { return kLength; }

  // Returns the i'th element.
  T operator[](size_t i);

  // The data in this elements set.
  T data_[kLength];
};

template <typename T>
class read_only_vector {
public:
  template <size_t kLength>
  read_only_vector(read_only_elements<T, kLength> &array)
    : length_(kLength)
    , data_(array.data_) { }

  // Returns the element of this array at the given index.
  T operator[](size_t index);

  // Returns the number of elements in this vector.
  size_t length() { return length_; }

private:
  // Number of elements.
  size_t length_;

  // The read-only data wrapped by this array.
  T *data_;
};

#endif // _READ_ONLY
