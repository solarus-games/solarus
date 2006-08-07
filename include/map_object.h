#ifndef ZSDX_MAP_OBJECT_H
#define ZSDX_MAP_OBJECT_H

#include <SDL/SDL.h>

class MapObject {

 protected:
  SDL_Rect where_in_map;

 public:
  virtual ~MapObject(void) { };

  inline int get_x(void) { return where_in_map.x; }
  inline int get_y(void) { return where_in_map.y; }
  inline int get_width(void) { return where_in_map.w; }
  inline int get_height(void) { return where_in_map.h; }

  virtual void display_on_map(SDL_Surface *map) = 0;

  // not good for extensible tiles...
  // virtual SDL_Surface *get_image(void) = 0;
};

#endif
