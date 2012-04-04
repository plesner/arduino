#ifndef _ATL
#define _ATL

// A stack-allocated array that converts easily to a vector. This can
// be declared inline as, for example:
//
// elements<uint8_t, 3> elms = {{1, 2, 3}};
template <typename T, size_t kLength>
struct elements {
  // Returns the number of elements in this set.
  size_t length() { return kLength; }

  // Returns the i'th element.
  T &operator[](size_t i) { return data_[i]; }

  // The data in this elements set.
  T data_[kLength];
};

// An array together with a length.
template <typename T>
class vector {
public:
  // Creates a new vector with the given size and data.
  vector(size_t length, T *data)
    : length_(length)
    , data_(data) { }

  // Creates a new vector backed by the given elements.
  template <size_t kLength>
  vector(elements<T, kLength> elms)
    : length_(kLength)
    , data_(elms.data_) { }

  // Returns the i'th vector element.
  T &operator[](size_t i) { return data_[i]; }

  // Returns the number of elements in this vector.
  size_t length() { return length_; }

private:
  size_t length_;
  T *data_;
};

// If you want to allocate an instance of an object but don't want the
// C++ compiler to insert initialization code you can wrap the type
// in this class. It will allocate a piece of memory of the right size
// but will not attempt to initialize the data.
template <typename T>
class uninitialized {
public:
  // Returns the uninitialized memory cast as the object type.
  T &operator*();

private:
  uint8_t storage_[sizeof(T)];
};

template <typename T>
T &uninitialized<T>::operator*() {
  return *reinterpret_cast<T*>(&storage_);
}

#endif // _ATL
