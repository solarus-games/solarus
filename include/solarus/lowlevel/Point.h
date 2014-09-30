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
#ifndef SOLARUS_POINT_H
#define SOLARUS_POINT_H

#include "solarus/Common.h"
#include <iosfwd>

namespace Solarus {

/**
 * \brief Represents a 2D point.
 *
 * This class represents a point with its two integer coordinates, x and y.
 */
class Point {

  public:

    ////////////////////////////////////////////////////////////
    // Constructors

    constexpr Point();
    constexpr Point(int x, int y);

    ////////////////////////////////////////////////////////////
    // Operators

    Point& operator+=(const Point& other);
    Point& operator-=(const Point& other);

    Point& operator*=(int factor);
    Point& operator/=(int divisor);

    ////////////////////////////////////////////////////////////
    // Member data

    int x = 0;    /**< First coordinate */
    int y = 0;    /**< Second coordinate */

};

////////////////////////////////////////////////////////////
// Free functions

// Comparison operators
bool operator==(const Point& lhs, const Point& rhs);
bool operator!=(const Point& lhs, const Point& rhs);

// Unary arithmetic operators
Point operator+(const Point& point);
Point operator-(const Point& point);

// Binary arithmetic operators
Point operator+(Point lhs, const Point& rhs);
Point operator-(Point lhs, const Point& rhs);

Point operator*(Point point, int factor);
Point operator*(int factor, Point point);
Point operator/(Point point, int divisor);

// Output to stream
std::ostream& operator<<(std::ostream& stream, const Point& point);

}

#include "solarus/lowlevel/Point.inl"

#endif

