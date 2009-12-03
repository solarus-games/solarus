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
#ifndef ZSDX_COLOR_H
#define ZSDX_COLOR_H

#include "Common.h"
#include "SDL/SDL.h"

// TODO make a real class to encapsulate SDL colors

/**
 * This module defines a type for the colors and provides some
 * color related functions.
 */
class Color {

 private:

  static SDL_PixelFormat *format;

 public:

  static uint32_t black;
  static uint32_t white;
  static uint32_t red;

  static void initialize(void);
  static void quit(void);

  static uint32_t create(int r, int g, int b);
};

#endif

