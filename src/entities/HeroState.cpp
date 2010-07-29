/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/Hero.h"
#include "entities/HeroSprites.h"
#include "entities/CarriedItem.h"
#include "entities/Teletransporter.h"
#include "entities/MapEntities.h"
#include "entities/Enemy.h"
#include "entities/ConveyorBelt.h"
#include "entities/Stairs.h"
#include "entities/Sensor.h"
#include "entities/Explosion.h"
#include "movements/PlayerMovement.h"
#include "movements/StraightMovement.h"
#include "movements/JumpMovement.h"
#include "movements/TargetMovement.h"
#include "movements/PathMovement.h"
#include "Game.h"
#include "Map.h"
#include "MapScript.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "Sprite.h"
#include "Treasure.h"
#include "GameControls.h"
#include "InventoryItem.h"
#include "lowlevel/System.h"

/**
 * @brief Returns the hero's state.
 * @return the state of the hero
 */
Hero::State Hero::get_state(void) {
  return state;
}

/**
 * @brief Sets the hero's state.
 * @param state the state of the hero
 */
void Hero::set_state(State state) {

  if (state != this->state) {

    this->state = state;

    if (!game->is_suspended()) {
      get_normal_movement()->set_moving_enabled(state < PUSHING, state <= CONVEYOR_BELT);
      notify_just_moved();
    }
  }
}

/**
 * @brief Returns whether the animation direction is locked.
 *
 * When this function returns false, which is the case most of the time,
 * it means that the animation direction is set to the movement direction.
 * When it returns false, it means that the animation direction is fixed
 * and do not depends on the movement direction anymore (this is the case
 * when the hero is loading his sword).
 *
 * @return true if the animation direction is locked
 */
bool Hero::is_direction_locked(void) {
  return state == SWORD_LOADING || state == PUSHING || state == PULLING || state == SWORD_TAPPING;
}

/**
 * @brief Sets the ground displayed under the hero.
 * @param ground the ground to display
 */
void Hero::set_ground(Ground ground) {

  if (ground != this->ground) {
    this->ground = ground;
  }
}

/**
 * @brief Returns the ground displayed under the hero.
 * @return ground the ground under the hero
 */
Ground Hero::get_ground(void) {
  return ground;
}

/**
 * @brief Starts displaying the ground specified by the last call to set_ground().
 */
void Hero::start_ground(void) {

  switch (ground) {

  case GROUND_NORMAL:
    // normal ground: remove any special sprite displayed under the hero
    sprites->destroy_ground();
    get_normal_movement()->set_moving_speed(walking_speed);
    break;

  case GROUND_DEEP_WATER:
    // deep water: plunge if the hero is not jumping
    if (state != JUMPING && state != HURT) {
      start_deep_water();
    }
    break;

  case GROUND_HOLE:
    // hole: attract the hero towards the hole
    if (state != JUMPING) {
      start_hole();
    }
    break;

  case GROUND_SHALLOW_WATER:
  case GROUND_GRASS:
    {
      // display a special sprite below the hero
      sprites->create_ground(ground);

      uint32_t now = System::now();
      next_ground_date = std::max(next_ground_date, now);

      get_normal_movement()->set_moving_speed(walking_speed * 4 / 5);
    }
    break;

  case GROUND_LADDER:
    get_normal_movement()->set_moving_speed(walking_speed * 3 / 5);
    break;
  }

  if (state == SWIMMING && ground != GROUND_DEEP_WATER) {
    stop_swimming();
  }
}

/**
 * @brief Updates the ground under the hero.
 */
void Hero::update_ground(void) {

  // see if it's time to do something (depending on the ground)
  uint32_t now = System::now();
  if (now >= next_ground_date) {

    if (is_ground_visible()) {
      // time to play a sound
      if (sprites->is_walking() && state <= SWORD_LOADING) {
	sprites->play_ground_sound();
      }

      next_ground_date = now + 300;
    }

    if (ground == GROUND_HOLE &&
	state != FALLING && state != RETURNING_TO_SOLID_GROUND && state != JUMPING) {

      if (get_distance(last_solid_ground_coords.get_x(), last_solid_ground_coords.get_y()) >= 8) {
	start_falling();
      }
      else {

	// time to move the hero on a hole one more pixel away from the solid ground
	Rectangle collision_box = get_bounding_box();
	collision_box.add_xy(hole_dx, hole_dy);

	if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	  set_bounding_box(collision_box);
	  notify_just_moved();
	}
	else {
          collision_box = get_bounding_box();
	  collision_box.add_xy(hole_dx, 0);
	  if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	    set_bounding_box(collision_box);
	    notify_just_moved();
	  }
	  else {
	    collision_box = get_bounding_box();
	    collision_box.add_xy(0, hole_dy);
	    if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	      set_bounding_box(collision_box);
	      notify_just_moved();
	    }
	  }
	}

	next_ground_date = now + 60;
      }
    }
  }
}

/**
 * @brief Returns whether the hero is in a state such that
 * a ground can be displayed under him.
 */
bool Hero::is_ground_visible(void) {

  return (ground == GROUND_GRASS || ground == GROUND_SHALLOW_WATER)
    && state != PLUNGING && state != SWIMMING && state != JUMPING
    && state != HURT && state != RETURNING_TO_SOLID_GROUND;
}

/**
 * @brief This function is called when a teletransporter detects a collision with the hero.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode) {

  if (state != JUMPING && state != CONVEYOR_BELT) {

    bool on_hole = map->get_tile_ground(get_layer(), get_x(), get_y()) == GROUND_HOLE;

    if (state == STAIRS || get_stairs_overlapping() != NULL) {
      this->delayed_teletransporter = teletransporter; // take the stairs first, transport later
    }
    else if (on_hole) {
      this->delayed_teletransporter = teletransporter; // fall first, transport later
    }
    else {
      teletransporter->transport_hero(this); // usual case: transport right now
    }
  }
}

/**
 * @brief This function is called when a conveyor belt detects a collision with this entity.
 * @param conveyor_belt a conveyor belt
 * @param dx direction of the x move in pixels (0, 1 or -1)
 * @param dy direction of the y move in pixels (0, 1 or -1)
 */
