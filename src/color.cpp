#include <SDL/SDL.h>
#include "color.h"

zsdx_color_t COLOR_BLACK;
zsdx_color_t COLOR_WHITE;

static SDL_PixelFormat *format;

void color_init(void) {
  format = SDL_GetVideoSurface()->format;
  COLOR_BLACK = SDL_MapRGB(format, 0, 0, 0);
  COLOR_WHITE = SDL_MapRGB(format, 255, 255, 255);
}

zsdx_color_t get_color(int r, int g, int b) {
  return SDL_MapRGB(format, r, g, b);
}
