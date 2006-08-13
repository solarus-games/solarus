#ifndef ZSDX_MAP_H
#define ZSDX_MAP_H

#include "color.h"
#include "map_object.h"
#include "dynamic_array.h"
#include "tileset.h"

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

  // fields
 protected:
  const int width; // map width in pixel
  const int height; // map height in pixel
  const int width8; // width / 8
  const int height8; // height / 8
  const zsdx_color_t background_color;
  Tileset *tileset;

 private:
  DynamicArray<MapObject*> *objects;
  int obstacle_tiles_size;
  tile_obstacle_t *obstacle_tiles;

  // methods
 private:
  void display(SDL_Surface *surface);  

 protected:
  inline void add_object(MapObject *object) { objects->add(object); }
  void add_new_tile(TileData *tile_data, SDL_Rect &where_in_map);

 public:
  Map(int width, int height, zsdx_color_t background_color,
      Tileset *tileset);
  virtual ~Map();

  virtual void load(void) = 0;
  void unload(void);
  void launch(void);
};

#endif
