#ifndef ZSDX_TILE_DATA_H
#define ZSDX_TILE_DATA_H

#include <SDL/SDL.h>
#include "datatypes.h"
#include "map_object.h"

class TileData {
  
 protected:
  tile_obstacle_t obstacle;

 public:
  inline TileData(tile_obstacle_t obstacle) { this->obstacle = obstacle; }
  inline virtual ~TileData(void) { };
  virtual void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map) = 0;
};

class SimpleTile: public TileData {

 protected:
  SDL_Surface *src_image;
  SDL_Rect where_in_src;

 public:
  SimpleTile(SDL_Surface *src_image, SDL_Rect &where_in_src, tile_obstacle_t obstacle);
  ~SimpleTile(void);

  inline int get_w(void) const { return where_in_src.w; }
  inline int get_h(void) const { return where_in_src.h; }

  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

class ExtensibleTile: public SimpleTile {

 private:
  int repeat_x;
  int repeat_y;

 public:
  ExtensibleTile(SDL_Surface *src_image, SDL_Rect &where_in_src, tile_obstacle_t obstacle, int repeat_x, int repeat_y);
  ~ExtensibleTile(void);

  inline int get_w(void) const { return where_in_src.w * repeat_x; }
  inline int get_h(void) const { return where_in_src.h * repeat_y; }

  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

class AnimatedTile: public TileData {

 private:
  SDL_Surface *src_image;
  animation_sequence_t sequence;

  SDL_Rect where_in_src[3]; /* array of 3 rect */

 public:
  AnimatedTile(SDL_Surface *src_image,
	       const SDL_Rect *where_in_src,
	       animation_sequence_t sequence,
	       tile_obstacle_t obstacle);
  ~AnimatedTile(void);

  inline int get_w(void) const { return where_in_src[0].w; } // we assume the dimensions are constant
  inline int get_h(void) const { return where_in_src[0].h; }

  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

#endif
