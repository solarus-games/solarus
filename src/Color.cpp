/**
 * This module defines a type for the colors and provides some
 * color related functions.
 */
#include "Color.h"

zsdx_color_t COLOR_BLACK;
zsdx_color_t COLOR_WHITE;

static SDL_PixelFormat *format;

/**
 * Initializes the global variables of colors.
 */
void color_init(void) {
  format = SDL_GetVideoSurface()->format;
  COLOR_BLACK = SDL_MapRGB(format, 0, 0, 0);
  COLOR_WHITE = SDL_MapRGB(format, 255, 255, 255);
}

/**
 * Returns a color given its RGB values.
 * @param r the red component (from 0 to 255)
 * @param g the green component (from 0 to 255)
 * @param b the blue component (from 0 to 255)
 */
zsdx_color_t get_color(int r, int g, int b) {
  return SDL_MapRGB(format, r, g, b);
}
