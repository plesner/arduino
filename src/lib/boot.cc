#include "time.h"
#include <Arduino.h>

extern "C" void __cxa_pure_virtual(void) {
  while(1);
}

int main(void) {
  Time::initialize();
  setup();
  while (true) {
    loop();
    if (serialEventRun)
      serialEventRun();
  }
  return 0;
}
