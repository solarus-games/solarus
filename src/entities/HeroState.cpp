/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/Hero.h"
#include "entities/CarriedItem.h"
#include "entities/Teletransporter.h"
#include "movements/PlayerMovement.h"
#include "movements/StraightMovement.h"
#include "movements/JumpMovement.h"
#include "movements/TargetMovement.h"
#include "movements/PathMovement.h"
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
using namespace std;

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
    get_normal_movement()->set_moving_enabled(state < PUSHING, state <= GRABBING);
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
  return state == SWORD_LOADING || state == PUSHING || state == PULLING;
}

/**
 * Sets the ground displayed under the hero.
 * @param ground the ground to display
 */
void Hero::set_ground(Ground ground) {

  if (ground != this->ground) {
    this->ground = ground;
  }
}

/**
 * Starts displaying the ground specified by the last set_ground() call.
 */
void Hero::start_ground(void) {

  switch (ground) {

  case GROUND_NORMAL:
    // normal ground: remove any special sprite displayed under the hero
    delete ground_sprite;
    ground_sprite = NULL;
    break;

  case GROUND_DEEP_WATER:
    // deep water: plunge if the hero is not jumping
    if (state != JUMPING && state != HURT) {
      start_deep_water();
    }
    break;

  case GROUND_HOLE:

    if (state != JUMPING) {

      start_falling();

//        TODO?
//       // make the hero fall if he really is on the hole (otherwise restore the normal ground)
//       const SDL_Rect &center = get_center_point();
//       bool on_hole = (center.y / 8 == get_y() / 8); /* consider that the hero is on the hole
// 						     * if its center point is on the same 8*8 square
// 						     * than its origin point */

//       if (on_hole) {
// 	start_falling();
//       }
//       else {
// 	ground = GROUND_NORMAL;
// 	start_ground();
//       }

    }
    break;

  case GROUND_SHALLOW_WATER:
  case GROUND_GRASS:
    // display a special sprite below the hero
    ground_sprite = new Sprite(ground_sprite_ids[ground - 1]);
    ground_sprite->set_current_animation(walking ? "walking" : "stopped");

    ground_sound = ResourceManager::get_sound(ground_sound_ids[ground - 1]);
    next_ground_sound_date = MAX(next_ground_sound_date, SDL_GetTicks());
    break;

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

  return (ground == GROUND_GRASS || ground == GROUND_SHALLOW_WATER)
    && state != PLUNGING && state != SWIMMING && state != JUMPING
    && state != HURT && state != RETURNING_TO_SOLID_GROUND;
}

/**
 * This function is called when a teletransporter detects a collision with the hero.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void Hero::collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode) {

  if (state != JUMPING) {

    destroy_carried_items();
    stop_displaying_sword();

    if (state != FREE) {
      start_free();
    }
    teletransporter->transport_hero(this);
  }
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle.
 * This depends on the hero's state.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for the hero
 */
bool Hero::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return state > SWIMMING;
}

/**
 * Returns whether a water tile is currently considered as an obstacle for the hero.
 * @return true if the water tiles are currently an obstacle for the hero
 */
bool Hero::is_water_obstacle(void) {
  return false;
}

/**
 * Returns whether a hole is currently considered as an obstacle for the hero.
 * @return true if the holes are currently an obstacle for the hero
 */
