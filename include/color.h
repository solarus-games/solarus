#ifndef ZSDX_COLOR_H
#define ZSDX_COLOR_H

#include <SDL/SDL.h>

/* Our own color type
 */
typedef Uint32 zsdx_color_t;

/* Black
 */
extern zsdx_color_t COLOR_BLACK;

/* White
 */
extern zsdx_color_t COLOR_WHITE;

/* Initializes the global variables of colors
 */
void color_init(void);

#endif
