/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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

namespace Solarus {

////////////////////////////////////////////////////////////
// Point methods

/**
 * \brief Creates a point with coordinates <tt>(0,0)</tt>.
 */
constexpr Point::Point()
  = default;

/**
 * \brief Creates a point with the specified coordinates.
 * \param x The x coordinate.
 * \param y The y coordinate.
 */
constexpr Point::Point(int x, int y):
  x(x),
  y(y) {
}

/**
 * \brief Adds another point to this one.
 * \param other Another point.
 * \return The current point after modification.
 */
inline Point& Point::operator+=(const Point& other) {
  x += other.x;
  y += other.y;
  return *this;
}

/**
 * \brief Subtracts another point from this one.
 * \param other Another point.
 * \return The current point after modification.
 */
inline Point& Point::operator-=(const Point& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

/**
 * \brief Multiplies this point by a value.
 * \param factor The multiplication factor.
 * \return The current point after modification.
 */
inline Point& Point::operator*=(int factor) {
  x *= factor;
  y *= factor;
  return *this;
}

/**
 * \brief Multiplies this point by a value.
 * \param divisor The divisor.
 * \return The current point after modification.
 */
inline Point& Point::operator/=(int divisor) {
  x /= divisor;
  y /= divisor;
  return *this;
}

////////////////////////////////////////////////////////////
// Free functions

/**
 * \brief Returns whether two points are equal.
 * \param lhs A point.
 * \param rhs Another point.
 * \return \c true if the points are equal.
 */
constexpr bool operator==(const Point& lhs, const Point& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

/**
 * \brief Returns whether two points are different.
 * \param lhs A point.
 * \param rhs Another point.
 * \return \c true if the points are not equal.
 */
constexpr bool operator!=(const Point& lhs, const Point& rhs) {
  return !(lhs == rhs);
}

/**
 * \brief Unary plus operator.
 * \param point A point.
 * \return A copy of the point.
 */
constexpr Point operator+(const Point& point) {
  return point;
}

/**
 * \brief Unary minus operator.
 * \param point A point.
 * \return A point with opposite coordinates.
 */
constexpr Point operator-(const Point& point) {
  return { -point.x, -point.y };
}

/**
 * \brief Adds two points.
 * \param lhs A point.
 * \param rhs Another point.
 * \return A point with the sum of their coordinates.
 */
constexpr Point operator+(const Point& lhs, const Point& rhs) {
  return { lhs.x + rhs.x, lhs.y + rhs.y };
}

/**
 * \brief Subtracts two points.
 * \param lhs A point.
 * \param rhs The point to subtract.
 * \return A point with the difference of their coordinates.
 */
constexpr Point operator-(const Point& lhs, const Point& rhs) {
  return { lhs.x - rhs.x, lhs.y - rhs.y };
}

/**
 * \brief Multiples a point by a number.
 * \param point A point.
 * \param factor The factor.
 * \return A point with coordinates multiplied by the factor.
 */
constexpr Point operator*(const Point& point, int factor) {
  return { point.x * factor, point.y * factor };
}

/**
 * \brief Multiples a point by a number.
 * \param factor The factor.
 * \param point A point.
 * \return A point with coordinates multiplied by the factor.
 */
constexpr Point operator*(int factor, const Point& point) {
  return point * factor;
}

/**
 * \brief Divides a point by a number.
 * \param point A point.
 * \param divisor The divisor.
 * \return A point with coordinates divided by the divisor.
 */
constexpr Point operator/(const Point& point, int divisor) {
  return { point.x / divisor, point.y / divisor };
}

}
