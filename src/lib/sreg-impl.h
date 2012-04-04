#ifndef _SREG_IMPL
#define _SREG_IMPL

#include "sreg.h"

#include <avr/common.h>

Interrupts::DisableDuring::DisableDuring()
  : prev_status_registers_(SREG) {
  Interrupts::disable();
}

Interrupts::DisableDuring::~DisableDuring() {
  SREG = prev_status_registers_;
}

#endif // _SREG_IMPL
