#include "entities/Link.h"
#include "entities/TransportableItem.h"
#include "entities/CarriedItem.h"
#include "entities/EntityDetector.h"
#include "movements/Movement8ByPlayer.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Equipment.h"
#include "Map.h"
#include "Sound.h"
#include "KeysEffect.h"
#include "Treasure.h"

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
const SpriteAnimationsId Link::tunic_sprite_ids[3] = {
  "link/tunic0", // green tunic
  "link/tunic1", // blue tunic
  "link/tunic2", // red tunic
};

/**
 * String constants corresponding to the sprites of Link's swords.
 */
const SpriteAnimationsId Link::sword_sprite_ids[4] = {
  "link/sword1",
  "link/sword2",
  "link/sword3",
  "link/sword4",
};

/**
 * String constants corresponding to the sprites of the stars of Link's swords.
 */
const SpriteAnimationsId Link::sword_stars_sprite_ids[4] = {
  "link/sword_stars1",
  "link/sword_stars1",
  "link/sword_stars2",
  "link/sword_stars2",
};

/**
 * String constants corresponding to the sprites of the shields.
 */
const SpriteAnimationsId Link::shield_sprite_ids[3] = {
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
Link::Link(Equipment *equipment):
  map(NULL), equipment(equipment),
  tunic_sprite(NULL), sword_sprite(NULL), sword_stars_sprite(NULL), shield_sprite(NULL),
  state(FREE), facing_entity(NULL), counter(0), next_counter_date(0),
  walking(false), pushing_direction_mask(0xFFFF),
  lifted_item(NULL), thrown_item(NULL), treasure(NULL) {

  set_size(16, 16);
  set_origin(8, 13);
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

  destroy_carried_items();
}

/**
 * Returns whether this entity is the hero
 * controlled by the player.
 * @return true
 */
bool Link::is_hero(void) {
  return true;
}

/**
 * Returns Link's movement.
 * @return Link's movement
 */
Movement8ByPlayer * Link::get_movement(void) {
  return (Movement8ByPlayer*) movement;
}

/**
 * Returns the point Link is looking at.
 * @return the point Link is looking at
 */
SDL_Rect Link::get_facing_point(void) {

 int direction = tunic_sprite->get_current_direction();
  SDL_Rect facing_point;

  switch (direction) {

    // right
  case 0:
    facing_point.x = position_in_map.x + 16;
    facing_point.y = position_in_map.y + 8;
    break;

    // up
  case 1:
    facing_point.x = position_in_map.x + 8;
    facing_point.y = position_in_map.y - 1;
    break;

    // left
  case 2:
    facing_point.x = position_in_map.x - 1;
    facing_point.y = position_in_map.y + 8;
    break;

    // down
  case 3:
    facing_point.x = position_in_map.x + 8;
    facing_point.y = position_in_map.y + 16;
    break;
  }

  return facing_point;
}

/**
 * Sets Link's current map.
 * This function is called when the map is changed.
 * @param map the map
 * @param initial_direction the direction of Link (0 to 3)
 */
void Link::set_map(Map *map, int initial_direction) {

  this->map = map;

  get_movement()->set_map(map);
  
  stop_displaying_sword();

  // remove the "throw" (or other) icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(ACTION_KEY_NONE);

  // stop loading the sword or carrying an item
  switch (get_state()) {

  case CARRYING:
  case SWORD_LOADING:
    start_free();
    break;

  default:
    break;

  }

  // destroy any carried item from the previous map
  destroy_carried_items();

  // take the specified direction
  set_animation_direction(initial_direction);
}

/**
 * Updates Link's position, movement and animation.
 * This function is called repeteadly by the game.
 */
void Link::update(void) {

  // update the movement
  if (!zsdx->game->is_suspended()) {
    get_movement()->set_moving_enabled(get_state() <= SWIMMING);
    
    // specific updates in some states
    if (get_state() == SWORD_LOADING) {
      update_sword_loading();
    }
  }

  update_position();
  update_sprites();

  update_carried_items();

  if (treasure != NULL) {
    update_treasure();
  }
}

/**
 * Displays Link on the map with its current animation and
 * at its current position.
 * @param map the map
 */
void Link::display_on_map(Map *map) {

  int x = get_x();
  int y = get_y();

  map->display_sprite(tunic_sprite, x, y);

  if (is_sword_visible()) {
    map->display_sprite(sword_sprite, x, y);
  }

  if (is_sword_stars_visible()) {
    map->display_sprite(sword_stars_sprite, x, y);
  }

  if (is_shield_visible()) {
    map->display_sprite(shield_sprite, x, y);
  }

  display_carried_items();

  if (treasure != NULL) {
    display_treasure();
  }
}

/**
 * Initializes the sprites of Link and his equipment,
 * depending on its equipment as specified in the savegame.
 * This function is called at the game beginning
 * and when Link's equipment is changed.
 */
void Link::initialize_sprites(void) {

  int animation_direction = -1;

  // Link
  if (tunic_sprite != NULL) {
    // save the animation direction
    animation_direction = tunic_sprite->get_current_direction();
    delete tunic_sprite;
  }

  int tunic_number = equipment->get_tunic();

  tunic_sprite = new Sprite(tunic_sprite_ids[tunic_number]);
  tunic_sprite->set_animation_listener(this); // to be notified when an animation of Link is over

  // Link's sword
  if (sword_sprite != NULL) {
    delete sword_sprite;
    delete sword_stars_sprite;
    sword_sprite = NULL;
  }

  int sword_number = equipment->get_sword();

  if (sword_number > 0) {
    // Link has a sword: get the sprite and the sound
    sword_sprite = new Sprite(sword_sprite_ids[sword_number - 1]);
    sword_sprite->stop_animation();

    sword_sound = ResourceManager::get_sound(sword_sound_ids[sword_number - 1]);

    sword_stars_sprite = new Sprite(sword_stars_sprite_ids[sword_number - 1]);
    sword_stars_sprite->stop_animation();
  }

  // Link's shield
  if (shield_sprite != NULL) {
    delete shield_sprite;
    shield_sprite = NULL;
  }

  int shield_number = equipment->get_shield();

  if (shield_number > 0) {
    // Link has a shield
    shield_sprite = new Sprite(shield_sprite_ids[shield_number - 1]);
  }

  // restore the animation direction
  if (animation_direction != -1) {
    set_animation_direction(animation_direction);
  }

  // animation walking or stopped
  set_state(FREE);
  if (get_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * Updates the animation of Link's sprites if necessary.
 */
void Link::update_sprites(void) {

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

  if (state == CARRYING && walking) {
    lifted_item->get_last_sprite()->set_current_frame(tunic_sprite->get_current_frame() % 3);
  }
}

/**
 * Updates Link depending on the arrows pressed.
 * This function is called when Link's direction changes (typically, because the player
 * pressed or released an arrow). It updates Link's animations and collisions according to the new movement.
 */
void Link::movement_just_changed(void) {

  // update the animation direction according to the movement direction
  int direction = get_direction();
  if (direction != -1) {

    Uint16 direction_mask = get_movement()->get_direction_mask();
    int old_animation_direction = tunic_sprite->get_current_direction();
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

  // check the collisions
  if (map != NULL && !zsdx->game->is_suspended()) {
    just_moved();
  }
}

/**
 * This function is called when Link's position is changed,
 * or when his direction changes. 
 */
void Link::just_moved(void) {

  set_facing_entity(NULL);
  MapEntity::just_moved();
}

/**
 * Sets the entity Link is currently facing.
 * This function is called when Link is just being
 * facing another entity.
 * @param detector the detector Link is facing
 */
void Link::set_facing_entity(EntityDetector *detector) {

  this->facing_entity = detector;

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  ActionKeyEffect action_key_effect = keys_effect->get_action_key_effect();

  // if Link stops facing an entity that showed an action icon
  if (facing_entity == NULL &&
      (action_key_effect == ACTION_KEY_LIFT
       || action_key_effect == ACTION_KEY_OPEN)) {

    keys_effect->set_action_key_effect(ACTION_KEY_NONE);
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

  if (state == FREE && move_tried) {
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

      if (state == PUSHING) {
	start_free();
      }
    }
  }
}

/**
 * Returns Link's state.
 * @return the state of Link
 */
Link::State Link::get_state(void) {
  return state;
}

/**
 * Sets Link's state.
 * @param state the state of Link
 */
void Link::set_state(State state) {
  this->state = state;
  get_movement()->set_moving_enabled(state <= SWIMMING);
}

/**
 * Lets Link can walk.
 * Moves to the state FREE and updates the animations accordingly.
 */
void Link::start_free(void) {
  set_state(FREE);
  
  if (get_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }

  // to check the facing entity
  just_moved();
}

/**
 * Lets Link swinging his sword if this action is possible.
 * The game should not be suspended.
 * Moves to the state SWORD_SWINGING, plays the sword sound
 * and updates the animations accordingly.
 */
void Link::start_sword(void) {
  if (can_start_sword()) {

    // remove the carried item
    if (state == CARRYING) {
      start_throwing();
    }

    set_state(SWORD_SWINGING);
    sword_sound->play();
    set_animation_sword();
  }
}

/**
 * Returns whether Link can swing his sword right now.
 * The function returns true if the game is not suspended and Link
 * is in state FREE, PUSHING,
 * CARRYING or SWORD_SWINGING.
 * @return true if Link can swing his sword, false otherwise
 */
bool Link::can_start_sword(void) {
  return !zsdx->game->is_suspended() &&
    (state <= CARRYING || state == SWORD_SWINGING);
}

/**
 * Makes Link push something.
 * Moves to the state FREE and updates the animations accordingly.
 */
void Link::start_pushing(void) {
  set_state(PUSHING);
  set_animation_pushing();
}

/**
 * Lets Link loading his sword.
 * Moves to the state SWORD_LOADING
 * and updates the animations accordingly.
 */
void Link::start_sword_loading(void) {
  set_state(SWORD_LOADING);
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
 * The state must be SWORD_LOADING.
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
      ResourceManager::get_sound("sword_spin_attack_load")->play();
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
 * Makes Link lift a transportable item.
 * @param item_to_lift the transportable item to lift
 */
void Link::start_lifting(TransportableItem *item_to_lift) {

  // create the corresponding carried item
  this->lifted_item = new CarriedItem(this, item_to_lift);

  zsdx->game->get_keys_effect()->set_action_key_effect(ACTION_KEY_THROW);
  set_state(LIFTING);
  set_animation_lifting();
  set_facing_entity(NULL);
}

/**
 * Makes Link carry the item he was lifting.
 */
void Link::start_carrying(void) {
  set_state(CARRYING);

  if (get_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * Makes Link throw the item he was carrying.
 */
void Link::start_throwing(void) {

  // we check the state because the "throw" icon is actually shown as soon as
  // Link starts lifting the item
  if (state == CARRYING) {

    lifted_item->throw_item(map, get_animation_direction());

    if (thrown_item != NULL) {
      delete thrown_item;
    }

    thrown_item = lifted_item;
    lifted_item = NULL;
    start_free();
  }
}

/**
 * Suspends or resumes the animation of Link's carried items.
 * This function is called when the game is suspended or resumed.
 * @param suspended true to suspend the game, false to resume it
 */
void Link::set_suspended_carried_items(bool suspended) {
  
  if (lifted_item != NULL) {
    lifted_item->set_suspended(suspended);
  }

  if (thrown_item != NULL) {
    thrown_item->set_suspended(suspended);
  }
}

/**
 * Updates the carried items.
 */
void Link::update_carried_items(void) {

  if (lifted_item != NULL) {
    lifted_item->update();
  }

  if (thrown_item != NULL) {
    thrown_item->update();

    if (thrown_item->is_broken()) {
      delete thrown_item;
      thrown_item = NULL;
    }
  }
}

/**
 * Displays the carried items.
 */
void Link::display_carried_items(void) {

  if (lifted_item != NULL) {
    lifted_item->display_on_map(map);
  }
  
  if (thrown_item != NULL) {
    thrown_item->display_on_map(map);
  }
}

/**
 * Deletes the carried items.
 */
void Link::destroy_carried_items(void) {

  if (lifted_item != NULL) {
    delete lifted_item;
    lifted_item = NULL;
  }

  if (thrown_item != NULL) {
    delete thrown_item;
    thrown_item = NULL;
  }
}

/**
 * Forbids Link to move until start_free() is called.
 * The current animation of Link's sprites is stopped and the "stopped" animation is played.
 */
void Link::freeze(void) {
  get_movement()->set_moving_enabled(false);
  set_animation_stopped();
  set_state(FREEZED);
}

/**
 * Makes Link brandish a treasure.
 * @param treasure the treasure to give him (will be deleted after Link brandishes it) 
 */
void Link::give_treasure(Treasure *treasure) {

  this->treasure = treasure;

  // goto the right state
  set_state(BRANDISHING_TREASURE);

  // show the animation
  save_animation_direction();
  tunic_sprite->set_current_animation("brandish");
  tunic_sprite->set_current_direction(0);

  // the shield and the sword are not visible when Link is brandishing a treasure
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
  stop_displaying_sword();

  // give the treasure and show the message
  treasure->give_to_player();
}

/**
 * Updates Link when he is brandishing a treasure.
 */
void Link::update_treasure(void) {

  if (!zsdx->game->is_showing_message()) {

    /* The treasure message is over: if the treasure was a tunic,
     * a sword or a shield, then we must reload Link's sprites now
     */
    Treasure::Content content = treasure->get_content();
    if (content >= Treasure::BLUE_TUNIC && content <= Treasure::SWORD_4) {
      zsdx->game->get_link()->initialize_sprites();
    }

    // restore Link's state
    treasure = NULL;
    start_free();
    restore_animation_direction();
  }
}

/**
 * Displays the treasure Link is brandishing.
 */
void Link::display_treasure(void) {

  int x = position_in_map.x;
  int y = position_in_map.y;

  SDL_Rect *screen_position = map->get_screen_position();
  treasure->display(map->get_visible_surface(),
		    x - screen_position->x,
		    y - 24 - screen_position->y);
}

/**
 * Lets Link loading his sword.
 * Moves to the state SWORD_LOADING
 * and updates the animations accordingly.
 */
void Link::start_spin_attack(void) {
  set_state(SPIN_ATTACK);
  sword_loaded = false;
  
  // play the sound
  ResourceManager::get_sound("sword_spin_attack_release")->play();

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
void Link::animation_over(Sprite *sprite) {

  Uint8 *key_state;

  int state = get_state();
  switch (state) {

  case SWORD_SWINGING:
    
    // if the player is still pressing the sword key, set the "sword loading" animation

    key_state = SDL_GetKeyState(NULL);

    if (key_state[SDLK_c]) {
      start_sword_loading();
    }
    else {
      start_free();
    }
    
    break;
    
  case SPIN_ATTACK:
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
  return state == SWORD_LOADING;
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
 * Suspends or resumes the animation of Link's sprites.
 * This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the animation, false to resume it
 */
void Link::set_suspended(bool suspended) {

  // sprites
  tunic_sprite->set_suspended(suspended);

  if (equipment->has_sword() && sword_sprite != NULL) {
    sword_sprite->set_suspended(suspended);
    sword_stars_sprite->set_suspended(suspended);
  }

  if (equipment->has_shield() && shield_sprite != NULL) {
    shield_sprite->set_suspended(suspended);
  }

  // carried items
  set_suspended_carried_items(suspended);

  // movement
  if (suspended) {
    get_movement()->set_moving_enabled(false);
    next_counter_date = SDL_GetTicks();
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
 * This function is called by the engine when the action key is pressed.
 * Depending on its effect, an action may be performed.
 */
void Link::action_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  
  switch (keys_effect->get_action_key_effect()) {

  case ACTION_KEY_LIFT:
  case ACTION_KEY_OPEN:
    // action on the facing entity
    facing_entity->action_key_pressed();
    break;

  case ACTION_KEY_THROW:
    start_throwing();
    break;

  default:
    break;
  }
}

/**
 * This function is called by the engine when the sword key is pressed.
 * Depending on its effect, an action may be performed.
 */
void Link::sword_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  
  switch (keys_effect->get_sword_key_effect()) {

  case SWORD_KEY_SWORD:
    start_sword();
    break;

  default:
    break;
  }
}