void Hero::notify_collision_with_conveyor_belt(ConveyorBelt *conveyor_belt, int dx, int dy) {

  on_conveyor_belt = true;

  if (state != JUMPING && state != CONVEYOR_BELT && state != PUSHING) {

    // check that a significant part of the hero is on the conveyor belt
    Rectangle center = get_center_point();
    center.add_xy(-1, -1);
    center.set_size(2, 2);

    if (conveyor_belt->overlaps(center)) {

      // check that the movement is possible for at least one pixel
      Rectangle collision_box = get_bounding_box();
      collision_box.add_xy(dx, dy);
 
      if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {

	// remove the carried item
	if (state == CARRYING) {
	  start_throwing();
	}

	// set the state
	this->current_conveyor_belt = conveyor_belt;
	set_state(CONVEYOR_BELT);
	sprites->set_animation_stopped();
	game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

	// set the movement
	conveyor_belt_snapping = true;
	set_movement(new TargetMovement(conveyor_belt, walking_speed));
      }
    }
  }
}

/**
 * @brief Updates the hero when he is on a conveyor belt.
 *
 * The state should be CONVEYOR_BELT.
 */
void Hero::update_conveyor_belt(void) {

  get_movement()->update();
  if (conveyor_belt_snapping && get_movement()->is_finished()) {
    clear_movement();
    conveyor_belt_snapping = false;
    std::string path = "  ";
    path[0] = path[1] = '0' + current_conveyor_belt->get_sprite()->get_current_direction();
    set_movement(new PathMovement(path, walking_speed * 2 / 3, false, true, false));
  }
  else {
    if (get_movement()->is_finished() || !on_conveyor_belt) {

      this->current_conveyor_belt = NULL;
      clear_movement();
      set_movement(normal_movement);
      start_free();
    }
    on_conveyor_belt =  false;
  }
}

/**
 * @brief This function is called when a stairs entity detects a collision with this entity.
 * @param stairs the stairs
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_stairs(Stairs *stairs, int collision_mode) {

  if (state != STAIRS
      && state != CARRYING
      && state != SWORD_LOADING
      && state != RETURNING_TO_SOLID_GROUND
      && state != USING_INVENTORY_ITEM) {

    Stairs::Way stairs_way;
    if (stairs->is_inside_floor()) {
      stairs_way = (get_layer() == LAYER_LOW) ? Stairs::NORMAL_WAY : Stairs::REVERSE_WAY;
    }
    else {
      stairs_way = (collision_mode == Detector::COLLISION_FACING_POINT) ? Stairs::NORMAL_WAY : Stairs::REVERSE_WAY;
    }

    // check whether the hero is trying to move in the direction of the stairs
    int correct_direction = stairs->get_movement_direction(stairs_way);
    if (is_moving_towards(correct_direction / 2) || collision_mode == Detector::COLLISION_RECTANGLE) {
      start_stairs(stairs, stairs_way);
    }
  }
}

/**
 * @brief Activates some stairs in the specified way.
 * @param stairs the stairs to activate
 * @param way the way the hero will take the stairs
 */
void Hero::start_stairs(Stairs *stairs, Stairs::Way stairs_way) {
  
  // state
  this->current_stairs = stairs;
  set_state(STAIRS);
  this->stairs_phase = 0;
  this->stairs_way = stairs_way;

  // movement
  int speed = stairs->is_inside_floor() ? 4 : 2;
  std::string path = stairs->get_path(stairs_way);
  PathMovement *movement = new PathMovement(path, speed, false, false, false);

  // sprites and sound
  sprites->set_animation_walking();
  sprites->set_animation_direction((path[0] - '0') / 2);
  game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (stairs->is_inside_floor()) {
    if (stairs_way == Stairs::NORMAL_WAY) {
      // low layer to intermediate layer: change the layer now
      map->get_entities()->set_entity_layer(this, LAYER_INTERMEDIATE);
    }
  }
  else {
    sprites->set_clipping_rectangle(stairs->get_clipping_rectangle(stairs_way));
    if (stairs_way == Stairs::REVERSE_WAY) {
      Rectangle dxy = movement->get_xy_change();
      int fix_y = 8;
      if (path[path.size() - 1] == '2') {
        fix_y *= -1;
      }
      set_xy(get_x() - dxy.get_x(), get_y() - dxy.get_y() + fix_y);
    }
  }
  set_movement(movement);
}

/**
 * @brief Updates the hero when he is walking on stairs.
 *
 * The state should be STAIRS.
 */
