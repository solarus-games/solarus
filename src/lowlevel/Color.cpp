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
/**
 * This module defines a type for the colors and provides some
 * color related functions.
 */
#include "lowlevel/Color.h"

Uint32 Color::black;
Uint32 Color::white;
Uint32 Color::red;

SDL_PixelFormat * Color::format;

/**
 * Initializes the color static fields.
 */
void Color::initialize(void) {
  format = SDL_GetVideoSurface()->format;

  black = SDL_MapRGB(format, 0, 0, 0);
  white = SDL_MapRGB(format, 255, 255, 255);
  red = SDL_MapRGB(format, 255, 0, 0);
}

/**
 * Returns a color given its RGB values.
 * @param r the red component (from 0 to 255)
 * @param g the green component (from 0 to 255)
 * @param b the blue component (from 0 to 255)
 */
Uint32 Color::create(int r, int g, int b) {
  return SDL_MapRGB(format, r, g, b);
}
