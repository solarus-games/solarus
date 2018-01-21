/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Geometry.h"

namespace Solarus {
namespace Geometry {

/**
 * \brief Converts an angle in radians into an angle in degrees.
 * \param radians Angle in radians.
 * \return The degrees value.
 */
int radians_to_degrees(double radians) {
  return (int) (radians * 360.0 / TWO_PI);
}

/**
 * \brief Converts an angle in degrees into an angle in radians.
 * \param degrees Angle in degrees.
 * \return The radians value.
 */
double degrees_to_radians(double degrees) {
  return degrees * TWO_PI / 360.0;
}

/**
 * \brief Returns the angle of the vector formed by two points.
 * \param x1 X coordinate of the first point.
 * \param y1 Y coordinate of the first point.
 * \param x2 X coordinate of the second point.
 * \param y2 Y coordinate of the second point.
 * \return The angle in radians, between 0 and TWO_PI.
 */
double get_angle(double x1, double y1, double x2, double y2) {

  double dx = x2 - x1;
  double dy = y2 - y1;

  // atan2 is undefined if (y == 0 and x == 0)
  if (dx == 0.0 && dy == 0.0) {
    return PI_OVER_2;
  }

  double angle = std::atan2(-dy, dx);

  // Normalize.
  if (angle < 0.0) {
    angle += TWO_PI;
  }

  return angle;
}

/**
 * \brief Returns the angle of the vector formed by two points.
 * \param point1 First point.
 * \param point2 Second point.
 * \return The angle in radians, between 0 and TWO_PI.
 */
double get_angle(const Point& point1, const Point& point2) {
  return get_angle(point1.x, point1.y, point2.x, point2.y);
}

/**
 * \brief Returns the cartesian coordinates of a vector that starts from the origin, given its angle and distance.
 * \param angle Angle of the vector in radians.
 * \param distance Length of the vector in pixels.
 * \return The coordinates of the second point.
 */
Point get_xy(double angle, int distance) {

  return {
      static_cast<int>(distance * std::cos(angle)),
      static_cast<int>(-distance * std::sin(angle))
  };
}

/**
 * \brief Returns the cartesian coordinates of a vector, given its initial point, angle and distance.
 * \param point1 Coordinates of the first point.
 * \param angle Angle of the vector in radians.
 * \param distance Length of the vector in pixels.
 * \return The coordinates of the second point.
 */
Point get_xy(const Point& point1, double angle, int distance) {
  return point1 + get_xy(angle, distance);
}

}
}
