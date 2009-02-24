#include "entities/Hero.h"
#include "entities/CarriedItem.h"
#include "entities/Teletransporter.h"
#include "movements/PlayerMovement.h"
#include "movements/StraightMovement.h"
#include "movements/JumpMovement.h"
#include "movements/TargetMovement.h"
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
 * Returns the hero's state.
 * @return the state of the hero
 */
Hero::State Hero::get_state(void) {
  return state;
}

/**
 * Sets the hero's state.
 * @param state the state of the hero
 */
void Hero::set_state(State state) {
  this->state = state;

  if (!zsdx->game->is_suspended()) {
    get_normal_movement()->set_moving_enabled(state <= SWIMMING);
  }
}

/**
 * Returns whether the animation direction is locked.
 * When this function returns false, which is the case most of the time,
 * it means that the animation direction is set to the movement direction.
 * When it returns false, it means that the animation direction is fixed
 * and do not depends on the movement direction anymore (this is the case
 * when the hero is loading his sword).
 * @return true if the animation direction is locked
 */
bool Hero::is_direction_locked(void) {
  return state == SWORD_LOADING;
}

/**
 * Sets the ground displayed under the hero.
 * @param ground the ground to display, in enum Map::Ground (including Map::NORMAL_GROUND)
 */
void Hero::set_ground(int ground) {

  if (ground != this->ground) {
    this->ground = ground;
  }
}
/**
 * Starts displaying the ground specified by the last set_ground() call.
 */
void Hero::start_ground(void) {

  if (ground == Map::NORMAL_GROUND) {
    delete ground_sprite;
    ground_sprite = NULL;
  }
  else if (ground == Map::DEEP_WATER) {
    if (state != JUMPING && state != HURT) {
      start_deep_water();
    }
  }
  else {
    ground_sprite = new Sprite(ground_sprite_ids[ground - 1]);
    ground_sprite->set_current_animation(walking ? "walking" : "stopped");

    ground_sound = ResourceManager::get_sound(ground_sound_ids[ground - 1]);
    next_ground_sound_date = MAX(next_ground_sound_date, SDL_GetTicks());
  }
}

/**
 * Updates the ground displayed under the hero.
 */
void Hero::update_ground(void) {

  Uint32 now = SDL_GetTicks();
  if (now >= next_ground_sound_date) {

    if (walking) {
      ground_sound->play();
    }

    next_ground_sound_date = now + 300;
  }
}

/**
 * Returns whether the hero is in a state such that
 * a ground can be displayed under him.
 */
bool Hero::is_ground_visible(void) {

  return ground != Map::NORMAL_GROUND
    && ground != Map::DEEP_WATER
    && state != PLUNGING
    && state != SWIMMING
    && state != JUMPING
    && state != HURT
    && state != DROWNING;
}

/**
 * This function is called when a teletransporter detects a collision with the hero.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void Hero::collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode) {

  if (collision_mode == Detector::COLLISION_ORIGIN_POINT) {
    teletransporter->transport_hero(this);
  }
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle.
 * This depends on the hero's state.
 * @param teletransporter
 * @return true if the teletransporter is currently an obstacle for the hero
 */
bool Hero::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return state > SWIMMING && state != HURT;
}

/**
 * Lets the hero walk.
 * Moves to the state FREE and updates the animations accordingly.
 */
void Hero::start_free(void) {

  set_state(FREE);

  get_normal_movement()->compute_movement();

  if (get_normal_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }

  // to check the facing entity and the ground
  just_moved();
}

/**
 * Makes the hero swing his sword if this action is possible.
 * The game should not be suspended.
 * Moves to the state SWORD_SWINGING, plays the sword sound
 * and updates the animations accordingly.
 */
