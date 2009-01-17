#include "entities/Link.h"
#include "entities/CarriedItem.h"
#include "movements/Movement8ByPlayer.h"
#include "movements/JumpMovement.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Equipment.h"
#include "Sprite.h"
#include "Treasure.h"
#include "Controls.h"

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
  get_normal_movement()->set_moving_enabled(state <= SWIMMING);
}

/**
 * Lets Link can walk.
 * Moves to the state FREE and updates the animations accordingly.
 */
void Link::start_free(void) {
  set_state(FREE);

  get_normal_movement()->compute_movement();
  
  if (get_normal_movement()->is_started()) {
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

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();

  return !zsdx->game->is_suspended()
    && (state <= CARRYING || state == SWORD_SWINGING)
    && keys_effect->get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD;
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

  if (get_normal_movement()->is_started()) {
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

  Controls *controls = zsdx->game->get_controls();

  if (!controls->is_key_pressed(Controls::SWORD)) {
    // the player just released the sword key

    // stop loading the sword, go to normal state or spin attack
    if (!sword_loaded) {
      // the sword was not loaded yet: go to the normal state
      start_free();
    }
    else {
      // the sword is loaded: release a spin attack
      start_spin_attack();
    }
  }
}

/**
 * Makes Link lift a destructible item.
 * @param item_to_lift the destructible item to lift
 */
void Link::start_lifting(DestructibleItem *item_to_lift) {

  // create the corresponding carried item
  this->lifted_item = new CarriedItem(this, item_to_lift);
  lifted_item->set_map(map);

  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
  set_state(LIFTING);
  set_animation_lifting();
  set_facing_entity(NULL);
}

/**
 * Makes Link carry the item he was lifting.
 */
void Link::start_carrying(void) {
  set_state(CARRYING);

  if (get_normal_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * Removes the item carried by Link, without throwing it.
 */
void Link::stop_carrying(void) {
  set_state(FREE);
  delete lifted_item;
  lifted_item = NULL;
  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
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
    lifted_item->display_on_map();
  }
  
  if (thrown_item != NULL) {
    thrown_item->display_on_map();
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
 * Makes Link grab the object he is facing.
 */
void Link::start_grabbing(void) {
  set_state(GRABBING);
  set_animation_grabbing();
}

/**
 * Makes Link pull the object he is grabbing.
 */
void Link::start_pulling(void) {
  set_state(PULLING);
  set_animation_pulling();
}

/**
 * This function is called repeatedly while Link is grabbing or pulling something.
 * It stops the action if the action key is released.
 * The state must be GRABBING or PULLING.
 */
void Link::update_grabbing_pulling(void) {

  Controls *controls = zsdx->game->get_controls();
  if (!controls->is_key_pressed(Controls::ACTION)) {
    start_free();
  }
}

/**
 * Forbids Link to move until start_free() is called.
 * The current animation of Link's sprites is stopped and the "stopped" animation is played.
 */
void Link::freeze(void) {
  get_normal_movement()->set_moving_enabled(false);
  set_animation_stopped();
  set_state(FREEZED);
}

/**
 * Makes Link brandish a treasure.
 * @param treasure the treasure to give him (you have to delete it after Link brandishes it) 
 */
void Link::give_treasure(Treasure *treasure) {

  this->treasure = treasure;

  // goto the right state
  stop_carrying();
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
}

/**
 * Updates Link when he is brandishing a treasure.
 */
void Link::update_treasure(void) {

  if (!zsdx->game->is_giving_treasure()) {

    /* The treasure message is over: if the treasure was a tunic,
     * a sword or a shield, then we must reload Link's sprites now
     */
    Treasure::Content content = treasure->get_content();
    if (content >= Treasure::BLUE_TUNIC && content <= Treasure::SWORD_4) {
      rebuild_equipment();
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
 * Makes Link jump in a direction.
 * While he is jumping, the player does not control him anymore.
 * @param direction direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 */
void Link::start_jumping(int direction, int length) {

  // remove the carried item
  if (state == CARRYING) {
    start_throwing();
  }

  stop_displaying_sword();

  // jump
  set_state(JUMPING);
  set_movement(new JumpMovement(direction, length));
  set_animation_jumping();
  ResourceManager::get_sound("jump")->play();
  jump_y = get_y();

  if (get_layer() == LAYER_INTERMEDIATE) {
    set_layer(LAYER_LOW);
  }
}

/**
 * Updates the jump action.
 */
void Link::update_jumping(void) {

  JumpMovement *movement = (JumpMovement*) get_movement();
  movement->update();
  jump_y = get_y() - movement->get_jump_height();
  if (movement->is_finished()) {
    clear_movement();
    set_movement(normal_movement);
    start_free();
  }
}

/**
 * This function is called when an animation of Link's sprite is over.
 * @param sprite the sprite
 */
void Link::animation_over(Sprite *sprite) {

  Controls *controls = zsdx->game->get_controls();

  int state = get_state();
  switch (state) {

  case SWORD_SWINGING:
    
    // if the player is still pressing the sword key, set the "sword loading" animation

    if (controls->is_key_pressed(Controls::SWORD)) {
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
