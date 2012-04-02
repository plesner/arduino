#ifndef _MAIN
#define _MAIN

// Container class for the methods used to run the device.
class Main {
public:
  // This method is called on startup.
  static void setup();

  // This method is called repeatedly.
  static void loop();

  // Main entry-point called from the system entry-point.
  static int main();

private:
  // Sets up timers.
  static void initialize_timers();
};

#endif // _MAIN