bool Hero::is_hole_obstacle(void) {
  return false;
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
 * Lets the hero load his sword.
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

  // action icon "throw"
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
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

  // remove the "throw" icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

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
  else { // no more lifted item: remove the "throw" icon if it is still here
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_THROW) {
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    }
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
 * Makes the hero push something.
 * Moves to the state PUSHING and updates the animations accordingly.
 */
void Hero::start_pushing(void) {
  set_state(PUSHING);
  set_animation_pushing();
  pushing_direction_mask = get_normal_movement()->get_direction_mask();
}

/**
 * Updates the hero pushing, whether
 * his state is already PUSHING or not.
 */
void Hero::update_pushing(void) {

  // no change when the game is suspended
  if (zsdx->game->is_suspended()) {
    return;
  }

  // get the direction of the arrows
  Uint16 direction_mask = get_normal_movement()->get_direction_mask();

  if (state == FREE && move_tried) {
    // the hero is trying to move with animation "walking"

    // see if the move has failed (i.e. if the hero's coordinates have not changed)
    if (get_x() == old_x && get_y() == old_y) {

      // the hero is facing an obstacle

      Uint32 now = SDL_GetTicks();
      if (pushing_direction_mask == 0xFFFF) { // we start counting to trigger animation "pushing"
	counter = 0;
	next_counter_date = now;
	pushing_direction_mask = direction_mask;
      }

      // increment the counter every 100 ms
      while (now >= next_counter_date && counter < 8) {
	counter++;
	next_counter_date += 100;
      }

      if (counter >= 8) {
	start_pushing(); // start animation "pushing" when the counter reaches 8
      }
    }
    else {
      // the hero has just moved successfuly: reset the counter
      counter = 0;
      pushing_direction_mask = 0xFFFF;
    }
  }
  else {

    // stop trying to push if the player changes his direction
    if (pushing_direction_mask != 0xFFFF && // the hero is pushing or about to push
	direction_mask != pushing_direction_mask) {

      // reset the counter
      counter = 0;
      pushing_direction_mask = 0xFFFF;
    }

    // if the hero is pushing an obstacle
    if (state == PUSHING && !is_moving_grabbed_entity()) {

      int straight_direction = get_animation_direction();

      // stop pushing if there is no more obstacle
      if (!is_facing_obstacle()) {
	start_free();
      }

      // stop pushing if the player changes his direction
      if (get_movement_direction() != straight_direction * 90) {
	start_grabbing();
      }

      // see if the obstacle is an entity that the hero can push
      else if (facing_entity != NULL && grabbed_entity == NULL) {

	if (facing_entity->get_type() == BLOCK) {
	  try_snap_to_facing_entity();
	}

	if (facing_entity->moved_by_hero()) {

	  string path = "  ";
	  int direction = get_animation_direction();
	  path[0] = path[1] = '0' + direction * 2;

	  set_movement(new PathMovement(map, path, 8, false, true));
	  grabbed_entity = facing_entity;
	}
      }
    }
  }
}

/**
 * Makes the hero grab the object he is facing.
 * @param delay delay before the hero can push or pull 
 * the entity he is facing (if any)
 */
void Hero::start_grabbing(void) {
  stop_displaying_sword();
  set_state(GRABBING);
  set_animation_grabbing();
  grabbed_entity = NULL;
}

/**
 * Makes the hero pull the object he is grabbing.
 */
void Hero::start_pulling(void) {
  set_state(PULLING);
  set_animation_pulling();
}

/**
 * This function is called repeatedly while the hero is grabbing something.
 * The state must be GRABBING or PULLING.
 */
void Hero::update_grabbing_pulling(void) {

  Controls *controls = zsdx->game->get_controls();

  // the hero is grabbing an obstacle
  if (state == GRABBING) {
    int keys_direction = get_normal_movement()->get_direction();
    int sprite_direction = get_animation_direction() * 90;

    // push the obstacle
    if (keys_direction == sprite_direction) {
      start_pushing();
    }

    // pull the obstacle
    else if (keys_direction == (sprite_direction + 180) % 360) {
      start_pulling();
    }

    // release the obstacle
    if (!is_moving_grabbed_entity()) {
      if (!controls->is_key_pressed(Controls::ACTION)) {
	start_free();
      }
    }
  }

  // the hero is pulling an obstacle
  if (state == PULLING && !is_moving_grabbed_entity()) {

    int opposite_direction = (get_animation_direction() + 2) % 4;

    // stop pulling the obstacle if the player changes his direction
    if (get_movement_direction() != opposite_direction * 90) {
      start_grabbing();
    }

    // stop pulling if the action key is released or if there is no more obstacle
    if (!controls->is_key_pressed(Controls::ACTION) ||
	!is_facing_obstacle()) {
      start_free();
    }

    // see if the obstacle is an entity that the hero can pull
    if (facing_entity != NULL) {

      if (facing_entity->get_type() == BLOCK) {
	try_snap_to_facing_entity();
      }

      if (facing_entity->moved_by_hero()) {

	string path = "  ";
	int opposite_direction = (get_animation_direction() + 2) % 4;
	path[0] = path[1] = '0' + opposite_direction * 2;

	set_movement(new PathMovement(map, path, 8, false, true));
	grabbed_entity = facing_entity;
      }
    }
  }
}

/**
 * This function is called repeatedly in order to update the hero when he
 * is pushing or pulling the entity he is grabbing.
 */
void Hero::update_moving_grabbed_entity(void) {

  if (is_moving_grabbed_entity()) {

    PathMovement *movement = (PathMovement*) get_movement();

    // detect when the hero movement is finished
    // because the hero has covered 16 pixels or has reached an obstacle
    if (movement->is_finished()) {
      stop_moving_grabbed_entity();
    }
  }
}

/**
 * Returns whether the hero is moving the entity he is grabbing.
 * @return true if the hero is moving the entity he is grabbing
 */
bool Hero::is_moving_grabbed_entity(void) {
  return grabbed_entity != NULL;
}

/**
 * Returns whether the hero is in state GRABBING or PULLING.
 * @return true if the state is GRABBING or PULLING
 */
bool Hero::is_grabbing_or_pulling(void) {
  return state == GRABBING || state == PULLING;
}

/**
 * Notifies the hero that the entity he is pushing or pulling
 * cannot move any more because of a collision.
 */
void Hero::grabbed_entity_collision(void) {

  // the hero has moved one pixel too much
  // because he moved before the block, not knowing that the block would not follow him

  int direction_back = get_animation_direction();

  if (state == PUSHING) {
    direction_back = (direction_back + 2) % 4;
  }

  // go back one pixel in that direction
  switch (direction_back) {

  case 0:
    set_x(get_x() + 1);
    break;

  case 1:
    set_y(get_y() - 1);
    break;

  case 2:
    set_x(get_x() - 1);
    break;

  case 3:
    set_y(get_y() + 1);
    break;

  }

  stop_moving_grabbed_entity();
}

/**
 * Makes the hero stop pushing or pulling the entity he is grabbing.
 * This function is called while moving the entity, when the 
 * hero or the entity collides with an obstacle or when
 * the hero's movement is finished.
 */
void Hero::stop_moving_grabbed_entity(void) {
  clear_movement();
  set_movement(normal_movement);

  Controls *controls = zsdx->game->get_controls();
  if (state == PUSHING && !controls->is_key_pressed(Controls::ACTION)) {
    // the hero was pushing an entity without grabbing it
    grabbed_entity = NULL;

    // stop the animation pushing if his direction changed
    int straight_direction = get_animation_direction();
    if (get_movement_direction() != straight_direction * 90) {
      start_free();
    }
  }
  else {
    start_grabbing();
  }
}

/**
 * Forbids the hero to move until unfreeze() is called.
 * The current animation of the hero's sprites is stopped and the "stopped" animation is played.
 */
void Hero::freeze(void) {
  get_normal_movement()->set_moving_enabled(false, false);
  set_animation_stopped();
  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  set_state(FREEZED);
}

/**
 * Lets the hero move again after a freeze() call.
 */
void Hero::unfreeze(void) {
  start_free();
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
    restore_animation_direction();
    start_free();
  }
}

