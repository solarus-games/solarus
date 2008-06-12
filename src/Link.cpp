#include "Link.h"
#include "AnimatedSprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include "Savegame.h"
#include "Equipment.h"
#include "Map.h"
#include "Sound.h"
#include "Movement8ByPlayer.h"

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
 * String constants corresponding to the sprites of the stars of Link's swords.
 */
const SpriteId Link::sword_stars_sprite_ids[4] = {
  "link/sword_stars1",
  "link/sword_stars1",
  "link/sword_stars2",
  "link/sword_stars2",
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
  equipment(zsdx->game->get_savegame()->get_equipment()),
  tunic_sprite(NULL), sword_sprite(NULL), sword_stars_sprite(NULL), shield_sprite(NULL),
  state(LINK_STATE_FREE), walking(false),
  counter(0), next_counter_date(0),
  pushing_direction_mask(0xFFFF) {

  set_size(16, 16);
  set_hotspot(8, 15);
  set_movement(new Movement8ByPlayer(12));
}

/**
 * Destructor.
 */
Link::~Link(void) {
  delete tunic_sprite;

  if (sword_sprite != NULL) {
    delete sword_sprite;
    delete sword_stars_sprite;
  }

  if (shield_sprite != NULL) {
    delete shield_sprite;
  }
}

/**
 * Returns Link's movement.
 * @return Link's movement
 */
Movement8ByPlayer * Link::get_movement(void) {
  return (Movement8ByPlayer*) movement;
}

/**
 * Sets Link's current map.
 * @param map the map
 * @param initial_direction the direction of Link (0 to 3)
 */
void Link::set_map(Map *map, int initial_direction) {

  get_movement()->set_map(map);
  
  stop_displaying_sword();

  if (get_state() == LINK_STATE_SWORD_LOADING) {
    start_free();
  }

  set_animation_direction(initial_direction);
}

/**
 * Updates Link's position, movement and animation.
 * This function is called repeteadly by the game.
 */
void Link::update(void) {

  if (zsdx->game->is_suspended()) {
    get_movement()->set_moving_enabled(false);
    next_counter_date = SDL_GetTicks();
  }
  else {
    
    // update the movement
    get_movement()->set_moving_enabled(get_state() <= LINK_STATE_SWIMMING);
    
    // specific updates in some states
    if (get_state() == LINK_STATE_SWORD_LOADING) {
      update_sword_loading();
    }
  }

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

  if (is_sword_stars_visible()) {
    map->display_sprite(sword_stars_sprite, get_x(), get_y());
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
    delete sword_stars_sprite;
    sword_sprite = NULL;
  }

  int sword_number = equipment->get_sword_number();

  if (sword_number > 0) {
    // Link has a sword: get the sprite and the sound
    sword_sprite = new AnimatedSprite(resource->get_sprite(sword_sprite_ids[sword_number - 1]));
    sword_sprite->stop_animation();

    sword_sound = resource->get_sound(sword_sound_ids[sword_number - 1]);

    sword_stars_sprite = new AnimatedSprite(resource->get_sprite(sword_stars_sprite_ids[sword_number - 1]));
    sword_stars_sprite->stop_animation();
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

  if (is_sword_stars_visible()) {
    // the stars are not synchronized with the other sprites
    sword_stars_sprite->update_current_frame();
  }

  if (is_shield_visible()) {
    shield_sprite->set_current_frame(tunic_sprite->get_current_frame());    
  }
}

/**
 * Updates Link depending on the arrows pressed.
 * This function is called when Link's direction changes (typically, because the player
 * pressed or released an arrow). It updates Link's animations according to the new movement.
 */
void Link::movement_just_changed(void) {

  // update the animation direction according to the movement direction
  int direction = get_direction();
  if (direction != -1) {

    Uint16 direction_mask = get_movement()->get_direction_mask();
    int old_animation_direction = tunic_sprite->get_current_animation_direction();
    int animation_direction = animation_directions[direction_mask];
      
    if (animation_direction != old_animation_direction
	&& animation_direction != -1
	&& !is_direction_locked()) {
      // if the direction defined by the arrows has changed,
      // update the sprite's direction of animation
      // (unless Link is loading his sword)
      set_animation_direction(animation_direction);
    }
  }

  // show the animation corresponding to the movement tried by the player
  if (get_movement()->is_moving_enabled()) {
    // the player can move
    string animation = tunic_sprite->get_current_animation();

    bool started = get_movement()->is_started();

    // stopped to walking
    if (started && !walking) {
      set_animation_walking();
    }
      
    // walking to stopped
    else if (!started && walking) {
      set_animation_stopped();
    }
  }
}

/**
 * Updates Link's position.
 * This function is called repeatedly by update().
 */
void Link::update_position(void) {

  // no position change when the game is suspended
  if (zsdx->game->is_suspended()) {
    return;
  }

  bool move_tried = get_movement()->has_to_move_now();
  int old_x = 0, old_y = 0;
  if (move_tried) {
    // save the current coordinates
    old_x = get_x();
    old_y = get_y();

    // try to move Link
    movement->update();
  }
  
  // the rest of the function handles the "pushing" animation

  Uint16 direction_mask = get_movement()->get_direction_mask();

  if (state == LINK_STATE_FREE && move_tried) {
    // Link is trying to move with animation "walking"

    // see if the move has failed (i.e. if Link's coordinates have not changed)
    if (get_x() == old_x && get_y() == old_y) {

      // Link is facing an obstacle

      Uint32 now = SDL_GetTicks();
      if (pushing_direction_mask == 0xFFFF) { // we start counting to trigger animation "pushing"
	counter = 0;
	next_counter_date = now;
	pushing_direction_mask = direction_mask;
      }

      while (now >= next_counter_date) {
	counter++;
	next_counter_date += 100;
      }
      
      if (counter >= 8) {
	start_pushing(); // start animation "pushing" when the counter gets to 8
      }
    }
    else {
      // Link has just moved successfuly
      counter = 0;
      pushing_direction_mask = 0xFFFF;
    }
  }
  else {

    // stop pushing or trying to push if the state changes (for example when Link swing his sword)
    // of if the player changes his direction
    if (pushing_direction_mask != 0xFFFF && // Link is pushing or about to push
	direction_mask != pushing_direction_mask) {
      
      counter = 0;
      pushing_direction_mask = 0xFFFF;

      if (state == LINK_STATE_PUSHING) {
	start_free();
      }
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
  get_movement()->set_moving_enabled(state <= LINK_STATE_SWIMMING);
}

/**
 * Lets Link can walk.
 * Moves to the state LINK_STATE_FREE and updates the animations accordingly.
 */
void Link::start_free(void) {
  set_state(LINK_STATE_FREE);
  
  if (get_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * Lets Link swinging his sword if this action is possible.
 * The game should not be suspended.
 * Moves to the state LINK_STATE_SWORD_SWINGING, plays the sword sound
 * and updates the animations accordingly.
 */
void Link::start_sword(void) {
  if (can_start_sword()) {
    set_state(LINK_STATE_SWORD_SWINGING);
    sword_sound->play();
    set_animation_sword();
  }
}

/**
 * Returns whether Link can swing his sword right now.
 * The function returns true if the game is not suspended and Link
 * is in state LINK_STATE_FREE, LINK_STATE_PUSHING,
 * LINK_STATE_CARRYING or LINK_STATE_SWORD_SWINGING.
 * @return true if Link can swing his sword, false otherwise
 */
bool Link::can_start_sword(void) {
  return !zsdx->game->is_suspended() &&
    (state <= LINK_STATE_CARRYING || state == LINK_STATE_SWORD_SWINGING);
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
 * Lets Link loading his sword.
 * Moves to the state LINK_STATE_SWORD_LOADING
 * and updates the animations accordingly.
 */
void Link::start_sword_loading(void) {
  set_state(LINK_STATE_SWORD_LOADING);
  sword_loaded = false;

  // initialize the counter to detect when the sword is loaded
  counter = 0;
  next_counter_date = SDL_GetTicks();

  if (get_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * This function is called repeatedly while Link is loading his sword.
 * It stops the loading if the sword key is released.
 * The state must be LINK_STATE_SWORD_LOADING.
 */
void Link::update_sword_loading(void) {
  
  Uint8 *key_state = SDL_GetKeyState(NULL);
  
  Uint32 now = SDL_GetTicks();
  while (!sword_loaded && now >= next_counter_date) {
    counter++;
    next_counter_date += 100;
  }
  
  // detect when the sword is loaded (i.e. ready for a spin attack)
  if (!sword_loaded && counter >= 10) {
      zsdx->game_resource->get_sound("sword_spin_attack_load")->play();
      sword_loaded = true;
      counter = 0;
  }
  
  if (!key_state[SDLK_c]) {
    // the player just released the sword key

    // stop loading the sword, go to normal state or spin attack
    if (!sword_loaded) {
      // the sword was not loaded yet: go to the normal state
      start_free();
      get_movement()->compute_movement(); // because the direction was locked
    }
    else {
      // the sword is loaded: release a spin attack
      start_spin_attack();
    }
  }
}

/**
 * Lets Link loading his sword.
 * Moves to the state LINK_STATE_SWORD_LOADING
 * and updates the animations accordingly.
 */
void Link::start_spin_attack(void) {
  set_state(LINK_STATE_SPIN_ATTACK);
  sword_loaded = false;
  
  // play the sound
  zsdx->game_resource->get_sound("sword_spin_attack_release")->play();

  // start the animation
  tunic_sprite->set_current_animation("spin_attack");
  sword_sprite->set_current_animation("spin_attack");
  sword_stars_sprite->stop_animation();

  if (equipment->has_shield()) {
    shield_sprite->stop_animation(); // the shield is not visible during a spin attack
  }
}

/**
 * Returns whether Link's sword is currently displayed on the screen.
 * @return true if Link's sword is currently displayed on the screen
 */
bool Link::is_sword_visible(void) {
  return equipment->has_sword() && sword_sprite->is_animation_started();
}

/**
 * Returns whether the stars of Link's sword are currently displayed on the screen.
 * @return true if the stars of Link's sword are currently displayed on the screen
 */
bool Link::is_sword_stars_visible(void) {
  return equipment->has_sword() && sword_stars_sprite->is_animation_started();
}

/**
 * Returns whether Link's shield is currently displayed on the screen.
 * @return true if Link's shield is currently displayed on the screen
 */
bool Link::is_shield_visible(void) {
  return equipment->has_shield() && shield_sprite->is_animation_started();
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
 * This function is called when an animation of Link's sprite is over.
 * @param sprite the sprite
 */
void Link::animation_over(AnimatedSprite *sprite) {

  Uint8 *key_state;

  int state = get_state();
  switch (state) {

  case LINK_STATE_SWORD_SWINGING:
    
    // if the player is still pressing the sword key, set the "sword loading" animation

    key_state = SDL_GetKeyState(NULL);

    if (key_state[SDLK_c]) {
      start_sword_loading();
    }
    else {
      start_free();
    }
    
    break;
    
  case LINK_STATE_SPIN_ATTACK:
    start_free();
    break;

  }
}

/**
 * Returns whether the animation direction is locked.
 * When this function returns false, which is the case most of the time,
 * it means that the animation direction is set to the movement direction.
 * When it returns false, it means that the animation direction is fixed
 * and do not depends on the movement direction anymore (this is the case
 * when Link is loading his sword).
 * @return true if the animation direction is locked
 */
bool Link::is_direction_locked(void) {
  return state == LINK_STATE_SWORD_LOADING;
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

  if (is_sword_stars_visible()) {
    sword_stars_sprite->set_current_animation_direction(direction);
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
    sword_stars_sprite->set_suspended(suspended);
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

  int direction = tunic_sprite->get_current_animation_direction();
  
  switch (get_state()) {
    
  case LINK_STATE_FREE:

    stop_displaying_sword();

    if (equipment->has_shield()) {
      
      tunic_sprite->set_current_animation("stopped_with_shield");
      
      shield_sprite->set_current_animation("stopped");
      shield_sprite->set_current_animation_direction(direction);
    }
    else {
      tunic_sprite->set_current_animation("stopped");
    }

    break;
    
  case LINK_STATE_SWORD_LOADING:

    tunic_sprite->set_current_animation("sword_loading_stopped");

    sword_sprite->set_current_animation("sword_loading_stopped");
    sword_sprite->set_current_animation_direction(direction);
    
    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_animation_direction(direction);

    if (equipment->has_shield()) {

      if (direction % 2 != 0) {
	shield_sprite->set_current_animation("sword_loading_stopped");
	shield_sprite->set_current_animation_direction(direction / 2);
      }
      else {
	shield_sprite->stop_animation();
      }
    }

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
  
  int direction = tunic_sprite->get_current_animation_direction();
  
  switch (get_state()) {
    
  case LINK_STATE_FREE:
    
    stop_displaying_sword();

    if (equipment->has_shield()) {

      tunic_sprite->set_current_animation("walking_with_shield");
      
      shield_sprite->set_current_animation("walking");
      shield_sprite->set_current_animation_direction(direction);
    }
    else {
      tunic_sprite->set_current_animation("walking");
    }

    break;

  case LINK_STATE_SWORD_LOADING:
    
    tunic_sprite->set_current_animation("sword_loading_walking");

    sword_sprite->set_current_animation("sword_loading_walking");
    sword_sprite->set_current_animation_direction(direction);

    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_animation_direction(direction);

    if (equipment->has_shield()) {

      if (direction % 2 != 0) {
	shield_sprite->set_current_animation("sword_loading_walking");
	shield_sprite->set_current_animation_direction(direction / 2);
      }
      else {
	shield_sprite->stop_animation();
      }
    }

    break;

  default:
    break;
  }

  walking = true;
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
  sword_stars_sprite->stop_animation();

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
