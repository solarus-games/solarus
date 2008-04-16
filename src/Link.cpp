#include "Link.h"
#include "AnimatedSprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include "Savegame.h"
#include "Equipment.h"
#include "Map.h"
#include "Sound.h"

/**
 * Indicates the direction of link's animation (from 0 to 4, or -1 for no change)
 * depending on the arrows pressed on the keyboard.
 */
static const int animation_directions[] = {
  -1,  // none: no change
  0,   // right
  1,   // up
  0,   // right + up: right
  2,   // left
  -1,  // left + right: no change
  2,   // left + up: left
  -1,  // left + right + up: no change
  3,   // down
  0,   // down + right: right
  -1,  // down + up: no change
  -1,  // down + right + up: no change
  2,   // down + left: left
  -1,  // down + left + right: no change
  -1,  // down + left + up: no change
  -1,  // down + left + right + up: no change
};

/**
 * String constants corresponding to the sprites of Link's tunics.
 */
const SpriteId Link::tunic_sprite_ids[3] = {
  "link/tunic0", // green tunic
  "link/tunic1", // blue tunic
  "link/tunic2", // red tunic
};

/**
 * String constants corresponding to the sprites of Link's swords.
 */
const SpriteId Link::sword_sprite_ids[4] = {
  "link/sword1",
  "link/sword2",
  "link/sword3",
  "link/sword4",
};

/**
 * String constants corresponding to the sprites of the shields.
 */
const SpriteId Link::shield_sprite_ids[3] = {
  "link/shield1",
  "link/shield2",
  "link/shield3",
};

/**
 * String constants corresponding to the sounds of the swords.
 */
const SoundId Link::sword_sound_ids[4] = {
  "sword1",
  "sword2",
  "sword3",
  "sword4",
};

/**
 * Constructor.
 */
Link::Link(void):
  Moving8ByPlayer(12),
  state(LINK_STATE_FREE), previous_state(LINK_STATE_FREE),
  equipment(zsdx->game->get_savegame()->get_equipment()),
  tunic_sprite(NULL), sword_sprite(NULL), shield_sprite(NULL) {

  set_size(16, 16);
  set_hotspot(8, 15);
}

/**
 * Destructor.
 */
Link::~Link(void) {
  delete tunic_sprite;

  if (sword_sprite != NULL) {
    delete sword_sprite;
  }

  if (shield_sprite != NULL) {
    delete shield_sprite;
  }
}

/**
 * Sets Link's current map.
 * @param map the map
 * @param initial_direction the direction of Link (0 to 3)
 */
void Link::set_map(Map *map, int initial_direction) {
  MovingWithCollision::set_map(map);
  set_animation_direction(initial_direction);
}

/**
 * Updates Link's position and animation.
 * This function is called repeteadly by the map.
 */
void Link::update(void) {
  Moving::update(); // update the position
  update_sprites();
}

/**
 * Displays Link on the map with its current animation and
 * at its current position
 * @param map the map
 */
void Link::display_on_map(Map *map) {

  map->display_sprite(tunic_sprite, get_x(), get_y());

  if (is_sword_visible()) {
    map->display_sprite(sword_sprite, get_x(), get_y());
  }

  if (is_shield_visible()) {
    map->display_sprite(shield_sprite, get_x(), get_y());
  }
}

/**
 * Initializes the sprites of Link and his equipment,
 * depending on its equipment as specified in the savegame.
 * This function is called at the game beginning
 * and when Link's equipment is changed.
 */
void Link::initialize_sprites(void) {

  GameResource *resource = zsdx->game_resource;

  int animation_direction = -1;

  // Link
  if (tunic_sprite != NULL) {
    // save the animation direction
    animation_direction = tunic_sprite->get_current_animation_direction();
    delete tunic_sprite;
  }

  int tunic_number = equipment->get_tunic_number();

  tunic_sprite = new AnimatedSprite(resource->get_sprite(tunic_sprite_ids[tunic_number]));
  tunic_sprite->set_animation_listener(this); // to be notified when an animation of Link is over

  // Link's sword
  if (sword_sprite != NULL) {
    delete sword_sprite;
    sword_sprite = NULL;
  }

  int sword_number = equipment->get_sword_number();

  if (sword_number > 0) {
    // Link has a sword: get the sprite and the sound
    sword_sprite = new AnimatedSprite(resource->get_sprite(sword_sprite_ids[sword_number - 1]));
    sword_sprite->stop_animation();
    sword_sprite->set_animation_listener(this); // to be notified when an animation of the sword is over

    sword_sound = resource->get_sound(sword_sound_ids[sword_number - 1]);
  }

  // Link's shield
  if (shield_sprite != NULL) {
    delete shield_sprite;
    shield_sprite = NULL;
  }

  int shield_number = equipment->get_shield_number();

  if (shield_number > 0) {
    // Link has a shield
    shield_sprite = new AnimatedSprite(resource->get_sprite(shield_sprite_ids[shield_number - 1]));
  }

  // restore the animation direction
  if (animation_direction != -1) {
    set_animation_direction(animation_direction);
  }

  set_state(LINK_STATE_FREE);
}

