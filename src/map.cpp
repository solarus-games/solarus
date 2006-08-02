#include "datatypes.h"
#include "map.h"
#include "map_object.h"

Map::Map(int width, int height, zsdx_color_t background_color) {
  this->width = width;
  this->height = height;
  this->background_color = background_color;
  this->objects = new DynamicArray<MapObject*>(256);
}

Map::~Map() {
  delete objects;
}

void Map::display(SDL_Surface *surface) {
  // for now we don't care about the scrolling

  // background color
  SDL_FillRect(surface, NULL, background_color);

  // map objects
  for (int i = 0; i < objects->get_size(); i++) {
    objects->get(i)->display_on_map(surface);
  }
}
