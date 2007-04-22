#include <SDL/SDL.h>
#include "color.h"
#include "zsdx.h"

zsdx_color_t COLOR_BLACK;
zsdx_color_t COLOR_WHITE;

static SDL_PixelFormat *format;

void color_init(void) {
  format = ZSDX::get_screen()->format;
  COLOR_BLACK = SDL_MapRGB(format, 0, 0, 0);
  COLOR_WHITE = SDL_MapRGB(format, 255, 255, 255);
}

zsdx_color_t get_color(int r, int g, int b) {
  return SDL_MapRGB(format, r, g, b);
}
