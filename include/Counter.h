/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 *
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_COUNTER_H
#define ZSDX_COUNTER_H

#include "Common.h"

/**
 * This class displays a counter on a surface.
 * The counter can only display positive integer numbers.
 */
class Counter {

 public:

   /**
    * Style of digits to display.
    */
  enum Style {
    BIG_DIGITS,   /**< 8*8 pixels (default)*/
    SMALL_DIGITS, /**< 6*7 pixels */
  };

 private:

  Style style;
  unsigned int nb_digits;        /**< number of digits to use to display the value */
  bool fill_with_zeros;          /**< indicates to fill the counter with zeros when the number of digits is too low */
  unsigned int maximum;          /**< the counter is shown with a special color when this value is reached (0 indicates that there is no maximum) */
  unsigned int value;

  SDL_Surface *surface_drawn;
  SDL_Rect destination_position;
  SDL_Surface *img_digits;

  void rebuild_with_value(unsigned int value);

 public:

  Counter(unsigned int nb_digits, bool fill_with_zeros, int x, int y);
  ~Counter(void);

  void set_style(Style style);
  void set_maximum(unsigned int maximum);
  unsigned int get_value(void);
  void set_value(unsigned int value);
  void increase(void);
  void decrease(void);

  void display(SDL_Surface *destination);
  void display(SDL_Surface *destination, int x, int y);
};

#endif