void Hero::update_stairs(void) {

  get_movement()->update();

  // first time: we play the sound and initialize
  if (stairs_phase == 0) {
    current_stairs->play_sound(stairs_way);
    next_stairs_phase_date = System::now() + 450;
    stairs_phase++;
  }

  if (current_stairs->is_inside_floor()) {

    // inside a single floor: return to normal state as soon as the movement is finished
    if (get_movement()->is_finished()) {

      if (stairs_way == Stairs::REVERSE_WAY) {
	map->get_entities()->set_entity_layer(this, LAYER_LOW);
      }
      clear_movement();
      set_movement(normal_movement);
      start_free();
    }
  }
  else {
    // stairs between two different floors: more complicated

    if (get_movement()->is_finished()) {
      clear_movement();
      set_movement(normal_movement);

      if (stairs_way == Stairs::NORMAL_WAY) {
	// we are on the old floor:
	// there must be a teletransporter associated with these stairs,
	// otherwise the hero would get stuck into the walls
	if (delayed_teletransporter == NULL) {
	  DIE("Teletransporter expected with the stairs");
	}
	this->current_stairs = NULL;
	delayed_teletransporter->transport_hero(this);
      }
      else {
	// we are on the new floor: it is finished
	sprites->set_clipping_rectangle();
	start_free();
      }
    }
    else { // movement not finished yet
      uint32_t now = System::now();
      if (now >= next_stairs_phase_date) {
	stairs_phase++;
	next_stairs_phase_date += 350;

	// main movement direction corresponding to each animation direction while taking stairs
	static const int movement_directions[] = { 0, 0, 2, 4, 4, 4, 6, 0 };

	int animation_direction = current_stairs->get_animation_direction(stairs_way);
	if (stairs_phase == 2) { // the first phase of the movement is finished
	  if (animation_direction % 2 != 0) {
	    // if the stairs are spiral, take a diagonal direction of animation
	    sprites->set_animation_walking_diagonal(animation_direction);
	  }
	  else {
	    // otherwise, take a usual direction
	    sprites->set_animation_direction(animation_direction / 2);
	    sprites->set_animation_walking();
	  }
	}
	else if (stairs_phase == 3) { // the second phase of the movement (possibly diagonal) is finished
	  sprites->set_animation_walking();

	  if (stairs_way == Stairs::NORMAL_WAY) {
	    // on the old floor, take a direction towards the next floor
	    sprites->set_animation_direction(movement_directions[animation_direction] / 2);
	  }
	  else {
	    // on the new floor, take the direction out of the stairs
	    sprites->set_animation_direction((current_stairs->get_direction() + 2) % 4);
	  }
	}
      }
    }
  }
}

/**
 * @brief Returns the stairs the hero may be currently overlapping.
 *
 * The result is calculated (not stored) so that you can know it
 * even when the game is suspended
 *
 * @return the stairs the hero is currently overlapping, or NULL
 */
Stairs * Hero::get_stairs_overlapping(void) {

  Stairs *stairs = NULL;
  std::list<Stairs*> *all_stairs = map->get_entities()->get_stairs(get_layer());
  std::list<Stairs*>::iterator it;
  for (it = all_stairs->begin(); it != all_stairs->end() && stairs == NULL; it++) {

    if (overlaps(*it)) {
      stairs = *it;
    }
  }

  return stairs;
}

/**
 * @brief This function is called when the hero arrives on this map by taking stairs.
 */
void Hero::stairs_just_arrived(void) {

  // check whether the hero is arriving on the map by stairs too (this is usually the case)
  Stairs *stairs = get_stairs_overlapping();

  if (stairs != NULL) {
    // the hero is arriving on the map by stairs: trigger the stairs manually
    start_stairs(stairs, Stairs::REVERSE_WAY);
  }
}

/**
 * @brief This function is called when a sensor detects a collision with this entity.
 * @param sensor a sensor
 */
void Hero::notify_collision_with_sensor(Sensor *sensor) {

  if (get_state() != RETURNING_TO_SOLID_GROUND && get_state() != JUMPING) {
    sensor->activate(this);
  }
}

/**
 * @brief This function is called when an explosion's sprite detects a collision with a sprite of the hero.
 * @param explosion the explosion
 * @param sprite_overlapping the sprite of the hero that collides with the explosion
 */
void Hero::notify_collision_with_explosion(Explosion *explosion, Sprite *sprite_overlapping) {

  if (get_state() != RETURNING_TO_SOLID_GROUND && get_state() != JUMPING) {
    if (sprite_overlapping->contains("tunic")) {
      hurt(explosion, 2, 0);
    }
  }
}

/**
 * @brief Lets the hero walk.
 *
 * Moves to the state FREE and updates the animations accordingly.
 */
void Hero::start_free(void) {

  set_state(FREE);

  // computes the movement from the directions pressed and detect collisions with entities
  get_normal_movement()->compute_movement();

  if (state == FREE) { // yes, the state may have already changed
    if (get_normal_movement()->is_started()) {
      sprites->set_animation_walking();
    }
    else {
      sprites->set_animation_stopped();
    }
  }
}

/**
 * @brief Makes the hero swing his sword if this action is possible.
 *
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
    sprites->play_sword_sound();
    sprites->set_animation_sword();
  }
}

/**
 * @brief Returns whether the hero can swing his sword right now.
 *
 * The function returns true if the game is not suspended and the hero
 * is in state FREE, PUSHING, CARRYING or SWORD_SWINGING.
 *
 * @return true if the hero can swing his sword, false otherwise
 */
bool Hero::can_start_sword(void) {

  KeysEffect *keys_effect = game->get_keys_effect();

  return !game->is_suspended()
    && (state <= CARRYING || state == SWORD_SWINGING)
    && keys_effect->get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD;
}

/**
 * @brief This function is called repeatedly while the hero is swinging his sword.
 *
 * The state must be SWORD_SWINGING.
 */
void Hero::update_sword_swinging(void) {

  if (sprites->is_animation_finished()) {

    // if the player is still pressing the sword key, start loading the sword
    if (game->get_controls()->is_key_pressed(GameControls::SWORD)) {
      start_sword_loading();
    }
    else {
      start_free();
    }
  }
}

/**
 * @brief Lets the hero load his sword.
 *
 * Moves to the state SWORD_LOADING
 * and updates the animations accordingly.
 */
void Hero::start_sword_loading(void) {
  set_state(SWORD_LOADING);

  // initialize the counter to detect when the sword is loaded
  counter = 0;
  next_counter_date = System::now();
  sword_loaded = false;

  if (get_normal_movement()->is_started()) {
    sprites->set_animation_walking();
  }
  else {
    sprites->set_animation_stopped();
  }
}

/**
 * @brief This function is called repeatedly while the hero is loading his sword.
 *
 * It stops the loading if the sword key is released.
 * The state must be SWORD_LOADING.
 */
