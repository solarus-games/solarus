#ifndef ZSDX_TILESET_H
#define ZSDX_TILESET_H

#include "Common.h"
#include "Color.h"

/**
 * A tileset is an image with a set of elements (tile patterns)
 * one can use to compose a map. See the directory images/tilesets.
 */
class Tileset {

 private:

  TilesetId id;                     /**< id of the tileset */
  TilePattern *tile_patterns[1024]; /**< tile patterns in this tileset (the maximum id is 1024) */
  int nb_tile_patterns;             /**< number of tile patterns in the tileset */
  int max_tile_id;                  /**< current maximum id of a tile pattern in this tileset */
  Uint32 background_color;          /**< background color of the tileset */
  SDL_Surface *tileset_image;       /**< image from which the tile patterns are extracted */

  void add_tile_pattern(int id, TilePattern *tile_pattern);

 public:

  Tileset(TilesetId id);
  ~Tileset(void);

  void load(void);
  void unload(void);

  Uint32 get_background_color(void);
  bool is_loaded(void);
  SDL_Surface *get_image(void);
  TilePattern *get_tile_pattern(int id);
};

#endif
