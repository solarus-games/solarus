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

namespace Solarus {

////////////////////////////////////////////////////////////
// Size methods

/**
 * \brief Creates the size <tt>(0,0)</tt>.
 */
constexpr Size::Size()
  = default;

/**
 * \brief Creates a size with the specified dimensions.
 * \param width The width.
 * \param height The height.
 */
constexpr Size::Size(int width, int height):
  width(width),
  height(height) {
}

/**
 * \brief Adds another size to this one.
 * \param other Another size.
 * \return The current size after modification.
 */
inline Size& Size::operator+=(const Size& other) {
  width += other.width;
  height += other.height;
  return *this;
}

/**
 * \brief Subtracts another size from this one.
 * \param other The size to subtract.
 * \return The current size after modification.
 */
inline Size& Size::operator-=(const Size& other) {
  width -= other.width;
  height -= other.height;
  return *this;
}

/**
 * \brief Multiplies this size by a value.
 * \param factor The multiplication factor.
 * \return The current size after modification.
 */
inline Size& Size::operator*=(int factor) {
  width *= factor;
  height *= factor;
  return *this;
}

/**
 * \brief Divides this size by a value.
 * \param divisor The divisor.
 * \return The current size after modification.
 */
inline Size& Size::operator/=(int divisor) {
  width /= divisor;
  height /= divisor;
  return *this;
}

/**
 * \brief Returns whether this size is flat.
 * \return \c true if the width or the height are zero.
 */
constexpr bool Size::is_flat() const {
  return width == 0 || height == 0;
}

/**
 * \brief Returns whether this width and height are the same.
 * \return \c true if the width and height are equal.
 */
constexpr bool Size::is_square() const {
  return width == height;
}

////////////////////////////////////////////////////////////
// Free functions

/**
 * \brief Returns whether two sizes are equal.
 * \param lhs A size.
 * \param rhs Another size.
 * \return \c true if the sizes are equal.
 */
inline bool operator==(const Size& lhs, const Size& rhs) {
  return lhs.width == rhs.width && lhs.height == rhs.height;
}

/**
 * \brief Returns whether two sizes are different.
 * \param lhs A size.
 * \param rhs Another size.
 * \return \c true if the sizes are not equal.
 */
inline bool operator!=(const Size& lhs, const Size& rhs) {
  return !(lhs == rhs);
}

/**
 * \brief Adds two sizes.
 * \param lhs A size.
 * \param rhs Another size.
 * \return The sum of both sizes.
 */
inline Size operator+(Size lhs, const Size& rhs) {
  return lhs += rhs;
}

/**
 * \brief Substracts two sizes.
 * \param lhs A size.
 * \param rhs The size to subtract.
 * \return The difference of both sizes.
 */
inline Size operator-(Size lhs, const Size& rhs) {
  return lhs -= rhs;
}

/**
 * \brief Multiplies a size by a value.
 * \param size A size.
 * \param factor The multiplication factor.
 * \return A size with width and height multiplied by the factor.
 */
inline Size operator*(Size size, int factor) {
  return size *= factor;
}

/**
 * \brief Multiplies a size by a value.
 * \param factor The multiplication factor.
 * \param size A size.
 * \return A size with width and height multiplied by the factor.
 */
inline Size operator*(int factor, Size size) {
  return size *= factor;
}

/**
 * \brief Divides a size by a value.
 * \param size A size.
 * \param divisor The divisor.
 * \return A size with width and height divided by the divisor.
 */
inline Size operator/(Size size, int divisor) {
  return size /= divisor;
}

}
