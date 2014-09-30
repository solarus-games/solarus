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
/**
 * This module defines a type for the colors and provides some
 * color related functions.
 */
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Video.h"

namespace Solarus {

Color Color::transparent;
Color Color::black;
Color Color::white;
Color Color::red;
Color Color::green;
Color Color::blue;
Color Color::yellow;
Color Color::magenta;
Color Color::cyan;

/**
 * \brief Initializes the color static fields.
 */
void Color::initialize() {

  transparent = Color(  0,   0,   0,   0);
  black =       Color(  0,   0,   0);
  white =       Color(255, 255, 255);
  red =         Color(255,   0,   0);
  green =       Color(  0, 255,   0);
  blue =        Color(  0,   0, 255);
  yellow =      Color(255, 255,   0);
  magenta =     Color(255,   0, 255);
  cyan =        Color(  0, 255, 255);
}

/**
 * \brief Uninitializes the color system.
 */
void Color::quit() {
}

/**
 * \brief Creates a transparent color with black RGB values.
 */
Color::Color() {
  internal_color.r = 0;
  internal_color.g = 0;
  internal_color.b = 0;
  internal_color.a = 0;
  internal_value = 0;
}

/**
 * \brief Creates a color with the specified RGBA values.
 * \param r the red component (from 0 to 255)
 * \param g the green component (from 0 to 255)
 * \param b the blue component (from 0 to 255)
 * \param a the alpha component (from 0 to 255)
 */
Color::Color(int r, int g, int b, int a) {
  internal_color.r = r;
  internal_color.g = g;
  internal_color.b = b;
  internal_color.a = a;

  internal_value = SDL_MapRGBA(Video::get_pixel_format(), r, g, b, a);
}

/**
 * \brief Returns the 32-bit value representing this color.
 *
 * This function must be used only by low-level classes.
 *
 * \return The 32-bit value of this color, in the preferred pixel format
 * (Video::get_pixel_format()).
 */
uint32_t Color::get_internal_value() const {
  return internal_value;
}

/**
 * \brief Returns the internal color encapsulated by this object.
 *
 * This function must be used only by low-level classes.
 *
 * \return the SDL color encapsulated
 */
SDL_Color* Color::get_internal_color() {
  return &internal_color;
}

/**
 * \brief Returns the red, green, blue and alpha values of this color.
 * \param r Red component to write.
 * \param g Green component to write.
 * \param b Blue component to write.
 * \param a Alpha component to write.
 */
void Color::get_components(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const {

  r = internal_color.r;
  g = internal_color.g;
  b = internal_color.b;
  a = internal_color.a;
}

/**
 * \brief Equality operator.
 * \param lhs A color.
 * \param rhs Another color.
 * \return \c true if both colors are equal.
 */
bool operator==(const Color& lhs, const Color& rhs) {
  return lhs.get_internal_value() == rhs.get_internal_value();
}

}

