#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include <SDL/SDL.h>
#include "movable.h"

class Link: public Movable {

 private:
  SDL_Surface *link_stopped_image;
  SDL_Rect position_in_image;

  bool started;
  int direction_mask;
  void update_movement(void);

 public:
  Link(void);
  ~Link(void);

  // put these functions in a class
  // "8 directions controlled by keyboard"
  void start_right(void);
  void start_up(void);
  void start_left(void);
  void start_down(void);
  void stop_right(void);
  void stop_up(void);
  void stop_left(void);
  void stop_down(void);

  void display_on_map(SDL_Surface *map);

};

#endif
