/**
 * This module defines a type for the colors and provides some
 * color related functions.
 */
#include "Color.h"

Uint32 Color::black;
Uint32 Color::white;

SDL_PixelFormat * Color::format;

/**
 * Initializes the color static fields.
 */
void Color::initialize(void) {
  format = SDL_GetVideoSurface()->format;

  black = SDL_MapRGB(format, 0, 0, 0);
  white = SDL_MapRGB(format, 255, 255, 255);
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
