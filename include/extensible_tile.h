#ifndef ZSDX_EXTENSIBLE_TILE_H
#define ZSDX_EXTENSIBLE_TILE_H

#include <SDL/SDL.h>
#include "datatypes.h"
#include "tile_data.h"

/* Tile pattern that can be extended
 */
class ExtensibleTile: public TileData {

 private:
  /* Position of the tile in the tileset image
   */
  SDL_Rect where_in_tileset;

  /* How many times the pattern is repeated in x
   */
  const int repeat_x;

  /* How many times the pattern is repeated in y
   */
  const int repeat_y;

 public:
  /* Create an extensible tile with a new pattern
   */
  ExtensibleTile(SDL_Rect &where_in_tileset,
		 tile_obstacle_t obstacle);

  /* Create an extensible tile with the same pattern as another one
   */
  ExtensibleTile(ExtensibleTile *other, int repeat_x, int repeat_y);

  /* Destructor
   */
  inline ~ExtensibleTile(void) { }

  /* Display the tile image on the map
   */
  void display_on_map(Map *map, SDL_Rect &where_in_map);
};

#endif