/**
 * Updates the animation of Link's sprites if necessary.
 */
void Link::update_sprites(void) {

  tunic_sprite->update_current_frame();

  if (is_sword_visible()) {
    sword_sprite->update_current_frame();
  }

  if (is_shield_visible()) {
    shield_sprite->update_current_frame();
  }
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
      int old_animation_direction = tunic_sprite->get_current_animation_direction();
      int animation_direction = animation_directions[direction_mask];
      
      if (animation_direction != old_animation_direction
	  && animation_direction != -1) {
	// if the direction defined by the arrows has changed,
	// update the sprite's direction of animation
	set_animation_direction(animation_direction);
      }
    }

    // update the animation

    // stopped to walking
    if (!old_started && started) {
      set_animation_walking();
    }
    
    // walking to stopped
    else if (old_started && !started) {
      set_animation_stopped();
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

  this->previous_state = this->state;
  this->state = state;

  set_moving_enabled(state <= LINK_STATE_SWIMMING);

  switch (state) {

  case LINK_STATE_FREE:
    update_movement();

    if (started) {
      set_animation_walking();
    }
    else {
      set_animation_stopped();
    }
    break;

  default:
    break;

  }
}

/**
 * Restores the previous state of Link.
 */
void Link::restore_state(void) {
  set_state(previous_state);
}

/**
 * Lets Link swinging his sword.
 */
void Link::start_sword(void) {

  set_state(LINK_STATE_SWORD_SWINGING);
  sword_sound->play();
  set_animation_sword();
}

/**
 * Returns whether Link's sword is currently displayed on the screen.
 * @return true if Link's sword is currently displayed on the screen
 */
bool Link::is_sword_visible(void) {
  return equipment->has_sword() && sword_sprite->is_animation_started();
}

/**
 * Returns whether Link's shield is currently displayed on the screen.
 * @return true if Link's shield is currently displayed on the screen
 */
bool Link::is_shield_visible(void) {
  return equipment->has_shield() && shield_sprite->is_animation_started();
}

/**
 * This function is called when an animation of Link's sprite is over.
 * @param sprite the sprite
 */
void Link::animation_over(AnimatedSprite *sprite) {

  string animation_name = tunic_sprite->get_current_animation();

  if (animation_name == "sword") {
    set_state(LINK_STATE_FREE);
  }
}

/**
 * Changes the direction of Link's sprites.
 * It is different from the movement direction, which you can
 * set by set_direction().
 * @param direction the direction to set (0 to 3)
 */
void Link::set_animation_direction(int direction) {

  tunic_sprite->set_current_animation_direction(direction);

  if (is_sword_visible()) {
    sword_sprite->set_current_animation_direction(direction);
  }

  if (is_shield_visible()) {
    shield_sprite->set_current_animation_direction(direction);
  }
}

/**
 * Starts the "stopped" animation of Link's sprites.
 */
void Link::set_animation_stopped(void) {

  tunic_sprite->set_current_animation("stopped");

  if (is_sword_visible()) {
    sword_sprite->stop_animation();
  }

  if (equipment->has_shield()) {

    int direction = tunic_sprite->get_current_animation_direction();
  
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_animation_direction(direction);
  }
}

/**
 * Starts the "walking" animation of Link's sprites.
 */
void Link::set_animation_walking(void) {
  
  tunic_sprite->set_current_animation("walking");

  if (is_sword_visible()) {
    sword_sprite->stop_animation();
  }

  if (equipment->has_shield()) {

    int direction = tunic_sprite->get_current_animation_direction();
  
    shield_sprite->set_current_animation("walking");
    shield_sprite->set_current_animation_direction(direction);
  }
}

/**
 * Suspends or resumes the animation of Link's sprites.
 * @param suspended true to suspend the animation, false to resume it
 */
void Link::set_animation_suspended(bool suspended) {
  tunic_sprite->set_suspended(suspended);

  if (equipment->has_sword()) {
    sword_sprite->set_suspended(suspended);
  }

  if (equipment->has_shield()) {
    shield_sprite->set_suspended(suspended);
  }
}

/**
 * Starts the "sword" animation of Link's sprites.
 * Link's state should be LINK_STATE_SWORD.
 */
void Link::set_animation_sword(void) {

  int direction = tunic_sprite->get_current_animation_direction();
  
  tunic_sprite->set_current_animation("sword");

  sword_sprite->set_current_animation("sword");
  sword_sprite->set_current_animation_direction(direction);

  if (equipment->has_shield()) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword");
      shield_sprite->set_current_animation_direction(direction / 2);
    }
    else {
      shield_sprite->stop_animation();
    }
  }
}
