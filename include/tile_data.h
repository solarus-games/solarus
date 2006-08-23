#ifndef ZSDX_TILE_DATA_H
#define ZSDX_TILE_DATA_H

#include <SDL/SDL.h>
#include "datatypes.h"
#include "animation_frame_handler.h"
#include "map_object.h"

/* TODO:
 * - rename into TileImage or something else?
 * - move the subclasses into separate files
 */

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
  virtual void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map) = 0;
};

/* Simple tile image, static and fixed
 */
class SimpleTile: public TileData {

 protected:
  /* Image from which the tile is extracted
   * TODO: put that in class Tileset
   */
  SDL_Surface *src_image;

  /* Position of the tile in src_image
   */
  SDL_Rect where_in_src;

 public:
  /* Constructor
   */
  SimpleTile(SDL_Surface *src_image, SDL_Rect &where_in_src, tile_obstacle_t obstacle);

  /* Destructor
   */
  ~SimpleTile(void);

  /* Display the tile image on the map
   */
  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

/* Tile pattern that can be extended
 */
class ExtensibleTile: public TileData {

 private:
  /* Image from which the tile is extracted
   */
  SDL_Surface *src_image;

  /* Position of the tile in src_image
   */
  SDL_Rect where_in_src;

  /* How many times the pattern is repeated in x
   */
  const int repeat_x;

  /* How many times the pattern is repeated in y
   */
  const int repeat_y;

 public:
  /* Create an extensible tile with a new pattern
   */
  ExtensibleTile(SDL_Surface *src_image,
		 SDL_Rect &where_in_src,
		 tile_obstacle_t obstacle);

  /* Create an extensible tile with the same pattern as another one
   */
  ExtensibleTile(ExtensibleTile *other, int repeat_x, int repeat_y);

  /* Destructor
   */
  ~ExtensibleTile(void);

  /* Display the tile image on the map
   */
  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

/* Animated tile image
 * An animated tile has always three frames.
 */
class AnimatedTile: public TileData {

 private:
  /* Image from which the tile is extracted
   */
  SDL_Surface *src_image;

  /* Animation sequence type: 1-2-3-2 or 1-2-3
   */
  const animation_sequence_t sequence;
  
  /* Array of 3 SDL_Rect representing the 3 animation frames
   * The 3 frames should have the same width and height.
   */
  SDL_Rect where_in_src[3];

 public:
  /* Constructor
   */
  AnimatedTile(SDL_Surface *src_image,
	       const SDL_Rect *where_in_src,
	       animation_sequence_t sequence,
	       tile_obstacle_t obstacle);

  /* Destructor
   */
  ~AnimatedTile(void);

  /* Return the tile image width
   */
  inline int get_w(void) const { return where_in_src[0].w; }

  /* Return the tile image height
   */
  inline int get_h(void) const { return where_in_src[0].h; }

  /* Display the tile image on the map
   */
  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

#endif
