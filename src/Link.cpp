#include "Link.h"
#include "AnimatedSprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include "Map.h"
#include "Savegame.h"
#include "Sound.h"

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
 * String constants corresponding to Link sprite's description.
 */
const SpriteId Link::link_sprite_ids[3] = {
  "link_green", // tunic 0
  "link_blue",  // tunic 1
  "link_red",   // tunic 2
};

/**
 * Constructor.
 */
Link::Link(void):
  Moving8ByPlayer(12), sprite(NULL) {

  SDL_Rect collision_box;
  
  collision_box.x = -8;
  collision_box.y = -15;
  collision_box.w = 16;
  collision_box.h = 16;

  set_collision_box(collision_box);
  set_sprite();
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
 * Sets the sprite of Link, depending on its equipment
 * as specified in the savegame.
 */
void Link::set_sprite(void) {

  if (sprite != NULL) {
    delete sprite;
  }

  Savegame *save = zsdx->game->get_savegame();
  GameResource *resource = zsdx->game_resource;

  int tunic_number = save->get_reserved_integer(SAVEGAME_LINK_TUNIC);

  sprite = new AnimatedSprite(resource->get_sprite(link_sprite_ids[tunic_number]));
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

/**
 * Lets Link swinging his sword if possible.
 */
void Link::start_sword(void) {
  zsdx->game_resource->get_sound("sword1.wav")->play();
  sprite->set_current_animation("sword");
}
