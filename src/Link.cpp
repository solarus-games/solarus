#include "Link.h"
#include "AnimatedSprite.h"
#include "ZSDX.h"
#include "GameResource.h"
#include "Map.h"

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

/**
 * Constructor.
 */
Link::Link(void):
Moving8ByPlayer(12), sprite(new AnimatedSprite(ZSDX::game_resource->get_sprite("link_red"))) {
  SDL_Rect collision_box;

  collision_box.x = -8;
  collision_box.y = -15;
  collision_box.w = 16;
  collision_box.h = 16;

  set_collision_box(collision_box);
}

/**
 * Destructor.
 */
Link::~Link(void) {
  delete sprite;
}

/**
 * Sets Link's current map.
 * @param map the map
 */
void Link::set_map(Map *map) {
  MovingWithCollision::set_map(map);
}

/**
 * Displays Link on the map with its current animation and
 * at its current position
 * @param map the map
 */
void Link::display_on_map(Map *map) {

  map->display_sprite(get_sprite(), &position_in_map);
}

/**
 * Redefinition of Moving8ByPlayer::update_movement
 * to take care of link's animation.
 * This function is called when the user has pressed or
 * released a keyboard arrow.
 */
void Link::update_movement(void) {
  bool old_started = started;

  // update the movement
  Moving8ByPlayer::update_movement();

  // has the direction changed?
  int direction = get_direction();

  if (direction != -1) {
    int old_animation_direction = sprite->get_current_animation_direction();
    int animation_direction = animation_directions[direction_mask];

    if (animation_direction != old_animation_direction) {
      // if the direction defined by the arrows has changed,
      // update the sprite's direction of animation
      sprite->set_current_animation_direction(animation_direction);
    }
  }

  // update the animation

  // stopped to walking
  if (!old_started && started) {
    sprite->set_current_animation("walking");
  }

  // walking to stopped
  else if (old_started && !started) {
    sprite->set_current_animation("stopped");
  }
}
