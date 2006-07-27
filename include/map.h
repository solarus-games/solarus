#ifndef ZSDX_MAP_H
#define ZSDX_MAP_H

#include "map_object.h"
#include "dynamic_array.h"

/* A map is represented by:
 * - a background color
 * - some tiles
 * - enemies and characters
 * We need the following types of tiles:
 * - single tile
 * - extensible tile
 * - animated tile
 * The coordinates cannot be stored in class Tile because tiles can be shared on the map.
 * Handle oblique obstacles.
 * 2 layers of tiles? or even more? make layer handling global (with swags? or is the layer of an object constant?)
 */
class Map {

 private:
  int width; // map width in pixel
  int height; // map height in pixel
  zsdx_color_t background_color;

  DynamicArray<MapObject*> *objects;

 public:
  Map(int width, int height, zsdx_color_t background_color);
  ~Map();

  void display(SDL_Surface *surface);
};

#endif
