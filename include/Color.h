#ifndef ZSDX_COLOR_H
#define ZSDX_COLOR_H

#include "Common.h"

/**
 * This module defines a type for the colors and provides some
 * color related functions.
 */
class Color {

 private:

  static SDL_PixelFormat *format;

 public:

  static Uint32 black;
  static Uint32 white;

  /**
   * Initializes the global variables of colors.
   */
  static void initialize(void);

  /**
   * Returns a color given its RGB values.
   * @param r the red component (from 0 to 255)
   * @param g the green component (from 0 to 255)
   * @param b the blue component (from 0 to 255)
   */
  static Uint32 create(int r, int g, int b);
};

#endif
