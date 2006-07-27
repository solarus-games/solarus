#ifndef GRAPHIC_OBJECTS_H
#define GRAPHIC_OBJECTS_H

#include <SDL/SDL.h>

class GraphicObject {

 private:

 public:
  virtual GraphicObject(void) = 0;
  virtual ~GraphicObject(void) = 0;

  virtual int get_x(void) = 0;
  virtual int get_y(void) = 0;
  virtual int get_width(void) = 0;
  virtual int get_height(void) = 0;

  virtual SDL_Surface *get_image(void) = 0;
};


// Tiles

const int TILE_SIZE = 8; // 8 pixels

enum tile_obstacle_t {
  NO_OBSTACLE,
  OBSTACLE,
  OBSTACLE_UP_RIGHT,
  OBSTACLE_UP_LEFT,
  OBSTACLE_DOWN_LEFT,
  OBSTACLE_DOWN_RIGHT,
}

class Tile: public GraphicObject { // inherit from GraphicObject?

 private:
  SDL_Surface *surface; // for now a tile is just a single surface
  tile_obstacle_t obstacle;

 public:
  Tile::Tile(SDL_Surface *surface_src);

  inline int get_x(void) { return -1; } // a tile is shared on the map
  inline int get_y(void) { return -1; }
  inline int get_width(void) { return TILE_SIZE; } // every tile is 8*8
  inline int get_height(void) { return TILE_SIZE; }

  inline SDL_Surface *get_image(void) { return surface; }
}

#endif
