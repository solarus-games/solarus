#ifndef ZSDX_SIMPLE_TILE_H
#define ZSDX_SIMPLE_TILE_H

#include <SDL/SDL.h>
#include "tile_data.h"

/* Simple tile image, static and fixed
 */
class SimpleTile: public TileData {

 protected:
  /* Position of the tile in the tileset image
   */
  SDL_Rect where_in_tileset;

 public:
  /* Constructor
   */
  SimpleTile(SDL_Rect &where_in_tileset, tile_obstacle_t obstacle);

  /* Destructor
   */
  inline ~SimpleTile(void) { }

  /* Display the tile image on the map
   */
  void display_on_map(Map *map, SDL_Rect &where_in_map);
};

#endif
