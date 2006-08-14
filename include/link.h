#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include <SDL/SDL.h>

class Link {

 private:
  SDL_Surface *link_stopped_image;
  SDL_Rect position_in_image;
  SDL_Rect position_in_map;

 public:
  Link(void);
  ~Link(void);

  inline int get_x(void) { return position_in_map.x; }
  inline int get_y(void) { return position_in_map.y; }

  inline void set_x(int x) { position_in_map.x = x; }
  inline void set_y(int y) { position_in_map.y = y; }
  inline void set_position(int x, int y) { position_in_map.x = y; position_in_map.y = y; }

  inline void move_right(void) { position_in_map.x++; }
  inline void move_up(void) { position_in_map.y--; }
  inline void move_left(void) { position_in_map.x--; }
  inline void move_down(void) { position_in_map.y++; }

  void display(SDL_Surface *surface);

};

#endif
