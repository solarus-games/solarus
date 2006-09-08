#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "link.h"
#include "animated_sprite.h"
#include "link_animations.h"

Link::Link(void):
Movable8ByPlayer(12), AnimatedSprite(LinkAnimations::get_instance()) {

}

Link::~Link(void) {
  LinkAnimations::destroy_instance();
}

void Link::display_on_map(Map *map) {
  update_position();
  AnimatedSprite::display_on_map(map, where_in_map);
}

void Link::start_right(void) {
  Movable8ByPlayer::start_right();
  set_current_animation_direction(0);
}

void Link::start_up(void) {
  Movable8ByPlayer::start_up();
  set_current_animation_direction(1);
}

void Link::start_left(void) {
  Movable8ByPlayer::start_left();
  set_current_animation_direction(2);
}

void Link::start_down(void) {
  Movable8ByPlayer::start_down();
  set_current_animation_direction(3);
}

void Link::update_movement(void) {
  bool old_started = started;
  Movable8ByPlayer::update_movement();

  // stopped to walking
  if (!old_started && started) {
    set_current_animation(1);
  }

  // walking to stopped
  else if (old_started && !started) {
    set_current_animation(0);
  }
}
