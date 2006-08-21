#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include <SDL/SDL.h>
#include "movable_8_keyboard.h"

class Link: public Movable8Keyboard {

 private:
  SDL_Surface *link_stopped_image;
  SDL_Rect position_in_image;

 public:
  Link(void);
  ~Link(void);

  void display_on_map(SDL_Surface *map);
};

#endif
