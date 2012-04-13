#undef ARDUINO_MAIN

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define IF_ELSE(cCond, T, F) cCond(T, F)

#define cAtMega8(T, F) F
