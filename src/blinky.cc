#include "lib/atl.h"
#include "lib/main.h"
#include "lib/read-only-impl.h"
#include "lib/shift.h"
#include "lib/time.h"

class MainData {
public:
  void initialize();

  ShiftRegister &shift() { return shift_; }
private:
  ShiftRegister shift_;
};

static uninitialized<MainData> main_data;

void MainData::initialize() {
  shift_ = ShiftRegister(9, 11, 12);
}

void Main::setup() {
  main_data->initialize();
}

// A helper class that turns the bits of a byte on and off such that
// they are turned on a certain per255age of the time.
class ByteGradient {
public:
  // Create a new byte gradient where each bit is on 'initial' per255 of
  // the time.
  ByteGradient(uint8_t initial);

  // Returns the current mask.
  uint8_t mask() { return mask_; }

  // Advances the mask one step.
  void advance();

  // Returns the index'th alpha entry that decides how bright the given
  // bit is.
  uint8_t &operator[](size_t index) { return alphas_[index]; }
private:
  static const size_t kSize = 8;
  uint8_t alphas_[kSize];
  uint8_t current_[kSize];
  uint8_t mask_;
};

ByteGradient::ByteGradient(uint8_t initial) {
  for (uint8_t i = 0; i < kSize; i++) {
    alphas_[i] = initial;
    current_[i] = 0x00;
  }
  mask_ = 0xFF;
}

void ByteGradient::advance() {
  mask_ = 0x00;
  for (uint8_t i = 0; i < kSize; i++) {
    uint16_t value = static_cast<uint16_t>(current_[i]) + alphas_[i];
    if (value >= 0xFF) {
      mask_ |= (1 << i);
    }
    current_[i] = value & 0xFF;
  }
}

// Utility for scanning an intensity around a byte mask.
class Scanner {
public:
  Scanner(uint16_t step)
    : gradient_(0xFF)
    , limit_(step)
    , step_(0) { }
  uint8_t mask() { return gradient_.mask(); }
  void advance();
  ByteGradient &gradient() { return gradient_; }
private:
  ByteGradient gradient_;
  uint16_t limit_;
  uint16_t step_;
};

void Scanner::advance() {
  if (step_ >= limit_) {
    step_ = 0;
    uint8_t first = gradient_[0];
    for (uint8_t i = 0; i < 7; i++)
      gradient_[i] = gradient_[i+1];
    gradient_[7] = first;
  } else {
    step_++;
  }
  gradient_.advance();
}

void Main::loop() {
  ShiftRegister &shift = main_data->shift();
  Scanner scanner(0x500);
  scanner.gradient()[0] = 0x20;
  scanner.gradient()[1] = 0x40;
  scanner.gradient()[2] = 0x80;
  scanner.gradient()[3] = 0xFF;
  scanner.gradient()[4] = 0xFF;
  scanner.gradient()[5] = 0x80;
  scanner.gradient()[6] = 0x40;
  scanner.gradient()[7] = 0x20;
  for (uint8_t i = 0x00; true; i++) {
    for (uint16_t step = 0; step < 0x1000; step++) {
      shift.set_value(scanner.mask() & i);
      scanner.advance();
    }
  }
}