void Hero::update_sword_loading(void) {

  uint32_t now = System::now();
  while (!sword_loaded && now >= next_counter_date) {
    counter++;
    next_counter_date += 100;
  }

  // detect when the sword is loaded (i.e. ready for a spin attack)
  if (!sword_loaded && counter >= 10) {
    game->play_sound("sword_spin_attack_load");
    sword_loaded = true;
    counter = 0;
  }

  GameControls *controls = game->get_controls();

  if (!controls->is_key_pressed(GameControls::SWORD)) {
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
 * @brief Makes the hero tap the wall he is facing with his sword.
 *
 * Moves to the state SWORD_TAPPING
 * and updates the animations accordingly.
 */
void Hero::start_sword_tapping(void) {
  set_state(SWORD_TAPPING);
  sprites->set_animation_sword_tapping();
  next_hit_sound_date = System::now() + 100;
}

/**
 * @brief This function is called repeatedly while the hero is tapping a wall with his sword.
 *
 * The state must be SWORD_TAPPING.
 */
void Hero::update_sword_tapping(void) {

  GameControls *controls = game->get_controls();
  const Rectangle &facing_point = get_facing_point();

  if (!controls->is_key_pressed(GameControls::SWORD)
      || get_wanted_movement_direction() != get_animation_direction() * 90
      || !map->test_collision_with_obstacles(get_layer(), facing_point.get_x(), facing_point.get_y(), this)) {
    // the sword key has been released, the player has moved or the obstacle is gone

    if (sprites->get_current_frame() >= 5) {
      // stop tapping the wall, go back to state SWORD_LOADING
      start_sword_loading();
    }
  }
  else {
    uint32_t now = System::now();
    if (sprites->get_current_frame() == 3 && now >= next_hit_sound_date) {

      Detector *facing_entity = get_facing_entity();
      SoundId sound_id;
      if (facing_entity != NULL) {
	sound_id = facing_entity->get_sword_tapping_sound();
      }
      else {
	sound_id = "sword_tapping";
      }
      game->play_sound(sound_id);
      next_hit_sound_date = now + 100;
    }
  }
}

/**
 * @brief Makes the hero lift a destructible item.
 * @param item_to_lift the destructible item to lift
 */
void Hero::start_lifting(DestructibleItem *item_to_lift) {

  // create the corresponding carried item
  this->lifted_item = new CarriedItem(this, item_to_lift);
  lifted_item->set_map(map);

  game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
  set_state(LIFTING);
  sprites->set_animation_lifting();
  set_facing_entity(NULL);
}

/**
 * @brief Returns the item the hero is currently lifting or carrying.
 * @return the item the hero is currently lifting or carrying
 */
CarriedItem * Hero::get_lifted_item(void) {
  return lifted_item;
}

/**
 * @brief Makes the hero carry the item he was lifting.
 */
void Hero::start_carrying(void) {
  set_state(CARRYING);

  if (get_normal_movement()->is_started()) {
    sprites->set_animation_walking();
  }
  else {
    sprites->set_animation_stopped();
  }

  // action icon "throw"
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
}

/**
 * @brief Removes the item carried by the hero, without throwing it.
 *
 * Change the hero state after calling this function.
 */
void Hero::stop_carrying(void) {
  delete lifted_item;
  lifted_item = NULL;
  game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
}

/**
 * @brief Makes the hero throw the item he was carrying.
 */
void Hero::start_throwing(void) {

  // remove the "throw" icon
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  // we check the state because the "throw" icon is actually shown as soon as
  // the hero starts lifting the item
  if (state == CARRYING) {

    if (!lifted_item->is_broken()) {
      lifted_item->throw_item(get_animation_direction());
      map->get_entities()->add_entity(lifted_item);
      lifted_item = NULL;
    }
    start_free();
  }
}

/**
 * @brief Suspends or resumes the animation of the hero's carried items.
 *
 * This function is called when the game is suspended or resumed.
 *
 * @param suspended true to suspend the game, false to resume it
 */
void Hero::set_suspended_carried_items(bool suspended) {

  if (lifted_item != NULL) {
    lifted_item->set_suspended(suspended);
  }
}

/**
 * @brief Updates the carried items.
 */
void Hero::update_carried_items(void) {

  if (lifted_item != NULL) {
    lifted_item->update();

    if (lifted_item != NULL && lifted_item->is_broken()) {
      delete lifted_item;
      lifted_item = NULL;
      start_free();
    }
  }
  else { // no more lifted item: remove the "throw" icon if it is still here
    KeysEffect *keys_effect = game->get_keys_effect();
    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_THROW) {
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    }
  }
}

/**
 * @brief Displays the carried items.
 */
void Hero::display_carried_items(void) {

  if (lifted_item != NULL) {
    lifted_item->display_on_map();
  }
}

/**
 * @brief Deletes the carried items.
 */
void Hero::destroy_carried_items(void) {

  if (lifted_item != NULL) {
    delete lifted_item;
    lifted_item = NULL;
  }
}

/**
 * @brief This function is called when the layer of this entity has just changed.
 */
void Hero::notify_layer_just_changed(void) {

  // if the hero is carrying an item, change its layer too
  if (lifted_item != NULL) {
    lifted_item->set_layer(get_layer());
  }
}

/**
 * @brief Makes the hero push something.
 *
 * Moves to the state PUSHING and updates the animations accordingly.
 */
void Hero::start_pushing(void) {
  set_state(PUSHING);
  sprites->set_animation_pushing();
  pushing_direction_mask = get_normal_movement()->get_direction_mask();
}

/**
 * @brief Updates the hero pushing.
 *
 * This function is called whether his state is already PUSHING or not.
 */
void Hero::update_pushing(void) {

  // no change when the game is suspended
  if (game->is_suspended()) {
    return;
  }

  // get the direction of the arrows
  uint16_t direction_mask = get_normal_movement()->get_direction_mask();

  if (move_tried) {
    // the hero is trying to move

    // see if the move has failed (i.e. if the hero's coordinates have not changed)
    if (get_x() == old_x && get_y() == old_y) {

      // the hero is facing an obstacle

      if (state == FREE) { // is state FREE: see when we can start animation "pushing"

	uint32_t now = System::now();
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
      else if (state == SWORD_LOADING
	  && get_wanted_movement_direction() == get_animation_direction() * 90
	  && (facing_entity == NULL || !facing_entity->is_sword_ignored())) {
	// in state SWORD_LOADING: hit the wall with the sword
	pushing_direction_mask = direction_mask;
	start_sword_tapping();
      }
    }
    else if (state == FREE) {
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

      int straight_direction = sprites->get_animation_direction();

      // stop pushing if there is no more obstacle
      if (!is_facing_obstacle()) {
	start_free();
      }

      // stop pushing if the player changes his direction
      if (get_wanted_movement_direction() != straight_direction * 90) {
	start_grabbing();
      }

      // see if the obstacle is an entity that the hero can push
      else if (facing_entity != NULL && grabbed_entity == NULL) {

	if (facing_entity->get_type() == BLOCK) {
	  try_snap_to_facing_entity();
	}

	if (facing_entity->moved_by_hero()) {

	  std::string path = "  ";
	  int direction = sprites->get_animation_direction();
	  path[0] = path[1] = '0' + direction * 2;

	  set_movement(new PathMovement(path, 8, false, true, false));
	  grabbed_entity = facing_entity;
	}
      }
    }
  }
}

/**
 * @brief Makes the hero grab the object he is facing.
 * @param delay delay before the hero can push or pull 
 * the entity he is facing (if any)
 */
void Hero::start_grabbing(void) {
  sprites->stop_displaying_sword();
  set_state(GRABBING);
  sprites->set_animation_grabbing();
  grabbed_entity = NULL;
}

/**
 * @brief Makes the hero pull the object he is grabbing.
 */
void Hero::start_pulling(void) {
  set_state(PULLING);
  sprites->set_animation_pulling();
}

/**
 * @brief This function is called repeatedly while the hero is grabbing something.
 *
 * The state must be GRABBING or PULLING.
 */
void Hero::update_grabbing_pulling(void) {

  GameControls *controls = game->get_controls();

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
      if (!controls->is_key_pressed(GameControls::ACTION)) {
	start_free();
      }
    }
  }

  // the hero is pulling an obstacle
  if (state == PULLING && !is_moving_grabbed_entity()) {

    int opposite_direction = (sprites->get_animation_direction() + 2) % 4;

    // stop pulling the obstacle if the player changes his direction
    if (get_wanted_movement_direction() != opposite_direction * 90) {
      start_grabbing();
    }

    // stop pulling if the action key is released or if there is no more obstacle
    if (!controls->is_key_pressed(GameControls::ACTION) ||
	!is_facing_obstacle()) {
      start_free();
    }

    // see if the obstacle is an entity that the hero can pull
    if (facing_entity != NULL) {

      if (facing_entity->get_type() == BLOCK) {
	try_snap_to_facing_entity();
      }

      if (facing_entity->moved_by_hero()) {

	std::string path = "  ";
	int opposite_direction = (sprites->get_animation_direction() + 2) % 4;
	path[0] = path[1] = '0' + opposite_direction * 2;

	set_movement(new PathMovement(path, 8, false, true, false));
	grabbed_entity = facing_entity;
      }
    }
  }
}

