/**
 * This module defines a type for the colors and provides some
 * color related functions.
 */

#ifndef ZSDX_COLOR_H
#define ZSDX_COLOR_H

#include "Common.h"

/**
 * Color type.
 */
typedef Uint32 zsdx_color_t;

/**
 * Black.
 */
extern zsdx_color_t COLOR_BLACK;

/**
 * White.
 */
extern zsdx_color_t COLOR_WHITE;

/**
 * Initializes the global variables of colors.
 */
void color_init(void);

/**
 * Returns a color given its RGB values.
 * @param r the red component (from 0 to 255)
 * @param g the green component (from 0 to 255)
 * @param b the blue component (from 0 to 255)
 */
zsdx_color_t get_color(int r, int g, int b);

#endif
