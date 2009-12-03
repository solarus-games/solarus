/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "lowlevel/Random.h"

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

