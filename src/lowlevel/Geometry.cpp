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
 * @brief The pi constant.
 */
const double Geometry::PI = 3.14159265358979323846;

/**
 * @brief 2 * pi.
 */
const double Geometry::TWO_PI = 2.0 * PI;

/**
 * @brief pi / 2.
 */
const double Geometry::PI_OVER_2 = PI / 2.0;

/**
 * @brief 3 * pi / 2.
 */
const double Geometry::THREE_PI_OVER_2 = 3 * PI_OVER_2;

/**
 * @brief Square root of 2.
 */
const double Geometry::SQRT_2 = 1.41421356237309504880;

/**
 * @brief Converts an angle in radians into an angle in degrees.
 * @param radians angle in radians
 * @return the degrees value
 */
const int Geometry::radians_to_degrees(double radians) {
  return (int) (radians * 360.0 / TWO_PI);
}

/**
 * @brief Converts an angle in degrees into an angle in radians.
 * @param degrees angle in degrees
 * @return the radians value
 */
const double Geometry::degrees_to_radians(double degrees) {
  return degrees * TWO_PI / 360.0;
}

/**
 * @brief Returns the distance between two points.
 * @param x1 x coordinate of the first point
 * @param y1 y coordinate of the first point
 * @param x2 x coordinate of the second point
 * @param y2 y coordinate of the second point
 * @return the distance in pixels
 */
double Geometry::get_distance(int x1, int y1, int x2, int y2) {

  int dx = x2 - x1;
  int dy = y2 - y1;
  return std::sqrt(dx * dx + dy * dy);
}

/**
 * @brief Returns the distance between two points.
 * @param xy1 coordinates of the first point
 * @param xy2 coordinates of the second point
 * @return the distance in pixels
 */
double Geometry::get_distance(const Rectangle& xy1, const Rectangle& xy2) {

  return get_distance(xy1.get_x(), xy1.get_y(), xy2.get_x(), xy2.get_y());
}

/**
 * @brief Returns the angle of the vector formed by two points.
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
    angle = std::atan((double) -dy / (double) dx);

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

/**
 * @brief Returns the cartesian coordinates of a vector that starts from the origin, given its angle and distance.
 * @param angle angle of the vector in radians
 * @param distance length of the vector in pixels
 * @return the coordinates of the second point
 */
const Rectangle Geometry::get_xy(double angle, int distance) {

  return Rectangle((int) (distance * std::cos(angle)), (int) (-distance * std::sin(angle)));
}

/**
 * @brief Returns the cartesian coordinates of a vector, given its initial point, angle and distance.
 * @param xy1 coordinates of the first point
 * @param angle angle of the vector in radians
 * @param distance length of the vector in pixels
 * @return the coordinates of the second point
 */
const Rectangle Geometry::get_xy(const Rectangle &xy1, double angle, int distance) {

  Rectangle xy = get_xy(angle, distance);
  xy.add_xy(xy1.get_x(), xy1.get_y());
  return xy;
}

