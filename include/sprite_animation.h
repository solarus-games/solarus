#ifndef ZSDX_SPRITE_ANIMATION_H
#define ZSDX_SPRITE_ANIMATION_H

#include <SDL/SDL.h>
#include "sprite_animation_direction.h"

class SpriteAnimation {

 private:
  const int nb_directions;
  int current_direction;
  SpriteAnimationDirection **directions;

  const Uint32 frame_interval;
  Uint32 next_frame_date;

  void update_current_frame(void);
  
 public:
  SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions, Uint32 frame_interval, Uint32 next_frame_date);
  SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions);
  virtual ~SpriteAnimation(void);

  inline int get_current_direction(void) { return current_direction; }
  inline void set_current_direction(int current_direction) {
    this->current_direction = current_direction;
  }

  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

#endif
