#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "link.h"
#include "animated_sprite.h"
#include "link_animations.h"

/**
 * Indicates the direction of link's animation (from 0 to 4, or -1 for no change)
 * depending on the arrows pressed on the keyboard.
 */
static const int animation_directions[] = {
  -1,  // none: no change
  0,   // right
  1,   // up
  0,   // right + up
  2,   // left
  -1,  // left + right: no change
  2,   // left + up
  -1,  // left + right + up: no change
  3,   // down
  0,   // down + right
  -1,  // down + up: no change
  -1,  // down + right + up: no change
  2,   // down + left
  -1,  // down + left + right: no change
  -1,  // down + left + up: no change
  -1,  // down + left + right + up: no change
};

Link::Link(void):
Movable8ByPlayer(12), AnimatedSprite(LinkAnimations::get_instance()) {
  SDL_Rect collision_box;
  collision_box.x = -8;
  collision_box.y = -16;
  collision_box.w = 16;
  collision_box.h = 16;
  set_collision_box(collision_box);
}

Link::~Link(void) {
  LinkAnimations::destroy_instance();
}

void Link::set_map(Map *map) {
  MovableWithCollision::set_map(map);
}

void Link::display_on_map(Map *map) {
  update_position();
  AnimatedSprite::display_on_map(map, where_in_map);
}

void Link::update_movement(void) {
  bool old_started = started;

  Movable8ByPlayer::update_movement();

  // has the direction changed?
  int direction = get_direction();

  if (direction != -1) {
    int old_animation_direction = get_current_animation_direction();
    int animation_direction = animation_directions[direction_mask];
    if (animation_direction != old_animation_direction) {
      set_current_animation_direction(animation_directions[direction_mask]);
    }
  }

  // stopped to walking
  if (!old_started && started) {
    set_current_animation(1);
  }

  // walking to stopped
  else if (old_started && !started) {
    set_current_animation(0);
  }
}
