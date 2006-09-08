#ifndef ZSDX_TILE_H
#define ZSDX_TILE_H

#include "map_entity.h"
#include "tile_data.h"

/* A tile is a small piece of map with a image of size 8*8 or more. 
 */
class Tile: public MapEntity {

 private:
  /* Image of the tile
   * Can be SimpleTile, ExtensibleTile or AnimatedTile
   */
  TileData *tile_data;

 public:
  /* Constructor
   */
  Tile(TileData *tile_data, SDL_Rect &where_in_map);

  /* Destructor
   */
  inline ~Tile() { }

  /* Display the tile on the map
   */
  inline void display_on_map(Map *map) { tile_data->display_on_map(map, where_in_map); }
};

#endif
