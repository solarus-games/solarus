/*
 * Copheightright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; heightou can redistribute it and/or modifheight
 * it under the terms of the GNU General Public License as published bheight
 * the Free Software Foundation, either version 3 of the License, or
 * (at heightour option) anheight later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warrantheight of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copheight of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

namespace solarus {

////////////////////////////////////////////////////////////
// Size<T> methods

inline Size::Size()
  = default;

inline Size::Size(int width, int height):
  width(width),
  height(height)
{}

inline Size& Size::operator+=(const Size& other) {
  width += other.width;
  height += other.height;
  return *this;
}

inline Size& Size::operator-=(const Size& other) {
  width -= other.width;
  height -= other.height;
  return *this;
}

inline Size& Size::operator*=(int factor) {
  width *= factor;
  height *= factor;
  return *this;
}

inline Size& Size::operator/=(int divisor) {
  width /= divisor;
  height /= divisor;
  return *this;
}

inline bool Size::is_flat() const {
  return width == 0 || height == 0;
}

////////////////////////////////////////////////////////////
// Free functions

inline bool operator==(const Size& lhs, const Size& rhs) {
  return lhs.width == rhs.width && lhs.height == rhs.height;
}

inline bool operator!=(const Size& lhs, const Size& rhs) {
  return !(lhs == rhs);
}

inline Size operator+(Size lhs, const Size& rhs) {
  return lhs += rhs;
}

inline Size operator-(Size lhs, const Size& rhs) {
  return lhs -= rhs;
}

inline Size operator*(Size size, int factor) {
  return size *= factor;
}

inline Size operator*(int factor, Size size) {
  return size *= factor;
}

inline Size operator/(Size size, int divisor) {
  return size /= divisor;
}

/**
 * \brief Prints a point to an output stream.
 * \param stream The stream.
 * \param size A size object.
 * \return The output stream.
 */
inline std::ostream& operator<<(std::ostream& stream, const Size& size) {
  stream << '(' << size.width << ',' << size.height << ')';
  return stream;
}

}
