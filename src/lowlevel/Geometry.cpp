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

#include "lowlevel/Geometry.h"
#include <cmath>

/**
 * Mathematic constants.
 * We don't use the constants from math.h
 * because they are not ANSI.
 */
const double Geometry::PI = 3.14159265358979323846;
const double Geometry::TWO_PI = 2.0 * PI;
const double Geometry::PI_OVER_2 = PI / 2.0;
const double Geometry::THREE_PI_OVER_2 = 3 * PI_OVER_2;
const double Geometry::SQRT_2 = 1.41421356237309504880;

/**
 * Returns the distance between two points.
 * @param x1 x coordinate of the first point
 * @param y1 y coordinate of the first point
 * @param x2 x coordinate of the second point
 * @param y2 y coordinate of the second point
 * @return the distance in pixels
 */
double Geometry::get_distance(int x1, int y1, int x2, int y2) { 
  int dx = x2 - x1;
  int dy = y2 - y1;
  return sqrt(dx * dx + dy * dy);
}

/**
 * Returns the angle of the vector formed by two points.
 * @param x1 x coordinate of the first point
 * @param y1 y coordinate of the first point
 * @param x2 x coordinate of the second point
 * @param y2 y coordinate of the second point
 * @return the angle in radians
 */
double Geometry::get_angle(int x1, int y1, int x2, int y2) {

  int dx = x2 - x1;
  int dy = y2 - y1;

  double angle;
  if (dx != 0) {
    angle = atan((double) -dy / (double) dx);

    if (dx < 0) {
      angle += PI;
    }
  }
  else {
    // special case (cannot divide by zero and compute atan)
    angle = (dy > 0) ? -PI_OVER_2 : PI_OVER_2;
  }

  return angle;
}

