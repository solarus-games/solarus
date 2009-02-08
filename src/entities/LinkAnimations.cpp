#include "entities/Link.h"
#include "entities/CarriedItem.h"
#include "Sprite.h"
#include "Equipment.h"
#include "Map.h"

/**
 * String constants corresponding to the sprites of Link's tunics.
 */
const SpriteAnimationSetId Link::tunic_sprite_ids[3] = {
  "link/tunic0", // green tunic
  "link/tunic1", // blue tunic
  "link/tunic2", // red tunic
};

/**
 * String constants corresponding to the sprites of Link's swords.
 */
const SpriteAnimationSetId Link::sword_sprite_ids[4] = {
  "link/sword1",
  "link/sword2",
  "link/sword3",
  "link/sword4",
};

/**
 * String constants corresponding to the sprites of the stars of Link's swords.
 */
const SpriteAnimationSetId Link::sword_stars_sprite_ids[4] = {
  "link/sword_stars1",
  "link/sword_stars1",
  "link/sword_stars2",
  "link/sword_stars2",
};

/**
 * String constants corresponding to the sprites of the shields.
 */
const SpriteAnimationSetId Link::shield_sprite_ids[3] = {
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
 * Returns whether Link's sword is currently displayed on the screen.
 * @return true if Link's sword is currently displayed on the screen
 */
bool Link::is_sword_visible(void) {
  return equipment->has_sword() && sword_sprite != NULL && sword_sprite->is_animation_started();
}

/**
 * Returns whether the stars of Link's sword are currently displayed on the screen.
 * @return true if the stars of Link's sword are currently displayed on the screen
 */
bool Link::is_sword_stars_visible(void) {
  return equipment->has_sword() && sword_stars_sprite != NULL && sword_stars_sprite->is_animation_started();
}

/**
 * Returns whether Link's shield is currently displayed on the screen.
 * @return true if Link's shield is currently displayed on the screen
 */
bool Link::is_shield_visible(void) {
  return equipment->has_shield() && shield_sprite != NULL && shield_sprite->is_animation_started();
}

/**
 * Returns whether Link's shadow is currently displayed, separated from the tunic sprite.
 * @return true if Link's shadow is currently displayed, separated from the tunic sprite
 */
bool Link::is_shadow_visible(void) {
  return get_state() == JUMPING;
}

/**
 * Stops displaying Link's sword (if any).
 */
void Link::stop_displaying_sword(void) {

  if (is_sword_visible()) {
    sword_sprite->stop_animation();
    sword_stars_sprite->stop_animation();
  }
}

/**
 * Makes Link blink for a while.
 */
void Link::blink(void) {
  tunic_sprite->set_blinking(50);

  if (equipment->has_shield()) {
    shield_sprite->set_blinking(50);
  }
  if (equipment->has_sword()) {
    sword_sprite->set_blinking(50);
  }

  end_blink_date = SDL_GetTicks() + 2000;
}

/**
 * Stops making the sprites blink.
 */
void Link::stop_blinking() {

  tunic_sprite->set_blinking(0);

  if (equipment->has_shield()) {
    shield_sprite->set_blinking(0);
  }
  if (equipment->has_sword()) {
    sword_sprite->set_blinking(0);
  }
}

/**
 * Returns the direction of Link's sprites.
 * It is different from the movement direction.
 * @return the direction of Link's sprites (0 to 3)
 */
int Link::get_animation_direction(void) {
  return tunic_sprite->get_current_direction();
}

/**
 * Changes the direction of Link's sprites.
 * It is different from the movement direction.
 * @param direction the direction to set (0 to 3)
 */
void Link::set_animation_direction(int direction) {

  tunic_sprite->set_current_direction(direction);

  if (is_sword_visible()) {
    sword_sprite->set_current_direction(direction);
  }

  if (is_sword_stars_visible()) {
    sword_stars_sprite->set_current_direction(direction);
  }

  if (is_shield_visible()) {
    shield_sprite->set_current_direction(direction);
  }
}

/**
 * Saves the current direction of the Link's sprite.
 * Call restore_animation_direction() to restore the direction saved here.
 */
void Link::save_animation_direction(void) {
  this->animation_direction_saved = get_animation_direction();
}

/**
 * Restores the direction of Link's sprite saved by the last
 * save_animation_direction() call.
 */
void Link::restore_animation_direction(void) {
  set_animation_direction(animation_direction_saved);
}

/**
 * Updates the animation of Link's sprites if necessary.
 */
void Link::update_sprites(void) {

  // update the frames
  tunic_sprite->update();

  if (is_sword_visible()) {
    sword_sprite->update();
    sword_sprite->set_current_frame(tunic_sprite->get_current_frame());
    map->check_collision_with_detectors(this, sword_sprite);
  }

  if (is_sword_stars_visible()) {
    // the stars are not synchronized with the other sprites
    sword_stars_sprite->update();
  }

  if (is_shield_visible()) {
    shield_sprite->update();
    if (walking) {
      shield_sprite->set_current_frame(tunic_sprite->get_current_frame());    
    }
  }

  if (state == CARRYING && walking) {
    lifted_item->get_sprite()->set_current_frame(tunic_sprite->get_current_frame() % 3);
  }

  // blinking
  if (tunic_sprite->is_blinking() && SDL_GetTicks() >= end_blink_date) {
    stop_blinking();
  }
}

/**
 * Restarts the animation of Link's sprites.
 * This function is called when the sprites have to
 * get back to their first frame.
 */
void Link::restart_animation(void) {
  tunic_sprite->restart_animation();

  if (is_sword_visible()) {
    sword_sprite->restart_animation();
  }

  if (is_sword_stars_visible()) {
    sword_stars_sprite->restart_animation();
  }

  if (is_shield_visible()) {
    shield_sprite->restart_animation();
  }
}

/**
 * Starts the "stopped" animation of Link's sprites.
 */
void Link::set_animation_stopped(void) {

  int direction = tunic_sprite->get_current_direction();
  
  switch (get_state()) {

  case FREE:

    stop_displaying_sword();

    if (equipment->has_shield()) {

      tunic_sprite->set_current_animation("stopped_with_shield");

      shield_sprite->set_current_animation("stopped");
      shield_sprite->set_current_direction(direction);
    }
    else {
      tunic_sprite->set_current_animation("stopped");
    }

    break;
    
  case SWORD_LOADING:

    tunic_sprite->set_current_animation("sword_loading_stopped");

    sword_sprite->set_current_animation("sword_loading_stopped");
    sword_sprite->set_current_direction(direction);
    
    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_direction(direction);

    if (equipment->has_shield()) {

      if (direction % 2 != 0) {
	shield_sprite->set_current_animation("sword_loading_stopped");
	shield_sprite->set_current_direction(direction / 2);
      }
      else {
	shield_sprite->stop_animation();
      }
    }

    break;

  case CARRYING:
    tunic_sprite->set_current_animation("carrying_stopped");
    lifted_item->set_animation_stopped();
    break;

  default:
    break;
  }

  walking = false;
}

/**
 * Starts the "walking" animation of Link's sprites.
 */
void Link::set_animation_walking(void) {

  int direction = tunic_sprite->get_current_direction();
  
  switch (get_state()) {
    
  case FREE:
    
    stop_displaying_sword();

    if (equipment->has_shield()) {

      tunic_sprite->set_current_animation("walking_with_shield");
      
      shield_sprite->set_current_animation("walking");
      shield_sprite->set_current_direction(direction);
    }
    else {
      tunic_sprite->set_current_animation("walking");
    }

    break;

  case SWORD_LOADING:
    
    tunic_sprite->set_current_animation("sword_loading_walking");

    sword_sprite->set_current_animation("sword_loading_walking");
    sword_sprite->set_current_direction(direction);

    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_direction(direction);

    if (equipment->has_shield()) {

      if (direction % 2 != 0) {
	shield_sprite->set_current_animation("sword_loading_walking");
	shield_sprite->set_current_direction(direction / 2);
      }
      else {
	shield_sprite->stop_animation();
      }
    }

    break;

  case CARRYING:
    tunic_sprite->set_current_animation("carrying_walking");
    lifted_item->set_animation_walking();
    break;

  default:
    break;
  }

  walking = true;
}

/**
 * Starts (or restarts) the "sword" animation of Link's sprites.
 * Link's state should be SWORD.
 */
void Link::set_animation_sword(void) {

  int direction = tunic_sprite->get_current_direction();
  
  tunic_sprite->set_current_animation("sword");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment->has_shield()) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword");
      shield_sprite->set_current_direction(direction / 2);
      shield_sprite->restart_animation();
    }
    else {
      shield_sprite->stop_animation();
    }
  }
}

