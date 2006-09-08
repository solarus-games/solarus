#ifndef ZSDX_TILE_DATA_H
#define ZSDX_TILE_DATA_H

#include <SDL/SDL.h>
#include "datatypes.h"

/* Abstract class for a tile image
 */
class TileData {
  
 protected:
  /* Is the tile an obstacle?
   */
  const tile_obstacle_t obstacle;

  /* Tile width
   */
  const int width;

  /* Tile height
   */
  const int height;

 public:
  /* Constructor
   */
  inline TileData(tile_obstacle_t obstacle, int width, int height):
    obstacle(obstacle), width(width), height(height) { }

  /* Destructor
   */
  inline virtual ~TileData(void) { };

  /* Return the tile width
   */
  inline int get_width(void) const { return width; }

  /* Return the tile height
   */
  inline int get_height(void) const { return height; }
  
  /* Return the obstacle property of the tile
   */
  inline tile_obstacle_t get_obstacle(void) const { return obstacle; }

  /* Display the tile image on the map
   */
  virtual void display_on_map(Map *map, SDL_Rect &where_in_map) = 0;
};

#endif
