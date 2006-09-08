#ifndef ZSDX_COLOR_H
#define ZSDX_COLOR_H

#include <SDL/SDL.h>
#include "datatypes.h"

/* Black
 */
extern zsdx_color_t COLOR_BLACK;

/* White
 */
extern zsdx_color_t COLOR_WHITE;

/* Initializes the global variables of colors
 */
void color_init(void);

/* Return a color given its RGB values
 */
zsdx_color_t get_color(int r, int g, int b);

#endif