/**
 * Starts the "grabbing" animation of Link's sprites.
 * Link's state should be GRABBING.
 */
void Link::set_animation_grabbing(void) {
  tunic_sprite->set_current_animation("grabbing");

  // the shield is not visible when Link is grabbing an object
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "pulling" animation of Link's sprites.
 * Link's state should be PULLING.
 */
void Link::set_animation_pulling(void) {
  tunic_sprite->set_current_animation("pulling");

  // the shield is not visible when Link is pulling an object
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "pushing" animation of Link's sprites.
 * Link's state should be PUSHING.
 */
void Link::set_animation_pushing(void) {
  tunic_sprite->set_current_animation("pushing");

  // the shield is not visible when Link is pushing
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "lifting" animation of Link's sprites.
 * Link's state should be LIFTING.
 */
void Link::set_animation_lifting(void) {
  tunic_sprite->set_current_animation("lifting");

  // the shield is not visible when Link is lifting
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "jumping" animation of Link's sprites.
 * Link's state should be JUMPING.
 */
void Link::set_animation_jumping(void) {
  tunic_sprite->set_current_animation("jumping");

  if (equipment->has_shield()) {
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_direction(get_animation_direction());
  }
}

/**
 * Starts the "hurt" animation of Link's sprites.
 * Link's state should be HURT.
 */
void Link::set_animation_hurt(void) {
  tunic_sprite->set_current_animation("hurt");

  // the shield is not visible when Link is hurt
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "gameover" animation of Link's sprites.
 * Link's state should be GAMEOVER.
 */
void Link::set_animation_gameover(void) {
  stop_blinking();
  tunic_sprite->set_current_animation("gameover");
  tunic_sprite->set_current_direction(0);

  // the shield is not visible
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}
