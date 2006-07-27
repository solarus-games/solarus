#ifndef ZSDX_MAP_OBJECT_H
#define ZSDX_MAP_OBJECT_H

#include <SDL/SDL.h>

class MapObject {

 private:
  SDL_Rect rectangle;

 public:
  virtual ~MapObject(void) = 0;

  inline int get_x(void) { return rectangle.x; }
  inline int get_y(void) { return rectangle.y; }
  inline int get_width(void) { return rectangle.w; }
  inline int get_height(void) { return rectangle.h; }

  virtual void display_on_map(SDL_Surface *map) = 0;

  // not good for extensible tiles...
  // virtual SDL_Surface *get_image(void) = 0;
};


// Tiles

enum tile_obstacle_t {
  NO_OBSTACLE,
  OBSTACLE,
  OBSTACLE_UP_RIGHT,
  OBSTACLE_UP_LEFT,
  OBSTACLE_DOWN_LEFT,
  OBSTACLE_DOWN_RIGHT,
};

class Tile: public MapObject {

 private:
  int x, y;
  tile_obstacle_t obstacle;
  // TODO: TileData tile_data;

 public:
  inline int get_x(void) { return -1; } // redefinition because a tile is shared on the map: TODO: throw an exception?
  inline int get_y(void) { return -1; }
};

#endif
