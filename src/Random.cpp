#include "Random.h"

/**
 * Initializes the random number generator.
 */
void Random::initialize() {
  srand((int) time(NULL));
}

/**
 * Returns a random integer number in [0, x[ with a uniform distribution.
 * @param x the superior bound
 * @return a random integer number in [0, x[
 */
int Random::get_number(unsigned int x) {
  return (int) ((double) x * rand() / (RAND_MAX + 1.0));
}

/**
 * Returns a random integer number in [x, y[ with a uniform distribution.
 * This is equivalent to: x + get_number(y)
 * @param x the inferior bound
 * @param y the superior bound
 * @return a random integer number in [x, y[
 */
int Random::get_number(unsigned int x, unsigned int y) {
  return x + get_number(y);
}

