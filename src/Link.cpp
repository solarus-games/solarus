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
  state(LINK_STATE_FREE),
  equipment(zsdx->game->get_savegame()->get_equipment()),
  tunic_sprite(NULL), sword_sprite(NULL), shield_sprite(NULL),
  pushing_counter(0), next_pushing_counter_date(0) {

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
 * This function is called repeteadly by the game.
 */
void Link::update(void) {

  // update the movement
  set_moving_enabled(!zsdx->game->is_suspended() && get_state() <= LINK_STATE_SWIMMING);

  update_position();
  update_sprites();
}

/**
 * Displays Link on the map with its current animation and
 * at its current position.
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

  start_free();
}

/**
 * Updates the animation of Link's sprites if necessary.
 */
void Link::update_sprites(void) {

  // suspend the animation when the game is suspended
  if (zsdx->game->is_suspended() && !tunic_sprite->is_suspended()) {
    set_animation_suspended(true);
  }
  else if (tunic_sprite->is_suspended() && !zsdx->game->is_suspended()) {
    set_animation_suspended(false);
  }

  // update the frames
  tunic_sprite->update_current_frame();

  if (is_sword_visible()) {
    sword_sprite->set_current_frame(tunic_sprite->get_current_frame());
  }

  if (is_shield_visible()) {
    shield_sprite->set_current_frame(tunic_sprite->get_current_frame());    
  }
}

/**
 * Changes the movement of the entity depending on the arrows pressed.
 * This function is called when an arrow is pressed or released
 * on the keyboard, or when the movement has just been enabled or
 * disabled (i.e. when set_moving_enabled() is called).
 * This is a redefinition of Moving8ByPlayer::update_movement
 * to take care of Link's animation.
 */
void Link::update_movement(void) {

  // update the movement according to the arrows keys pressed
  Moving8ByPlayer::update_movement();

  // update the animation direction according to the movement direction
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

  // show the animation corresponding to the movement tried by the player
  if (can_move) {    
    string animation = tunic_sprite->get_current_animation();

    // stopped to walking
    if (started && animation == "stopped") {
      set_animation_walking();
    }
      
    // walking to stopped
    else if (!started && animation == "walking") {
      set_animation_stopped();
    }
  }
}

/**
 * Updates Link's position.
 * This function is called repeatedly by update().
 */
void Link::update_position(void) {

  Uint32 now = SDL_GetTicks();

  // no position change when the game is suspended
  if (zsdx->game->is_suspended()) {
    next_pushing_counter_date = now;
    return;
  }

  bool move_tried = has_to_move_now();
  int old_x = 0, old_y = 0;
  if (move_tried) {
    // save the current coordinates
    old_x = get_x();
    old_y = get_y();

    // try to move Link
    Moving::update();
  }
  
  // the rest of the function handles the "pushing" animation

  if (state == LINK_STATE_FREE && move_tried) {
    // Link is trying to move with animation "walking"

    // see if the move has failed (i.e. if Link's coordinates have not changed)
    if (get_x() == old_x && get_y() == old_y) {

      // Link is facing an obstacle

      Uint32 now = SDL_GetTicks();
      if (pushing_counter == 0) {
	next_pushing_counter_date = now;
	pushing_direction_mask = direction_mask;
      }

      while (now >= next_pushing_counter_date) {
	pushing_counter++;
	next_pushing_counter_date += 100;
      }
      
      if (pushing_counter >= 8) {
	start_pushing(); // start animation "pushing" when the pushing counter gets to 8
      }
    }
    else {
      pushing_counter = 0;
    }
  }
  else {
    
    // stop pushing if the player changes his direction
    if (state == LINK_STATE_PUSHING && direction_mask != pushing_direction_mask) {
      start_free();
    }

    // reset the pushing counter if the state changes (for example when Link swing his sword)
    // of if the player changes his direction
    if (pushing_counter > 0 &&
	(state != LINK_STATE_FREE || direction_mask != pushing_direction_mask)) {

      pushing_counter = 0;
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
}

/**
 * Lets Link can walk.
 * Moves to the state LINK_STATE_FREE and updates the animations accordingly.
 */
void Link::start_free(void) {
  set_state(LINK_STATE_FREE);
  
  if (started) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * Lets Link swinging his sword.
 * Moves to the state LINK_STATE_SWORD_SWINGING, plays the sword sound
 * and updates the animations accordingly.
 */
void Link::start_sword(void) {
  set_state(LINK_STATE_SWORD_SWINGING);
  sword_sound->play();
  set_animation_sword();
}

/**
 * Makes Link push something.
 * Moves to the state LINK_STATE_FREE and updates the animations accordingly.
 */
void Link::start_pushing(void) {
  set_state(LINK_STATE_PUSHING);
  set_animation_pushing();
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
    start_free();
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
 * Restarts the animation of Link's sprites.
 * This function is called when the sprites have to
 * get back to their first frame.
 */
void Link::restart_animation(void) {
  tunic_sprite->restart_animation();

  if (is_sword_visible()) {
    sword_sprite->restart_animation();
  }

  if (is_shield_visible()) {
    shield_sprite->restart_animation();
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
 * Starts (or restarts) the "sword" animation of Link's sprites.
 * Link's state should be LINK_STATE_SWORD.
 */
void Link::set_animation_sword(void) {

  int direction = tunic_sprite->get_current_animation_direction();
  
  tunic_sprite->set_current_animation("sword");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword");
  sword_sprite->set_current_animation_direction(direction);
  sword_sprite->restart_animation();

  if (equipment->has_shield()) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword");
      shield_sprite->set_current_animation_direction(direction / 2);
      shield_sprite->restart_animation();
    }
    else {
      shield_sprite->stop_animation();
    }
  }
}

/**
 * Starts the "pushing" animation of Link's sprites.
 * Link's state should be LINK_STATE_PUSHING.
 */
void Link::set_animation_pushing(void) {
  tunic_sprite->set_current_animation("pushing");

  // the shield is not visible when Link is pushing
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}
