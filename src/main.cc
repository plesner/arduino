#include <Arduino.h>

#include "main.h"

// Main entry-point.
int main() {
  init();
  Main::setup();
  while (true) {
    Main::loop();
    if (serialEventRun) serialEventRun();
  }
  return 0;
}
