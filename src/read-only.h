#ifndef _READ_ONLY
#define _READ_ONLY

template <typename T, size_t size>
struct read_only_array_block {
  // The underlying data.
  T data_[size];
};

template <typename T>
class read_only_array {
public:
  template <size_t size>
  read_only_array(read_only_array_block<T, size> &array)
    : data_(array.data_) { }

  // Returns the element of this array at the given index.
  T operator[](size_t index);

private:
  // The read-only data wrapped by this array.
  T *data_;
};

#endif // _READ_ONLY
