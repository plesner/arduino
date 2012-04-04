#ifndef _INTERRUPT_IMPL
#define _INTERRUPT_IMPL

#include "interrupt.h"

Interrupts::DisableDuring::DisableDuring()
  : prev_status_registers_(SREG) {
  Interrupts::disable();
}

Interrupts::DisableDuring::~DisableDuring() {
  SREG = prev_status_registers_;
}

#endif // _INTERRUPT_IMPL
