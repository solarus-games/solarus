#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include <SDL/SDL.h>
#include "movable_8_keyboard.h"
#include "animated_sprite.h"

class Link: public Movable8Keyboard, public AnimatedSprite {

 private:
  SDL_Surface *link_stopped_image;

 protected:
  void create_animations(void);

 public:
  Link(void);
  ~Link(void);

  // redefinition of these methods from Movable8Keyboard
  // to take care of Link's animation direction 
  void start_right(void);
  void start_up(void);
  void start_left(void);
  void start_down(void);

  void display_on_map(SDL_Surface *map);
};

#endif
