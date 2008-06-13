#ifndef ZSDX_RANDOM_H
#define ZSDX_RANDOM_H

#include "Common.h"

/**
 * Provides some functions to compute random numbers.
 */
class Random {

 private:

  Random();

 public:

  static void initialize();
  static int get_number(unsigned int x);
  static int get_number(unsigned int x, unsigned int y);

};

#endif