/**
 * @brief This function is called repeatedly in order to update the hero when he
 * is pushing or pulling the entity he is grabbing.
 */
void Hero::update_moving_grabbed_entity(void) {

  // detect when the hero movement is finished
  // because the hero has covered 16 pixels, has reached an obstacle or has aligned the entity on the grid
  if (is_moving_grabbed_entity()) {

    PathMovement *movement = (PathMovement*) get_movement();

    bool horizontal = get_animation_direction() % 2 == 0;
    bool has_reached_grid = movement->get_total_distance_covered() > 8
      && ((horizontal && is_x_aligned_to_grid()) || (!horizontal && is_y_aligned_to_grid()));

    if (movement->is_finished() || has_reached_grid) {
      stop_moving_grabbed_entity();
    }
  }
}

/**
 * @brief Returns whether the hero is moving the entity he is grabbing.
 * @return true if the hero is moving the entity he is grabbing
 */
bool Hero::is_moving_grabbed_entity(void) {
  return grabbed_entity != NULL;
}

/**
 * @brief Returns whether the hero is in state GRABBING or PULLING.
 * @return true if the state is GRABBING or PULLING
 */
bool Hero::is_grabbing_or_pulling(void) {
  return state == GRABBING || state == PULLING;
}

/**
 * @brief Notifies the hero that the entity he is pushing or pulling
 * cannot move any more because of a collision.
 */