void Hero::start_sword(void) {
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
 * Returns whether the hero can swing his sword right now.
 * The function returns true if the game is not suspended and the hero
 * is in state FREE, PUSHING,
 * CARRYING or SWORD_SWINGING.
 * @return true if the hero can swing his sword, false otherwise
 */
bool Hero::can_start_sword(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();

  return !zsdx->game->is_suspended()
    && (state <= CARRYING || state == SWORD_SWINGING)
    && keys_effect->get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD;
}

/**
 * Makes the hero push something.
 * Moves to the state FREE and updates the animations accordingly.
 */
void Hero::start_pushing(void) {
  set_state(PUSHING);
  set_animation_pushing();
}

/**
 * Lets the hero loading his sword.
 * Moves to the state SWORD_LOADING
 * and updates the animations accordingly.
 */
void Hero::start_sword_loading(void) {
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
 * This function is called repeatedly while the hero is swinging his sword.
 * The state must be SWORD_SWINGING.
 */
void Hero::update_sword_swinging(void) {

  if (tunic_sprite->is_animation_finished()) {

    // if the player is still pressing the sword key, start loading the sword
    if (zsdx->game->get_controls()->is_key_pressed(Controls::SWORD)) {
      start_sword_loading();
    }
    else {
      start_free();
    }
  }
}

/**
 * This function is called repeatedly while the hero is loading his sword.
 * It stops the loading if the sword key is released.
 * The state must be SWORD_LOADING.
 */
void Hero::update_sword_loading(void) {

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
 * Makes the hero lift a destructible item.
 * @param item_to_lift the destructible item to lift
 */
void Hero::start_lifting(DestructibleItem *item_to_lift) {

  // create the corresponding carried item
  this->lifted_item = new CarriedItem(this, item_to_lift);
  lifted_item->set_map(map);

  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
  set_state(LIFTING);
  set_animation_lifting();
  set_facing_entity(NULL);
}

/**
 * Makes the hero carry the item he was lifting.
 */
void Hero::start_carrying(void) {
  set_state(CARRYING);

  if (get_normal_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * Removes the item carried by the hero, without throwing it.
 * Change the hero state after calling this function.
 */
void Hero::stop_carrying(void) {
  delete lifted_item;
  lifted_item = NULL;
  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
}

/**
 * Makes the hero throw the item he was carrying.
 */
void Hero::start_throwing(void) {

  // we check the state because the "throw" icon is actually shown as soon as
  // the hero starts lifting the item
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
 * Suspends or resumes the animation of the hero's carried items.
 * This function is called when the game is suspended or resumed.
 * @param suspended true to suspend the game, false to resume it
 */
void Hero::set_suspended_carried_items(bool suspended) {

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
void Hero::update_carried_items(void) {

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
void Hero::display_carried_items(void) {

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
void Hero::destroy_carried_items(void) {

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
 * Makes the hero grab the object he is facing.
 */
void Hero::start_grabbing(void) {
  stop_displaying_sword();
  set_state(GRABBING);
  set_animation_grabbing();
}

/**
 * Makes the hero pull the object he is grabbing.
 */
void Hero::start_pulling(void) {
  set_state(PULLING);
  set_animation_pulling();
}

/**
 * This function is called repeatedly while the hero is grabbing or pulling something.
 * It stops the action if the action key is released.
 * The state must be GRABBING or PULLING.
 */
void Hero::update_grabbing_pulling(void) {

  Controls *controls = zsdx->game->get_controls();
  if (!controls->is_key_pressed(Controls::ACTION)) {
    start_free();
  }
}

/**
 * Forbids the hero to move until start_free() is called.
 * The current animation of the hero's sprites is stopped and the "stopped" animation is played.
 */
void Hero::freeze(void) {
  get_normal_movement()->set_moving_enabled(false);
  set_animation_stopped();
  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  set_state(FREEZED);
}

/**
 * Makes the hero brandish a treasure.
 * @param treasure the treasure to give him (you have to delete it after the hero brandishes it)
 */
void Hero::give_treasure(Treasure *treasure) {

  this->treasure = treasure;

  // goto the right state
  stop_carrying();
  set_state(BRANDISHING_TREASURE);

  // show the animation
  save_animation_direction();
  tunic_sprite->set_current_animation("brandish");
  tunic_sprite->set_current_direction(0);

  // the shield and the sword are not visible when the hero is brandishing a treasure
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
  stop_displaying_sword();
}

/**
 * Updates the hero when he is brandishing a treasure.
 */
void Hero::update_treasure(void) {

  if (!zsdx->game->is_giving_treasure()) {

    /* The treasure message is over: if the treasure was a tunic,
     * a sword or a shield, then we must reload the hero's sprites now
     */
    Treasure::Content content = treasure->get_content();
    if (content >= Treasure::BLUE_TUNIC && content <= Treasure::SWORD_4) {
      rebuild_equipment();
    }

    // restore the hero's state
    treasure = NULL;
    start_free();
    restore_animation_direction();
  }
}

/**
 * Displays the treasure the hero is brandishing.
 */
void Hero::display_treasure(void) {

  int x = position_in_map.x;
  int y = position_in_map.y;

  SDL_Rect *camera_position = map->get_camera_position();
  treasure->display(map->get_visible_surface(),
		    x - camera_position->x,
		    y - 24 - camera_position->y);
}

/**
 * Makes the hero load his sword.
 * Moves to the state SWORD_LOADING
 * and updates the animations accordingly.
 */
void Hero::start_spin_attack(void) {
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
 * This function is called repeatedly during the spin attack.
 * The state must be SPIN_ATTACK.
 */
void Hero::update_spin_attack(void) {

  if (tunic_sprite->is_animation_finished()) {
    start_free();
  }
}
/**
 * Makes the hero jump in a direction.
 * While he is jumping, the player does not control him anymore.
 * @param direction direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param with_collisions true to stop the movement if there is a collision
 */
void Hero::start_jumping(int direction, int length, bool with_collisions) {

  // remove the carried item
  if (state == CARRYING) {
    start_throwing();
  }

  stop_displaying_sword();

  // jump
  set_state(JUMPING);
  set_movement(new JumpMovement(map, direction, length, with_collisions));
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
void Hero::update_jumping(void) {

  JumpMovement *movement = (JumpMovement*) get_movement();
  movement->update();
  jump_y = get_y() - movement->get_jump_height();

  if (movement->is_finished()) {

    clear_movement();
    set_movement(normal_movement);

    if (ground == Map::DEEP_WATER) {
      start_deep_water();
    }
    else {
      start_free();
    }
  }
}

/**
 * Returns whether the hero can be hurt.
 * @return true if the hero can be hurt in its current state
 */
bool Hero::can_be_hurt(void) {
  return state <= SPIN_ATTACK && !tunic_sprite->is_blinking();
}

/**
 * Hurts the hero if possible.
 * @param source the entity that hurts the hero (usually an enemy)
 * @param life number of heart quarters to remove (this number may be reduced by the tunic)
 */
void Hero::hurt(MapEntity *source, int life) {

  if (can_be_hurt()) {

    // remove the carried item
    if (state == CARRYING) {
      start_throwing();
    }
    stop_displaying_sword();

    ResourceManager::get_sound("hero_hurt")->play();

    int life_removed = MAX(1, life / (equipment->get_tunic() + 1));

    equipment->remove_hearts(life_removed);
    set_state(HURT);
    blink();
    set_animation_hurt();

    double angle = source->get_vector_angle(this);
    set_movement(new StraightMovement(map, 12, angle, 200));
  }
}

/**
 * Updates the HURT state.
 */
void Hero::update_hurt(void) {

  StraightMovement *movement = (StraightMovement*) get_movement();
  movement->update();

  if (movement->is_finished()) {

    clear_movement();
    set_movement(normal_movement);

    if (ground == Map::DEEP_WATER) {
      start_deep_water();
    }
    else {
      start_free();
    }
  }
}

/**
 * Returns whether the hero is in a state such that the game over
 * sequence can start.
 * @return true if the game over sequence can start
 */
bool Hero::can_start_gameover_sequence(void) {
  return state != HURT && state != PLUNGING;
}

/**
 * This function is called when the hero was dead but saved by a fairy.
 */
void Hero::get_back_from_death(void) {
  start_free();
  blink();
  when_suspended = SDL_GetTicks();
}

/**
 * This function is called when an enemy collides with the hero.
 * @param enemy the enemy
 */
void Hero::collision_with_enemy(Enemy *enemy) {
  enemy->attack_hero(this);
}

/**
 * This function is called when an enemy's sprite collides with a sprite of the hero.
 * @param enemy the enemy
 * @param sprite the hero sprite that collides with the enemy
 */
void Hero::collision_with_enemy(Enemy *enemy, Sprite *sprite_overlapping) {

  if (sprite_overlapping->get_animation_set_id().find("sword") != string::npos) {
    enemy->hurt(Enemy::ATTACK_SWORD, this);
  }
}

/**
 * Notifies the hero that he has just attacked an enemy
 * (even if this attack was not successful).
 * @param attack the attack
 * @param victim the enemy just hurt
 */
void Hero::just_attacked_enemy(Enemy::Attack attack, Enemy *victim) {

  switch (attack) {

  case Enemy::ATTACK_SWORD:
    if (get_state() == SWORD_LOADING) {
      stop_displaying_sword();
      start_free();
    }
    break;

  default:
    DIE("Unknown attack '" << attack << "'");
  }
}

/**
 * Makes the hero drown or swim.
 */
void Hero::start_deep_water(void) {

  // stop the sword
  stop_displaying_sword();

  // throw any carried item
  if (state == CARRYING) {
    start_throwing();
  }

  if (state == JUMPING || state == HURT) {
    // plunge into the water
    start_plunging();
  }
  else {
    // move to state swimming or jumping
    if (equipment->has_inventory_item(InventoryItem::FLIPPERS)) {
      start_swimming();
    }
    else {
      start_jumping(get_movement_direction() / 45, 32, true);
    }
  }
}

/**
 * Makes the hero plunge into the water.
 */
void Hero::start_plunging(void) {
  set_state(PLUNGING);
  set_animation_plunging();
  ResourceManager::get_sound("splash")->play();
}

/**
 * Updates the PLUNGING state.
 */
void Hero::update_plunging(void) {

  if (tunic_sprite->is_animation_finished()) {

    if (ground != Map::DEEP_WATER) {
      start_free();
    }
    else if (equipment->has_inventory_item(InventoryItem::FLIPPERS)) {
      start_swimming();
    }
    else {
      ResourceManager::get_sound("message_end")->play();
      set_movement(new TargetMovement(last_ground_x, last_ground_y, 12));
      set_state(DROWNING);
    }
  }
}

/**
 * Updates the DROWNING state.
 */
void Hero::update_drowning(void) {

  TargetMovement *movement = (TargetMovement*) get_movement();
  movement->update();

  if (movement->is_finished()) {
    clear_movement();
    set_movement(normal_movement);
    start_free();
    blink();
  }
}

/**
 * Makes the hero swim.
 */
void Hero::start_swimming(void) {
  set_state(SWIMMING);
  // TODO
}

/**
 * Makes the hero stop swimming.
 */
void Hero::stop_swimming(void) {
  start_free();
}
