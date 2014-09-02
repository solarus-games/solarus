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

namespace solarus {

////////////////////////////////////////////////////////////
// Point methods

inline Point::Point()
  = default;

inline Point::Point(int x, int y):
  x(x),
  y(y)
{}

inline Point& Point::operator+=(const Point& other) {
  x += other.x;
  y += other.y;
  return *this;
}

inline Point& Point::operator-=(const Point& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

inline Point& Point::operator*=(int factor) {
  x *= factor;
  y *= factor;
  return *this;
}

inline Point& Point::operator/=(int divisor) {
  x /= divisor;
  y /= divisor;
  return *this;
}

////////////////////////////////////////////////////////////
// Free functions

inline bool operator==(const Point& lhs, const Point& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const Point& lhs, const Point& rhs) {
  return !(lhs == rhs);
}

inline bool operator<(const Point& lhs, const Point& rhs) {
  if (lhs.x < rhs.x) {
      return true;
  }
  if (rhs.x < lhs.x) {
      return false;
  }
  return lhs.y < rhs.y;
}

inline bool operator<=(const Point& lhs, const Point& rhs) {
  return !(rhs < lhs);
}

inline bool operator>(const Point& lhs, const Point& rhs) {
  return rhs < lhs;
}

inline bool operator>=(const Point& lhs, const Point& rhs) {
  return !(lhs < rhs);
}

inline Point operator+(const Point& point) {
  return point;
}

inline Point operator-(const Point& point) {
  return { -point.x, -point.y };
}

inline Point operator+(const Point& lhs, const Point& rhs) {
  return { lhs.x + rhs.x, lhs.y + rhs.y };
}

inline Point operator-(const Point& lhs, const Point& rhs) {
  return { lhs.x - rhs.x, lhs.y - rhs.y };
}

inline Point operator*(Point point, int factor) {
  return point *= factor;
}

inline Point operator*(int factor, Point point) {
  return point *= factor;
}

inline Point operator/(Point point, int divisor) {
  return point /= divisor;
}

/**
 * \brief Prints a point to an output stream.
 * \param stream The stream.
 * \param point A point.
 * \return The output stream.
 */
inline std::ostream& operator<<(std::ostream& stream, const Point& point) {
  stream << '(' << point.x << ',' << point.y << ')';
  return stream;
}

}
