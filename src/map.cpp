#include "zsdx.h"
#include "map.h"

Map::Map(int width, int height, zsdx_color_t background_color) {
  this->width = width;
  this->height = height;
  this->background_color = background_color;
}

void Map::display(SDL_Surface *surface) {
  // for now we don't care about the scrolling
  
}
