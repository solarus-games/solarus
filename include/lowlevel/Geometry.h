/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_GEOMETRY_H
#define SOLARUS_GEOMETRY_H

#include "Common.h"
#include "lowlevel/Point.h"
#include <cmath>
#include <cstdlib>

namespace solarus {

/**
 * \brief Provides functions to make mathematic computations.
 */
class Geometry {

  public:

    // Mathematic constants:
    // we don't use the ones from cmath
    // because they are not ANSI
    static const double PI;
    static const double TWO_PI;
    static const double PI_OVER_2;
    static const double PI_OVER_4;
    static const double THREE_PI_OVER_2;
    static const double SQRT_2;

    static int radians_to_degrees(double radians);
    static double degrees_to_radians(double degrees);

    static double get_distance(int x1, int y1, int x2, int y2);
    static int get_distance2(int x1, int y1, int x2, int y2);
    static double get_distance(const Point& point1, const Point& point2);
    static int get_distance2(const Point& point1, const Point& point2);
    static double get_angle(int x1, int y1, int x2, int y2);
    static double get_angle(const Point& point1, const Point& point2);
    static const Point get_xy(double angle, int distance);
    static const Point get_xy(const Point& point1, double angle, int distance);

};

/**
 * \brief Returns the distance between two points.
 * \param x1 X coordinate of the first point.
 * \param y1 Y coordinate of the first point.
 * \param x2 X coordinate of the second point.
 * \param y2 Y coordinate of the second point.
 * \return The distance in pixels
 */
inline double Geometry::get_distance(int x1, int y1, int x2, int y2) {

  const int dx = x2 - x1;
  const int dy = y2 - y1;
  return std::sqrt((double) (dx * dx + dy * dy));
}

/**
 * \brief Returns the square of the distance between two points.
 * \param x1 X coordinate of the first point.
 * \param y1 Y coordinate of the first point.
 * \param x2 X coordinate of the second point.
 * \param y2 Y coordinate of the second point.
 * \return Square of the distance.
 */
inline int Geometry::get_distance2(int x1, int y1, int x2, int y2) {

  const int dx = x2 - x1;
  const int dy = y2 - y1;
  return dx * dx + dy * dy;
}

/**
 * \brief Returns the distance between two points.
 * \param point1 First point.
 * \param point2 Second point.
 * \return The distance in pixels.
 */
inline double Geometry::get_distance(const Point& point1, const Point& point2) {

  return get_distance(point1.x, point1.y, point2.x, point2.y);
}

/**
 * \brief Returns the square of the distance between two points.
 * \param point1 The first point.
 * \param point2 The second point.
 * \return The distance in pixels
 */
inline int Geometry::get_distance2(const Point& point1, const Point& point2) {

  return get_distance2(point1.x, point1.y, point2.x, point2.y);
}

}

#endif

