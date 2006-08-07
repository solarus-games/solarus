#include <SDL/SDL.h>
#include "color.h"
#include "global.h"

zsdx_color_t COLOR_BLACK;
zsdx_color_t COLOR_WHITE;

void color_init(void) {
  SDL_PixelFormat *format = zsdx_global.screen->format;
  COLOR_BLACK = SDL_MapRGB(format, 0, 0, 0);
  COLOR_WHITE = SDL_MapRGB(format, 255, 255, 255);
}
