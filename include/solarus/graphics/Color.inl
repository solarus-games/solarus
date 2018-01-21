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

namespace Solarus {

////////////////////////////////////////////////////////////
// Point methods

/**
 * \brief Creates a transparent color with black RGB values.
 */
constexpr Color::Color():
    r(0),
    g(0),
    b(0),
    a(0) {
}

/**
 * \brief Creates a color with the specified RGBA values.
 * \param r The red component (0 to 255)
 * \param g The green component (0 to 255)
 * \param b The blue component (0 to 255)
 * \param a The alpha component (0 to 255, 255 is opaque).
 */
constexpr Color::Color(int r, int g, int b, int a):
    r(r),
    g(g),
    b(b),
    a(a) {
}

/**
 * \brief Returns the alpha component of this color.
 * \return The alpha component. 255 is opaque.
 */
constexpr uint8_t Color::get_alpha() const {
  return a;
}

/**
 * \brief Sets the alpha component of this color.
 * \param alpha The alpha component. 255 is opaque.
 */
inline void Color::set_alpha(uint8_t alpha) {
  this->a = alpha;
}

/**
 * \brief Gets the red, green, blue and alpha values of this color.
 * \param[out] r Red component.
 * \param[out] g Green component.
 * \param[out] b Blue component.
 * \param[out] a Alpha component.
 */
inline void Color::get_components(
    uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const {

  r = this->r;
  g = this->g;
  b = this->b;
  a = this->a;
}

/**
 * \brief Sets the red, green, blue and alpha values of this color.
 * \param r Red component to write.
 * \param g Green component to write.
 * \param b Blue component to write.
 * \param a Alpha component to write.
 */
inline void Color::set_components(int r, int g, int b, int a) {

  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;
}

/**
 * \brief Equality operator.
 * \param lhs A color.
 * \param rhs Another color.
 * \return \c true if both colors are equal.
 */
constexpr bool operator==(const Color& lhs, const Color& rhs) {

  return lhs.r == rhs.r &&
      lhs.g == rhs.g &&
      lhs.b == rhs.b &&
      lhs.a == rhs.a;
}

/**
 * \brief Compare two colors.
 * \param lhs A color.
 * \param rhs Another color.
 * \return \c true if both colors are equal.
 */
constexpr bool operator!=(const Color& lhs, const Color& rhs) {

  return lhs.r != rhs.r ||
      lhs.g != rhs.g ||
      lhs.b != rhs.b ||
      lhs.a != rhs.a;
}

}
