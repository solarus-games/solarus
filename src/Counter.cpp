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
#include "Counter.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"

/**
 * @brief Constructor.
 *
 * The counter's value is initialized to zero.
 *
 * @param nb_digits number of digits to display
 * @param fill_with_zeros true to fill with zeros when the current number of digits is lower than nb_digits
 * @param x x coordinate of the top-left corner of the counter on the destination surface
 * @param y y coordinate of the top-left corner of the counter on the destination surface
 */
Counter::Counter(unsigned int nb_digits, bool fill_with_zeros,
			 int x, int y):
  style(BIG_DIGITS), nb_digits(nb_digits), fill_with_zeros(fill_with_zeros), maximum(0) {

  surface_drawn = new Surface(8 * nb_digits, 8);
  surface_drawn->set_transparency_color(Color::get_black());
  img_digits = new Surface("hud/digits.png");

  destination_position.set_xy(x, y);

  rebuild_with_value(0);
}

/**
 * @brief Destructor.
 */
Counter::~Counter() {
  delete surface_drawn;
  delete img_digits;
}

/**
 * @brief Sets the style of digits to display.
 * @param style the style to set
 */
void Counter::set_style(Style style) {

  if (style != this->style) {
    this->style = style;
    rebuild_with_value(get_value());
  }
}

/**
 * @brief Changes the counter's maximum value.
 *
 * The surface is redrawn if necessary.
 * Nothing is done if the maximum value is unchanged.
 *
 * @param maximum the new maximum value to set (0 to set no maximum)
 */
void Counter::set_maximum(unsigned int maximum) {

  if (maximum != this->maximum) {
    this->maximum = maximum;
    rebuild_with_value(get_value());
  }
}

/**
 * @brief Returns the value currently displayed by the counter.
 * @return the value currently displayed by the counter
 */
unsigned int Counter::get_value() {
  return value;
}

/**
 * @brief Changes the counter's value.
 *
 * The surface is redrawn such that the counter displays
 * the new value.
 * Nothing is done if the value remains the same.
 *
 * @param value the new value to display
 */
void Counter::set_value(unsigned int value) {

  if (value != this->value) {
    rebuild_with_value(value);
  }
}

/**
 * @brief Redraws the surface to represent a new value.
 * @param value the new value to display
 */
void Counter::rebuild_with_value(unsigned int value) {

  this->value = value;

  // fill with transparent color
  surface_drawn->fill_with_color(Color::get_black());

  int y, width;
  if (style == BIG_DIGITS) {
    y = 0;
    width = 7;
  }
  else {
    y = 16;
    width = 5;
  }

  if (value != 0 && value == maximum) {
    y += 8;
  }

  Rectangle digit_position_in_src(0, y, 8, 8);
  Rectangle digit_position_in_counter(0, 0, 8, 8);

  bool right_digit = true;

  // draw the digit from the right to the left
  for (int i = nb_digits - 1; i >= 0; i--) {

    // compute each digit
    uint8_t digit = value % 10;
    value /= 10;

    if (value != 0 || digit != 0 || fill_with_zeros || right_digit) {

      // draw the surface
      digit_position_in_src.set_x(digit * 8);
      digit_position_in_counter.set_x(i * width);

      img_digits->blit(digit_position_in_src, surface_drawn, digit_position_in_counter);

      right_digit = false;
    }
  }
}

/**
 * @brief Adds 1 to the counter.
 *
 * The surface is redrawn such that the counter displays
 * the new value.
 */
void Counter::increase() {
  set_value(value + 1);
}

/**
 * @brief Substracts 1 from the counter.
 *
 * The surface is redrawn such that the counter displays
 * the new value.
 */
void Counter::decrease() {
  set_value(value - 1);
}

/**
 * @brief Displays the counter on a surface.
 *
 * The counter is displayed at the position specified when the constructor was called.
 *
 * @param destination the destination surface
 */
void Counter::display(Surface *destination) {
  surface_drawn->blit(destination, destination_position);
}

/**
 * @brief Displays the counter on a surface, specifying the position.
 * @param destination the destination surface
 * @param x x coordinate of the top-left corner of the counter on the destination surface
 * @param y y coordinate of the top-left corner of the counter on the destination surface
 */
void Counter::display(Surface *destination, int x, int y) {
  destination_position.set_xy(x, y);
  surface_drawn->blit(destination, destination_position);
}

