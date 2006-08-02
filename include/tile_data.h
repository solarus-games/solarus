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

  inline int get_w(void) { return where_in_src.w; }
  inline int get_h(void) { return where_in_src.h; }

  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

class ExtensibleTile: public SimpleTile {

 private:
  int repeat_x;
  int repeat_y;

 public:
  ExtensibleTile(SDL_Surface *src_image, SDL_Rect &where_in_src, tile_obstacle_t obstacle, int repeat_x, int repeat_y);
  ~ExtensibleTile(void);

  inline int get_w(void) { return where_in_src.w * repeat_x; }
  inline int get_h(void) { return where_in_src.h * repeat_y; }

  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);  
};

/* class AnimatedTile: public TileData { */

/* }; */

#endif
