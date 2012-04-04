#ifndef _SREG
#define _SREG

#include "platform.h"

// Helper class for interacting with interrupts.
class Interrupts {
public:
  // Disables all interrupts by clearing the global interrupt mask.
  static inline void disable() {
    __asm__ __volatile__ ("cli" ::);
  }

  // Enables all interrupts by setting the global interrupt mask.
  static inline void enable() {
    __asm__ __volatile__ ("sei" ::);
  }

  // Stack-allocated class that turns off interrupts while it is in
  // scope and restores them on destruction.
  class DisableDuring {
  public:
    // Capture the current status registers and disables interrupts.
    inline DisableDuring();
    // Restores the status registers that were active before.
    inline ~DisableDuring();
  private:
    uint8_t prev_status_registers_;
  };
};

#endif // _SREG
