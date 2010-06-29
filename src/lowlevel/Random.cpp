/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "lowlevel/Random.h"
#include <ctime>
#include <cstdlib>

/**
 * @brief Initializes the random number generator.
 */
void Random::initialize() {
  srand((int) time(NULL));
}

/**
 * @brief Uninitializes the random number generator.
 */
void Random::quit(void) {
  // nothing to do
}

/**
 * @brief Returns a random integer number in [0, x[ with a uniform distribution.
 *
 * This is equivalent to: Random::get_number(0, x).
 *
 * @param x the superior bound
 * @return a random integer number in [0, x[
 */
int Random::get_number(unsigned int x) {
  return (int) ((double) x * rand() / (RAND_MAX + 1.0));
}

/**
 * @brief Returns a random integer number in [x, y[ with a uniform distribution.
 * @param x the inferior bound
 * @param y the superior bound
 * @return a random integer number in [x, y[
 */
int Random::get_number(unsigned int x, unsigned int y) {
  return x + get_number(y - x);
}

