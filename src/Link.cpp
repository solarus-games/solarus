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

  set_size(16, 16);
  set_hotspot(8, 15);
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

  map->display_sprite(get_sprite(), get_x(), get_y());
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
  sprite->set_animation_listener(this); // to be notified when an animation of Link is over
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
  if (state <= LINK_STATE_SWIMMING) {
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
}

/**
 * Returns Link's state.
 * @return the state of Link
 */
LinkState Link::get_state(void) {
  return state;
}

/**
 * Sets Link's state.
 * @param state the state of Link
 */
void Link::set_state(LinkState state) {

  this->state = state;

  set_moving_enabled(state <= LINK_STATE_SWIMMING);

  switch (state) {

  case LINK_STATE_FREE:
    update_movement();
    sprite->set_current_animation(started ? "walking" : "stopped");
    break;

  default:
    break;

  }
}

/**
 * Lets Link swinging his sword if possible.
 */
void Link::start_sword(void) {
  set_state(LINK_STATE_SWORD_SWINGING);
  zsdx->game_resource->get_sound("sword1.wav")->play();
  sprite->set_current_animation("sword");
}

/**
 * This function is called when an animation of Link's sprite is over.
 */
void Link::animation_over(AnimatedSprite *sprite) {

  string animation_name = sprite->get_current_animation();

  if (animation_name == "sword") {
    set_state(LINK_STATE_FREE);
  }
}