/**
 * Displays the treasure the hero is brandishing.
 */
void Hero::display_treasure(void) {

  int x = position_in_map.x;
  int y = position_in_map.y;

  const SDL_Rect &camera_position = map->get_camera_position();
  treasure->display(map->get_visible_surface(),
		    x - camera_position.x,
		    y - 24 - camera_position.y);
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

    if (ground == GROUND_DEEP_WATER) {
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
    if (state == LIFTING || state == CARRYING) {
      start_throwing();
    }
    stop_displaying_sword();

    ResourceManager::get_sound("hero_hurt")->play();

    int life_removed = MAX(1, life / (equipment->get_tunic() + 1));

    equipment->remove_hearts(life_removed);
    blink();
    set_state(HURT);
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

    if (ground == GROUND_DEEP_WATER) {
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
  return state != HURT && state != PLUNGING && state != FALLING && state != RETURNING_TO_SOLID_GROUND;
}

/**
 * This function is called when the hero was dead but saved by a fairy.
 */
void Hero::get_back_from_death(void) {
  blink();
  start_free();
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
 * @param sprite_overlapping the hero sprite that collides with the enemy
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
      ((JumpMovement*) get_movement())->set_delay(13);
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

    if (ground != GROUND_DEEP_WATER) {
      start_free();
    }
    else if (equipment->has_inventory_item(InventoryItem::FLIPPERS)) {
      start_swimming();
    }
    else {
      ResourceManager::get_sound("message_end")->play();
      start_returning_to_solid_ground(last_solid_ground_coords);
    }
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

/**
 * Makes the hero fall into a hole.
 */
void Hero::start_falling(void) {

  // if the hero is being hurt, stop the movement to make him fall
  if (state == HURT) {
    // TODO delete movement later
  }

  // remove the carried item
  else if (state == CARRYING) {
    start_throwing();
  }

  set_state(FALLING);
  set_animation_falling();
  ResourceManager::get_sound("hero_falls")->play();
}

/**
 * Updates the FALLING state.
 */
void Hero::update_falling(void) {

  if (tunic_sprite->is_animation_finished()) {

    start_returning_to_solid_ground(last_solid_ground_coords);
    equipment->remove_hearts(1);
    set_animation_stopped();
    restore_animation_direction();
  }
  else if (get_movement() != normal_movement) {
    // delete a possible hurt movement
    clear_movement();
    set_movement(normal_movement);
  }
}


/**
 * Hides the hero and makes him move back to the specified solid ground location.
 * @param target coordinates of the solid ground location
 */
void Hero::start_returning_to_solid_ground(const SDL_Rect &target) {
  set_movement(new TargetMovement(target.x, target.y, 12));
  set_state(RETURNING_TO_SOLID_GROUND);
}

/**
 * Updates the RETURNING_TO_SOLID_GROUND state.
 */
void Hero::update_returning_to_solid_ground(void) {

  TargetMovement *movement = (TargetMovement*) get_movement();
  movement->update();

  if (movement->is_finished()) {
    clear_movement();
    set_movement(normal_movement);
    blink();
    start_free();
  }
}
