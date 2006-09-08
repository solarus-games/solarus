#ifndef ZSDX_ANIMATED_TILE_H
#define ZSDX_ANIMATED_TILE_H

#include <SDL/SDL.h>
#include "tile_data.h"
#include "tile_animation_manager.h"

/* Animated tile image
 * An animated tile has always three frames.
 */
class AnimatedTile: public TileData {

 private:
  /* Animation sequence type: 1-2-3-2 or 1-2-3
   */
  const tile_animation_sequence_t sequence;
  
  /* Array of 3 SDL_Rect representing the 3 animation frames
   * in the tileset image.
   * The 3 frames should have the same width and height.
   */
  SDL_Rect where_in_tileset[3];

 public:
  /* Constructor
   */
  AnimatedTile(const SDL_Rect *where_in_tileset,
	       tile_animation_sequence_t sequence,
	       tile_obstacle_t obstacle);

  /* Destructor
   */
  inline ~AnimatedTile(void) { }

  /* Return the tile image width
   */
  inline int get_w(void) const { return where_in_tileset[0].w; }

  /* Return the tile image height
   */
  inline int get_h(void) const { return where_in_tileset[0].h; }

  /* Display the tile image on the map
   */
  void display_on_map(Map *map, SDL_Rect &where_in_map);
};

#endif
