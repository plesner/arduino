#ifndef _READ_ONLY
#define _READ_ONLY

template <typename T, size_t size>
struct ReadOnlyArray {
  // Returns the element of this array at the given index.
  T operator[](size_t index);

  // The underlying data.
  T data_[size];
};

#endif // _READ_ONLY