void Hero::grabbed_entity_collision(void) {

  // the hero has moved one pixel too much
  // because he moved before the block, not knowing that the block would not follow him

  int direction_back = sprites->get_animation_direction();

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
 * @brief Makes the hero stop pushing or pulling the entity he is grabbing.
 *
 * This function is called while moving the entity, when the 
 * hero or the entity collides with an obstacle or when
 * the hero's movement is finished.
 */
void Hero::stop_moving_grabbed_entity(void) {

  if (get_movement() != normal_movement) {
    clear_movement();
    set_movement(normal_movement);
  }

  GameControls *controls = game->get_controls();
  if (state == PUSHING && !controls->is_key_pressed(GameControls::ACTION)) {
    // the hero was pushing an entity without grabbing it
    grabbed_entity = NULL;

    // stop the animation pushing if his direction changed
    int straight_direction = sprites->get_animation_direction();
    if (get_wanted_movement_direction() != straight_direction * 90) {
      start_free();
    }
  }
  else {
    start_grabbing();
  }
}

/**
 * @brief Prevents the hero from moving until unfreeze() is called.
 *
 * The current animation of the hero's sprites is stopped and the "stopped" animation is played.
 */
void Hero::freeze(void) {
  get_normal_movement()->set_moving_enabled(false, false);
  sprites->set_animation_stopped();
  destroy_carried_items();
  game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  set_state(FREEZED);
}

/**
 * @brief Lets the hero move again after a call to freeze().
 */
void Hero::unfreeze(void) {
  start_free();
}

/**
 * @brief Makes the hero brandish a treasure.
 * @param treasure the treasure to give him (you have to delete it after the hero brandishes it)
 */
void Hero::give_treasure(Treasure *treasure) {

  this->treasure = treasure;

  // goto the right state
  stop_carrying();
  set_state(BRANDISHING_TREASURE);

  // show the animation
  sprites->save_animation_direction();
  sprites->set_animation_brandish();

  // the shield and the sword are not visible when the hero is brandishing a treasure
  if (equipment->has_shield()) {
    sprites->stop_displaying_shield();
  }
  sprites->stop_displaying_sword();
}

/**
 * @brief Updates the hero when he is brandishing a treasure.
 */
void Hero::update_treasure(void) {

  if (!game->is_giving_treasure()) {

    /* The treasure message is over: if the treasure was a tunic,
     * a sword or a shield, then we must reload the hero's sprites now
     */
    Treasure::Content content = treasure->get_content();
    if (content >= Treasure::BLUE_TUNIC && content <= Treasure::SWORD_4) {
      rebuild_equipment();
    }

    // restore the hero's state
    treasure = NULL;
    sprites->restore_animation_direction();
    start_free();
  }
}

/**
 * @brief Displays the treasure the hero is brandishing.
 */
void Hero::display_treasure(void) {

  int x = get_top_left_x();
  int y = get_top_left_y();

  const Rectangle &camera_position = map->get_camera_position();
  treasure->display(map->get_visible_surface(),
      x - camera_position.get_x(),
      y - 24 - camera_position.get_y());
}

/**
 * @brief Makes the hero load his sword.
 *
 * Moves to the state SWORD_LOADING
 * and updates the animations accordingly.
 */
void Hero::start_spin_attack(void) {
  set_state(SPIN_ATTACK);
  sword_loaded = false;

  // play the sound
  game->play_sound("sword_spin_attack_release");

  // start the animation
  sprites->set_animation_spin_attack();
}

/**
 * @brief This function is called repeatedly during the spin attack.
 *
 * The state must be SPIN_ATTACK.
 */
void Hero::update_spin_attack(void) {

  if (sprites->is_animation_finished()) {
    start_free();
  }
}

/**
 * @brief Makes the hero jump into a direction.
 *
 * While he is jumping, the player does not control him anymore.
 *
 * @param direction direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param with_collisions true to stop the movement if there is a collision
 * @param with_sound true to play the "jump" sound
 */
void Hero::start_jumping(int direction, int length, bool with_collisions, bool with_sound) {
  start_jumping(direction, length, with_collisions, with_sound, get_layer());
}

/**
 * @brief Makes the hero jump into a direction.
 *
 * While he is jumping, the player does not control him anymore.
 *
 * @param direction direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param with_collisions true to stop the movement if there is a collision
 * @param with_sound true to play the "jump" sound
 * @param layer_after_jump the layer to set when the jump is finished
 */
void Hero::start_jumping(int direction, int length, bool with_collisions, bool with_sound, Layer layer_after_jump) {

  // remove the carried item
  if (state == CARRYING) {
    start_throwing();
  }

  // update the sprites
  sprites->stop_displaying_sword();
  sprites->set_animation_direction8(direction);
  sprites->set_animation_jumping();

  // jump
  set_state(JUMPING);
  set_movement(new JumpMovement(direction, length, with_collisions));

  if (with_sound) {
    game->play_sound("jump");
  }
  jump_y = get_y();
  this->layer_after_jump = layer_after_jump;
}

/**
 * @brief Updates the jump action.
 */
void Hero::update_jumping(void) {

  JumpMovement *movement = (JumpMovement*) get_movement();
  movement->update();
  jump_y = get_y() - movement->get_jump_height();

  if (movement->is_finished()) {

    map->get_entities()->set_entity_layer(this, layer_after_jump);
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
 * @brief Returns the height of the current jump.
 * @return the height of the current jump in pixels
 */
int Hero::get_jump_y(void) {
  return jump_y;
}

/**
 * @brief Returns whether the hero can be hurt.
 * @return true if the hero can be hurt in its current state
 */
bool Hero::can_be_hurt(void) {
  return state <= SPIN_ATTACK && !sprites->is_blinking() && get_movement() == normal_movement;
}

/**
 * @brief Hurts the hero if possible.
 * @param source the entity that hurts the hero (usually an enemy)
 * @param life_points number of heart quarters to remove (this number may be reduced by the tunic)
 * @param magic_points number of magic points to remove
 */
void Hero::hurt(MapEntity *source, int life_points, int magic_points) {

  if (can_be_hurt()) {

    // remove the carried item
    if (state == LIFTING || state == CARRYING) {
      start_throwing();
    }
    sprites->stop_displaying_sword();

    game->play_sound("hero_hurt");

    life_points = std::max(1, life_points / (equipment->get_tunic() + 1));

    equipment->remove_hearts(life_points);
    if (magic_points > 0 && equipment->get_magic() > 0) {
      equipment->remove_magic(magic_points);
      game->play_sound("magic_bar");
    }
    sprites->blink();
    set_state(HURT);
    sprites->set_animation_hurt();

    double angle = source->get_vector_angle(this);
    set_movement(new StraightMovement(12, angle, 200));
  }
}

/**
 * @brief Updates the HURT state.
 */
void Hero::update_hurt(void) {

  get_movement()->update();
  if (get_movement()->is_finished()) {

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
 * @brief Returns whether the hero is in a state such that the game over sequence can start.
 * @return true if the game over sequence can start
 */
bool Hero::can_start_gameover_sequence(void) {
  return state != HURT && state != PLUNGING && state != FALLING && state != RETURNING_TO_SOLID_GROUND;
}

/**
 * @brief This function is called when the hero was dead but saved by a fairy.
 */
void Hero::get_back_from_death(void) {
  sprites->blink();
  start_free();
  when_suspended = System::now();
}

/**
 * @brief This function is called when a non-pixel perfect enemy collides with the hero.
 * @param enemy the enemy
 */
void Hero::notify_collision_with_enemy(Enemy *enemy) {
  enemy->attack_hero(this, NULL);
}

/**
 * @brief This function is called when an enemy's sprite collides with a sprite of the hero.
 * @param enemy the enemy
 * @param enemy_sprite the enemy's sprite that overlaps a sprite of the hero
 * @param this_sprite the hero's sprite that overlaps the enemy's sprite
 */
void Hero::notify_collision_with_enemy(Enemy *enemy, Sprite *enemy_sprite, Sprite *this_sprite) {

  if (this_sprite->contains("sword")) {
    // the hero's sword overlaps the enemy
    enemy->try_hurt(ATTACK_SWORD, this, enemy_sprite);
  }
  else if (this_sprite->contains("tunic")) {
    // the hero's body overlaps the enemy: ensure that the 16*16 rectangle of the hero also overlaps the enemy
    Rectangle enemy_sprite_rectangle = enemy_sprite->get_size();
    const Rectangle &enemy_sprite_origin = enemy_sprite->get_origin();
    enemy_sprite_rectangle.set_x(enemy->get_x() - enemy_sprite_origin.get_x());
    enemy_sprite_rectangle.set_y(enemy->get_y() - enemy_sprite_origin.get_y());

    if (overlaps(enemy_sprite_rectangle)) {
      enemy->attack_hero(this, enemy_sprite);
    }
  }
}

/**
 * @brief Notifies this entity that it has just attacked an enemy
 *
 * This function is called even if this attack was not successful.
 *
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack (see Enemy.h)
 * @param killed indicates that the attack has just killed the enemy
 */
void Hero::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed) {

  if (result == 0) {
    // attack ignored
    return;
  }

  switch (attack) {

    case ATTACK_SWORD:
      if (get_state() == SWORD_LOADING) {
	sprites->stop_displaying_sword();
	start_free();
      }
      break;

    default:
      DIE("Unknown attack '" << attack << "'");
  }
}

/**
 * @brief Makes the hero drown or swim.
 */
void Hero::start_deep_water(void) {

  // stop the sword
  sprites->stop_displaying_sword();

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
    if (equipment->has_inventory_item(INVENTORY_FLIPPERS)) {
      start_swimming();
    }
    else {
      start_jumping(get_wanted_movement_direction8(), 32, true, true);
      ((JumpMovement*) get_movement())->set_delay(13);
    }
  }
}

/**
 * @brief Makes the hero plunge into the water.
 */
void Hero::start_plunging(void) {
  set_state(PLUNGING);
  sprites->set_animation_plunging();
  game->play_sound("splash");
}

/**
 * @brief Updates the PLUNGING state.
 */
void Hero::update_plunging(void) {

  if (sprites->is_animation_finished()) {

    if (ground != GROUND_DEEP_WATER) {
      start_free();
    }
    else if (equipment->has_inventory_item(INVENTORY_FLIPPERS)) {
      start_swimming();
    }
    else {
      game->play_sound("message_end");
      start_returning_to_solid_ground(last_solid_ground_coords, last_solid_ground_layer);
      equipment->remove_hearts(1);
    }
  }
}

/**
 * @brief Makes the hero swim.
 */
void Hero::start_swimming(void) {
  set_state(SWIMMING);
  // TODO
}

/**
 * @brief Makes the hero stop swimming.
 */
void Hero::stop_swimming(void) {
  start_free();
}

/**
 * @brief Makes the hero move towards a hole of fall into it.
 */
void Hero::start_hole(void) {

  next_ground_date = System::now();
  delayed_teletransporter = NULL;

  // push the hero into a direction
  if (is_moving_towards(0)) {
    hole_dx = 1;
  }
  else if (is_moving_towards(2)) {
    hole_dx = -1;
  }
  else {
    hole_dx = 0;
  }

  if (is_moving_towards(1)) {
    hole_dy = -1;
  }
  else if (is_moving_towards(3)) {
    hole_dy = 1;
  }
  else {
    hole_dy = 0;
  }

  get_normal_movement()->set_moving_speed(3);
}

/**
 * @brief Makes the hero fall into a hole.
 */
void Hero::start_falling(void) {

  // remove the carried item
  if (state == CARRYING) {
    start_throwing();
  }

  sprites->save_animation_direction();
  set_state(FALLING);
  sprites->set_animation_falling();
  game->play_sound("hero_falls");
}

/**
 * @brief Updates the FALLING state.
 */
void Hero::update_falling(void) {

  if (sprites->is_animation_finished()) {

    // the hero has just finished falling
    if (delayed_teletransporter != NULL) {
      // special hole with a teletransporter
      delayed_teletransporter->transport_hero(this);
    }
    else {
      // hole that hurts the hero
      if (target_solid_ground_coords.get_x() != -1) {
	// go back to a target point specified earlier
	start_returning_to_solid_ground(target_solid_ground_coords, target_solid_ground_layer);
      }
      else {
	// nothing was specified: just go back to the last solid ground location
	start_returning_to_solid_ground(last_solid_ground_coords, last_solid_ground_layer);
      }

      equipment->remove_hearts(2);
      sprites->set_animation_stopped();
      sprites->restore_animation_direction();
    }
  }
  else if (get_movement() != normal_movement) {
    // delete a possible hurt movement
    clear_movement();
    set_movement(normal_movement);
  }
}

/**
 * @brief Specifies a point of the map where the hero will go back if he falls
 * into a hole or some other bad ground.
 *
 * This function is usually called when the hero walks on a special sensor.
 *
 * @param target_solid_ground_coords coordinates of the position where
 * the hero will go if he falls into a hole or some other bad ground
 * @param layer the layer
 */
void Hero::set_target_solid_ground_coords(const Rectangle &target_solid_ground_coords, Layer layer) {
  this->target_solid_ground_coords = target_solid_ground_coords;
  this->target_solid_ground_layer = layer;
}

/**
 * @brief Hides the hero and makes him move back to the specified solid ground location.
 * @param target_xy coordinates of the solid ground location
 * @param target_layer the destination layer
 */
void Hero::start_returning_to_solid_ground(const Rectangle &target_xy, Layer target_layer) {
  map->get_entities()->remove_boomerang();
  set_state(RETURNING_TO_SOLID_GROUND);
  set_movement(new TargetMovement(target_xy.get_x(), target_xy.get_y(), walking_speed));
  map->get_entities()->set_entity_layer(this, target_layer);
}

/**
 * @brief Updates the RETURNING_TO_SOLID_GROUND state.
 */
void Hero::update_returning_to_solid_ground(void) {

  // the current movement is an instance of TargetMovement
  get_movement()->update();

  if (get_movement()->is_finished()) {
    clear_movement();
    set_movement(normal_movement);
    sprites->blink();
    start_free();
  }
}

/**
 * @brief Makes the hero brandish his sword meaning a victory.
 */
void Hero::start_victory() {

  set_state(VICTORY);
  sprites->set_animation_victory();
  game->play_sound("victory");

  // compute the date when the VICTORY state is considered as finished,
  // but the game may be currently suspended
  uint32_t start_victory_date = suspended ? when_suspended : System::now();
  end_victory_date = start_victory_date + 1500;
}

/**
 * @brief Updates the VICTORY state.
 */
void Hero::update_victory() {

  if (System::now() >= end_victory_date) {
    map->get_script()->event_hero_victory_sequence_finished();
  }
}

/**
 * @brief Updates the FREEZED state.
 */
void Hero::update_freezed(void) {
}

/**
 * @brief Returns whether the hero can use an item from
 * the inventory now.
 * @param item_id id of the item to check
 */
bool Hero::can_start_inventory_item(InventoryItemId item_id) {

  return state == FREE && InventoryItem::can_be_assigned(item_id)
    && equipment->has_inventory_item(item_id)
    && System::now() >= when_can_use_inventory_item;
}

/**
 * @brief Starts using an item from the inventory.
 * @param item the item to use.
 */
void Hero::start_inventory_item(InventoryItemId item_id) {

  this->current_inventory_item = new InventoryItem(item_id);
  this->when_can_use_inventory_item = System::now() + 500;
  set_state(USING_INVENTORY_ITEM);
  current_inventory_item->start(game);
}

/**
 * @brief Updates the USING_INVENTORY_ITEM state.
 */
void Hero::update_inventory_item(void) {

  current_inventory_item->update();
  if (current_inventory_item->is_finished()) {
    delete current_inventory_item;
    current_inventory_item = NULL;

    if (get_state() == USING_INVENTORY_ITEM) {
      // if the state was not modified by the item, return to the normal state
      start_free();
    }
  }
}

/**
 * @brief Starts the boomerang animation.
 *
 * The state should be USING_INVENTORY_ITEM.
 */
void Hero::start_boomerang(void) {
  sprites->set_animation_boomerang();
}

/**
 * @brief Starts the bow animation.
 *
 * The state should be USING_INVENTORY_ITEM.
 */
void Hero::start_bow(void) {
  sprites->set_animation_bow();
}

/**
 * @brief Starts running with the running shoes.
 *
 * The state should be USING_INVENTORY_ITEM.
 */
void Hero::start_running(void) {

  set_movement(new StraightMovement(10, get_animation_direction() * 90, 300));
  sprites->set_animation_walking();
  inventory_item_phase = 0;
  inventory_item_phase_date = System::now() + 300;
}

/**
 * @brief This function is called repeatedly while the hero is running with the running shoes.
 *
 * The state should be USING_INVENTORY_ITEM.
 */
void Hero::update_running(void) {

  uint32_t now = System::now();
  if (inventory_item_phase == 0) {
    
    if (now >= inventory_item_phase_date) {
      set_movement(new StraightMovement(30, get_animation_direction() * 90, 2000));
      sprites->set_animation_walking();
      inventory_item_phase++;
    }
  }
  else if (get_movement()->is_finished() 
      || map->test_collision_with_obstacles(get_layer(), get_movement()->get_last_collision_box_on_obstacle(), this)) {
    clear_movement();
    set_movement(normal_movement);
  }
}

/**
 * @brief Stops running.
 */
void Hero::stop_running(void) {
  
  StraightMovement *movement = (StraightMovement*) get_movement();
  movement->set_finished();
  inventory_item_phase++;
}

/**
 * @brief Returns whether the hero is currently running.
 * @return true if the heor is running
 */
bool Hero::is_running(void) {

  return state == USING_INVENTORY_ITEM && current_inventory_item->get_id() == INVENTORY_SPEED_SHOES;
}

/**
 * @brief Returns whether the run is finished.
 *
 * The state should be USING_INVENTORY_ITEM.
 *
 * @return true if the run is finished
 */
bool Hero::is_running_finished(void) {
  return inventory_item_phase != 0 && get_movement() == get_normal_movement();
}

