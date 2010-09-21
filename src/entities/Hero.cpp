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
#include "entities/MapEntities.h"
#include "entities/DestinationPoint.h"
#include "entities/Teletransporter.h"
#include "entities/Stairs.h"
#include "entities/DestructibleItem.h"
#include "entities/ConveyorBelt.h"
#include "entities/CrystalSwitch.h"
#include "entities/Block.h"
#include "entities/JumpSensor.h"
#include "entities/Sensor.h"
#include "hero/HeroSprites.h"
#include "hero/CarryingState.h"
#include "hero/ConveyorBeltState.h"
#include "hero/FallingState.h"
#include "hero/FreeState.h"
#include "hero/FreezedState.h"
#include "hero/GrabbingState.h"
#include "hero/HurtState.h"
#include "hero/JumpingState.h"
#include "hero/LiftingState.h"
#include "hero/PlungingState.h"
#include "hero/RunningState.h"
#include "hero/StairsState.h"
#include "hero/SwimmingState.h"
#include "hero/TreasureState.h"
#include "hero/VictoryState.h"
#include "movements/Movement.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Game.h"
#include "Map.h"
#include "Equipment.h"
#include "KeysEffect.h"
#include "Sprite.h"

const int Hero::normal_walking_speed = 9;

/**
 * @brief Creates a hero.
 * @param equipment the equipment
 */
Hero::Hero(Equipment *equipment):
  state(NULL), old_state(NULL), equipment(equipment), facing_entity(NULL),
  walking_speed(normal_walking_speed), on_conveyor_belt(false), on_raised_blocks(false),
  last_inventory_item_name(""), can_use_inventory_item_date(0),
  ground(GROUND_NORMAL), next_ground_date(0) {

  // position
  set_size(16, 16);
  set_origin(8, 13);
  last_solid_ground_coords.set_xy(-1, -1);
  last_solid_ground_layer = LAYER_LOW;
 
  // sprites
  sprites = new HeroSprites(this, equipment);
  rebuild_equipment();

  // state
  set_state(new FreeState(this));
}

/**
 * @brief Destructor.
 */
Hero::~Hero(void) {

  delete sprites;
  delete state;
  delete old_state;
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Hero::get_type() {
  return HERO;
}

/**
 * @brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * @return true if this type of entity can be obstacle for other entities
 */
bool Hero::can_be_obstacle(void) {
  return true; 
}

/**
 * @brief Returns whether entities of this type have detection capabilities.
 *
 * This function returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 *
 * @return true if this type of entity can detect other entities
 */
bool Hero::can_detect_entities(void) {
  return false;
}

/**
 * @brief Returns whether entities of this type can be displayed.
 *
 * If yes, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 *
 * @return true if this type of entity can be displayed
 */
bool Hero::can_be_displayed(void) {
  return true; 
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * This function returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Hero::is_displayed_in_y_order(void) {
  return true;
}

/**
 * @brief Changes the hero's internal state.
 *
 * This function stops the old state and starts the new one.
 * The old state will also be automatically destroyed, but not right now,
 * in order to allow this function to be called by the old state itself safely.
 *
 * @param state the new state of the hero
 */
void Hero::set_state(State *new_state) {

  // stop the previous state
  if (this->state != NULL) {
    this->state->stop(new_state);
  }

  old_state = this->state;

  this->state = new_state;
  this->state->start(old_state);
}

/**
 * @brief Suspends or resumes the animation and the movements of the hero.
 *
 * This function is called by the map when the game is suspended or resumed.
 *
 * @param suspended true to suspend the hero, false to resume it
 */
void Hero::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended);

  if (!suspended) {

    uint32_t diff = System::now() - when_suspended;
    next_ground_date += diff;
    can_use_inventory_item_date += diff;
  }

  sprites->set_suspended(suspended);
  state->set_suspended(suspended);
}

/**
 * @brief Updates the hero's position, movement and animation.
 *
 * This function is called repeteadly by the game loop.
 */
void Hero::update(void) {

  update_movement();
  update_state();

  if (!is_suspended()) {
    
    sprites->update();
    update_ground();
    map->check_collision_with_detectors(this);
    check_gameover();
  }
}

/**
 * @brief Updates the hero's internal state.
 *
 * This function is called repeatedly by update().
 */
void Hero::update_state(void) {

  // update the current state
  state->update();

  // see if there is an old state to destroy
  if (old_state != NULL) {
    delete old_state;
    old_state = NULL;
  }
}

/**
 * @brief Updates the hero's position according to its movement.
 *
 * This function is called repeatedly by update().
 */
void Hero::update_movement(void) {

  on_raised_blocks = map->get_entities()->overlaps_raised_blocks(get_layer(), get_bounding_box());

  if (movement == NULL) {
    return;
  }
  movement->update();
}

/**
 * @brief Updates the ground under the hero.
 *
 * This function is called repeatedly.
 */
void Hero::update_ground(void) {

  // see if it's time to do something (depending on the ground)
  uint32_t now = System::now();
  if (now >= next_ground_date) {

    if (is_ground_visible() && get_movement() != NULL) {

      // a special ground is displayed under the hero and it's time to play a sound
      next_ground_date = now + std::max(150, (int) (2000 / get_movement()->get_speed()));
      if (sprites->is_walking() && state->is_touching_ground()) {
        sprites->play_ground_sound();
      }
    }

    else if (ground == GROUND_HOLE && !state->can_avoid_hole()) {
      // the hero is being attracted by a hole and it's time to move one more pixel into the hole
      
      next_ground_date = now + 60;

      if (get_distance(last_solid_ground_coords.get_x(), last_solid_ground_coords.get_y()) >= 8) {
	// too far from the solid ground: make the hero fall
	set_walking_speed(normal_walking_speed);
        set_state(new FallingState(this));
      }
      else {

        // not too far yet
	bool moved = false;
        Rectangle collision_box = get_bounding_box();
        collision_box.add_xy(hole_dxy);

        if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
          set_bounding_box(collision_box);
          notify_position_changed();
	  moved = true;
        }

	if (!moved && hole_dxy.get_x() != 0) { // try x only
	  collision_box = get_bounding_box();
	  collision_box.add_xy(hole_dxy.get_x(), 0);
	  if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	    set_bounding_box(collision_box);
	    notify_position_changed();
	    moved = true;
	  }
	}

	if (!moved && hole_dxy.get_y() != 0) { // try y only
	  collision_box = get_bounding_box();
	  collision_box.add_xy(0, hole_dxy.get_y());
	  if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	    set_bounding_box(collision_box);
	    notify_position_changed();
	    moved = true;
	  }
	}

	if (!moved) {
	  // the hero cannot be moved towards the direction previously calculated
	  set_walking_speed(normal_walking_speed);
	  set_state(new FallingState(this));
	}
      }
    }
  }
}

/**
 * @brief Checks that the hero has still some life.
 *
 * If his life reaches zero, this function starts the gameover sequence.
 */
void Hero::check_gameover(void) {

  if (equipment->get_life() <= 0 && state->can_start_gameover_sequence()) {
    sprites->stop_blinking();
    game->start_gameover_sequence();
  }
}

/**
 * @brief Displays this entity on the map.
 *
 * This function should draw the entity only if is_visible() returns true.
 * The hero is displayed with its current animation and at its current position.
 */
void Hero::display_on_map(void) {

  if (is_visible()) {
    // the state may call get_sprites()->display_on_map() or make its own display
    state->display_on_map();
  }
}

/**
 * @brief This function is called when a game key is pressed
 * and the game is not suspended.
 * @param key the key pressed
 */
void Hero::key_pressed(GameControls::GameKey key) {
  state->key_pressed(key);
}

/**
 * @brief This function is called when a key is released
 * if the game is not suspended.
 * @param key the key released
 */
void Hero::key_released(GameControls::GameKey key) {
  state->key_released(key);
}

/**
 * @brief Returns the sprites currently representing the hero.
 * @return the sprites
 */
HeroSprites * Hero::get_sprites(void) {
  return sprites;
}

/**
 * @brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * @return the direction of the sprites (0 to 3)
 */
int Hero::get_animation_direction(void) {
  return sprites->get_animation_direction();
}

/**
 * @brief Changes the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * @param direction4 the direction to set (0 to 3)
 */
void Hero::set_animation_direction(int direction4) {
  sprites->set_animation_direction(direction4);
}

/**
 * @brief Returns whether the sprites animations are finished.
 * @return true if the animation is finished
 */
bool Hero::is_animation_finished(void) {
  return sprites->is_animation_finished();
}

/**
 * @brief Loads (or reloads) the sprites and sounds of the hero and his equipment.
 *
 * The sprites and sounds depend on its tunic, sword and shield as specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as the hero's equipment is changed.
 */
void Hero::rebuild_equipment(void) {

  sprites->rebuild_equipment();
}

/**
 * @brief Returns whether this entity is currently visible.
 * @return true if this entity is currently visible
 */
bool Hero::is_visible(void) {

  return MapEntity::is_visible()
    && state->is_hero_visible()
    && !game->is_showing_gameover();
}

/**
 * @brief Returns whether the shadow should be currently displayed, separate from the tunic sprite.
 * @return true if the shadow should be currently displayed.
 */
bool Hero::is_shadow_visible(void) {
  return get_height_above_shadow() > 0;
}

/**
 * @brief When a shadow is displayed separate from the tunic sprite,
 * returns the height where the tunic sprite should be displayed.
 * @return the height in pixels, or zero if there is no separate shadow in this state
 */
int Hero::get_height_above_shadow(void) {
  return state->get_height_above_shadow();
}

/**
 * @brief Sets the hero's current map.
 *
 * This function is called when the map is changed.
 *
 * @param map the map
 */
void Hero::set_map(Map *map) {

  MapEntity::set_map(map);

  last_solid_ground_coords.set_xy(-1, -1);
  target_solid_ground_coords.set_xy(-1, -1);
  ground = GROUND_NORMAL;

  state->set_map(map);
}

/**
 * @brief Sets the hero's current map.
 *
 * This function is called when the map is changed.
 *
 * @param map the map
 * @param initial_direction the direction of the hero (0 to 3)
 * or -1 to let the direction unchanged
 */
void Hero::set_map(Map *map, int initial_direction) {

  // take the specified direction
  if (initial_direction != -1) {
    sprites->set_animation_direction(initial_direction);
  }

  set_map(map);
}

/**
 * @brief Places the hero on the map specified and at its destination point selected.
 * @param map the new map
 */
void Hero::place_on_destination_point(Map *map) {

  MapEntities *entities = map->get_entities();

  std::string destination_point_name = map->get_destination_point_name();

  if (destination_point_name == "_same") {

    // the hero's coordinates are the same as on the previous map
    // but we may have to change the layer
    
    Layer layer = LAYER_INTERMEDIATE;
    if (entities->get_obstacle_tile(LAYER_INTERMEDIATE, get_x(), get_y()) == OBSTACLE_EMPTY) {
      layer = LAYER_LOW;
    }
    set_map(map);
    map->get_entities()->set_entity_layer(this, layer);

    start_free();
  }
  else {
    int side = map->get_destination_side();

    if (side != -1) {

      // only one coordinate is changed
      set_map(map);

      switch (side) {

	case 0: // right side
	  set_x(map->get_width());
	  break;

	case 1: // top side
	  set_y(5);
	  break;

	case 2: // left side
	  set_x(0);
	  break;

	case 3: // bottom side
	  set_y(map->get_height() + 5);
	  break;

	default:
	  Debug::die(StringConcat() << "Invalid destination side: " << side);
      }
      // note that we keep the state from the previous map
    }
    else {

      // normal case: the location is specified by a destination point object

      DestinationPoint *destination_point = (DestinationPoint*)
	entities->get_entity(DESTINATION_POINT, destination_point_name);

      set_map(map, destination_point->get_direction());
      set_xy(destination_point->get_x(), destination_point->get_y());
      entities->set_entity_layer(this, destination_point->get_layer());

      entities->remove_boomerang(); // useful when the map remains the same

      Stairs *stairs = get_stairs_overlapping();
      if (stairs != NULL) {
        // the hero arrived on the map by stairs
	set_state(new StairsState(this, stairs, Stairs::REVERSE_WAY));
      }
      else {
	// the hero arrived on the map by a usual destination point
	start_free();
      }
    }
  }
}

/**
 * @brief This function is called when the opening transition of the map is finished.
 *
 * The position of the hero is changed if necessary.
 */
void Hero::notify_opening_transition_finished(void) {

  int side = map->get_destination_side();  
  if (side != -1) {
    // the hero was placed on the side of the map:
    // there was a scrolling between the previous map and this one

    switch (side) {

      case 0: // right side
	set_x(map->get_width() - 8);
	break;

      case 1: // top side
	set_y(13);
	break;

      case 2: // left side
	set_x(8);
	break;

      case 3: // bottom side
	set_y(map->get_height() - 3);
	break;

      default:
	Debug::die(StringConcat() << "Invalid destination side: " << side);
    }
  }
  notify_position_changed();
}

/**
 * @brief Returns the coordinates of the first point in the direction the hero's sprite is looking at.
 *
 * This point is 1 pixel outside the hero's bounding box (and centered). It is used
 * to determine the actions he can do depending on the entity he is facing
 * (a bush, a pot, an NPC…)
 *
 * @return the point the hero is touching
 */
const Rectangle Hero::get_facing_point(void) {

  return get_facing_point(get_animation_direction());
}

/**
 * @brief Returns the coordinates of the first point in the specified direction.
 *
 * This point is 1 pixel outside the hero's bounding box (and centered). It is used
 * to determine the actions he can do depending on the entity he is facing
 * (a bush, a pot, an NPC…)
 *
 * @param direction a direction (0 to 3)
 * @return coordinates of the point the hero would be touching if he was looking towards that direction
 */
const Rectangle Hero::get_facing_point(int direction) {

  Rectangle facing_point;

  switch (direction) {

    // right
    case 0:
      facing_point.set_xy(bounding_box.get_x() + 16, bounding_box.get_y() + 8);
      break;

      // up
    case 1:
      facing_point.set_xy(bounding_box.get_x() + 8, bounding_box.get_y() - 1);
      break;

      // left
    case 2:
      facing_point.set_xy(bounding_box.get_x() - 1, bounding_box.get_y() + 8);
      break;

      // down
    case 3:
      facing_point.set_xy(bounding_box.get_x() + 8, bounding_box.get_y() + 16);
      break;

    default:
      Debug::die(StringConcat() << "Invalid direction for Hero::get_facing_point(): " << direction);
  }

  facing_point.set_size(1, 1);

  return facing_point;
}

/**
 * @brief Returns the entity in front of the hero.
 * @return the detector the hero is touching, or NULL if he is not touching a detector in front of him
 */
Detector * Hero::get_facing_entity(void) {
  return facing_entity;
}

/**
 * @brief Sets the entity the hero is currently facing.
 *
 * This function may be called by an entity that has just detected
 * that the player was facing it.
 *
 * @param detector the detector the hero is facing (may be NULL)
 */
void Hero::set_facing_entity(Detector *detector) {

  this->facing_entity = detector;

  KeysEffect *keys_effect = game->get_keys_effect();

  if (facing_entity == NULL &&
      keys_effect->is_action_key_acting_on_facing_entity()) {

    // the hero stopped facing an entity that was showing an action icon
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  }
}

/**
 * @brief Returns whether there is an obstacle in front of the hero.
 *
 * This function returns whether he is touching an obstacle that
 * is just in front of him.
 * Note that even if this function returns true, the hero
 * may still be able to move in that direction due to the possibly sliding movement.
 * This information is calculated and not stored, so it is
 * always up to date.
 *
 * @return true if the hero is facing an obstacle
 */
bool Hero::is_facing_obstacle(void) {

  Rectangle collision_box = get_bounding_box();
  switch (sprites->get_animation_direction()) {

    case 0:
      collision_box.add_x(1);
      break;

    case 1:
      collision_box.add_y(-1);
      break;

    case 2:
      collision_box.add_x(-1);
      break;

    case 3:
      collision_box.add_y(1);
      break;

    default:
      Debug::die(StringConcat() << "Invalid animation direction '" << sprites->get_animation_direction() << "'");
      break;
  }

  return map->test_collision_with_obstacles(get_layer(), collision_box, this);
}

/**
 * @brief Returns whether the facing point of the hero is overlapping an obstacle.
 *
 * This function returns whether his facing point is overlapping an obstacle of the map.
 * This information is calculated and not stored, so it is always up to date.
 *
 * @return true if the facing point is overlapping an obstacle
 */
bool Hero::is_facing_point_on_obstacle(void) {

  const Rectangle &facing_point = get_facing_point();
  return map->test_collision_with_obstacles(get_layer(), facing_point.get_x(), facing_point.get_y(), this);
}

/**
 * @brief Returns whether the hero is looking towards the specified direction.
 * @param direction4 a direction (0 to 3)
 * @return true if the hero is looking towards the specified direction
 */
bool Hero::is_facing_direction4(int direction4) {
  return get_animation_direction() == direction4;
}

/**
 * @brief Returns whether the hero is looking towards the specified direction.
 * @param direction8 a direction (0 to 7)
 * @return true if the hero is looking towards the specified direction
 * (always false for diagonal directions)
 */
bool Hero::is_facing_direction8(int direction8) {
  return get_animation_direction() * 2 == direction8;
}

/**
 * @brief Returns whether the hero is currently on raised crystal switch blocks.
 * @return true if the hero is currently on raised crystal switch blocks
 */
bool Hero::is_on_raised_blocks(void) {
  return on_raised_blocks;
}

/**
 * @brief Returns the stairs the hero may be currently overlapping.
 *
 * The result is calculated (not stored) so that you can know it
 * even when the game is suspended.
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
 * @brief Returns whether the player can control his movements in the current state.
 * @return true if the player can control his movements
 */
bool Hero::can_control_movement(void) {
  return state->can_control_movement();
}

/**
 * @brief Returns the current speed applied to the hero's movements when he is walking.
 * @return the current walking speed
 */
int Hero::get_walking_speed(void) {
  return walking_speed;
}

/**
 * @brief Sets the speed to apply to the hero's movements when he is walking.
 * @param walking_speed the new walking speed
 */
void Hero::set_walking_speed(int walking_speed) {
  this->walking_speed = walking_speed;
  state->notify_walking_speed_changed();
}

/**
 * @brief Returns the direction of the hero's movement as defined by the controls applied by the player.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * @return the hero's wanted direction between 0 and 7, or -1 if he is stopped
 */
int Hero::get_wanted_movement_direction8(void) {
  return state->get_wanted_movement_direction8();
}

/**
 * @brief Returns the actual direction of the hero's movement.
 *
 * This function returns the actual direction of the hero's movement, which can be different from the one
 * defined by the directional keys pressed by the player because we consider obstacles here.
 * If he does not want to move, -1 is returned. If he is trying to move but cannot because of obstacles,
 * the direction he is trying to move toward is returned.
 * This function is not used to compute the hero's movement (PlayerMovement does that) but only
 * to decide what direction to give to its sprites once the movement is already computed.
 *
 * @return the hero's actual direction between 0 and 7, or -1 if he is stopped
 */
int Hero::get_real_movement_direction8(void) {

  int result;

  int wanted_direction8 = get_wanted_movement_direction8();
  if (wanted_direction8 == -1) {
    // the hero does not want to move
    result = -1;
  }
  else {
    // the hero wants to move

    Rectangle collision_box(get_bounding_box());

    // if we can move towards the wanted direction, no problem
    Rectangle xy_move = direction_to_xy_move(wanted_direction8);
    collision_box.add_xy(xy_move.get_x(), xy_move.get_y());
    if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
      result = wanted_direction8;
    }
    else {
      // otherwise, see if he can move in one of the two closest directions (i.e. he is sliding)

      int alternative_direction8 = (wanted_direction8 + 1) % 8;
      collision_box = get_bounding_box();
      xy_move = direction_to_xy_move(alternative_direction8);
      collision_box.add_xy(xy_move.get_x(), xy_move.get_y());
      if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	result = alternative_direction8;
      }
      else {
        alternative_direction8 = (wanted_direction8 - 1) % 8;
        collision_box = get_bounding_box();
	xy_move = direction_to_xy_move(alternative_direction8);
	collision_box.add_xy(xy_move.get_x(), xy_move.get_y());
        if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	  result = alternative_direction8;
        }
	else {
	  // he is not sliding, he wants to move but can't
          result = wanted_direction8;
	}
      }
    }
  }

  return result;
}

/**
 * @brief Returns whether the hero is moving towards the specified direction
 * among the four main directions.
 *
 * If the hero is not moving, false is returned.
 * If he is making a diagonal move, this function considers that he is moving
 * towards both directions.
 *
 * @param direction4 one of the four main directions (0 to 3)
 * @return true if the hero is moving in that direction, even if he is actually doing a diagonal move
 */
bool Hero::is_moving_towards(int direction4) {

  Movement *movement = get_movement();
  if (movement == NULL || movement->is_stopped()) {
    return false;
  }

  int direction8 = direction4 * 2;
  int movement_direction8 = get_wanted_movement_direction8();
  return movement_direction8 == direction8
    || (movement_direction8 + 1) % 8 == direction8
    || (movement_direction8 + 7) % 8 == direction8;
}

/**
 * @brief Returns whether the animation direction is locked.
 *
 * When this function returns false, which is the case most of the time,
 * it means that the animation direction is set to the movement direction.
 * When it returns true, it means that the animation direction is fixed
 * and do not depends on the movement direction anymore (this is the case
 * when the hero is loading his sword).
 *
 * @return true if the animation direction is locked
 */
bool Hero::is_direction_locked(void) {
  return state->is_direction_locked();
}

/**
 * @brief Notifies this entity that it has just tried to change his position.
 * @param success true if the position has actually just changed
 */
void Hero::notify_movement_tried(bool success) {

  MapEntity::notify_movement_tried(success);

  state->notify_movement_tried(success);
}

/**
 * @brief This function is called when the hero's position is changed,
 * or when his direction changes.
 */
void Hero::notify_position_changed(void) {

  if (state->are_collisions_ignored()) {
    // do not take care of the ground or detectors
    return;
  }

  // the facing entity has to be recomputed
  set_facing_entity(NULL);

  // save the current ground
  Ground previous_ground = this->ground;

  // see the ground indicated by the tiles
  if (!is_suspended()) { // when the game is suspended, the hero may have invalid coordinates (e.g. transition between maps)
    Ground tiles_ground = map->get_tile_ground(get_layer(), get_x(), get_y() - 2);
    set_ground(tiles_ground);
  }

  // see the ground indicated by the dynamic entities, also find the facing entity
  MapEntity::notify_position_changed();

  if (this->ground != previous_ground) {
    notify_ground_changed();
  }

  if (ground < GROUND_DEEP_WATER
      && state->is_touching_ground()
      && (get_x() != last_solid_ground_coords.get_x() || get_y() != last_solid_ground_coords.get_y())) {

    // save the hero's last valid position
    last_solid_ground_coords.set_xy(get_xy());
    last_solid_ground_layer = get_layer();
  }
}

/**
 * @brief This function is called when the layer of this entity has just changed.
 */
void Hero::notify_layer_changed(void) {
  state->notify_layer_changed();
}

/**
 * @brief Updates the hero depending on its movement.
 *
 * This function is called when the hero's movement direction changes (for instance
 * because the player pressed or released a directional key, or the hero just reached an obstacle).
 * It updates the hero's animations and collisions according to the new movement.
 */
void Hero::notify_movement_changed(void) {

  // update the animation direction according to the movement direction
  int wanted_direction8 = get_wanted_movement_direction8();
  if (wanted_direction8 != -1) {

    int old_animation_direction = sprites->get_animation_direction();
    int animation_direction = sprites->get_animation_direction(wanted_direction8, get_real_movement_direction8());

    if (animation_direction != old_animation_direction
	&& animation_direction != -1
	&& !is_direction_locked()) {
      // if the direction defined by the directional keys has changed,
      // update the sprite's direction of animation
      // (unless the hero is loading his sword)
      sprites->set_animation_direction(animation_direction);
    }
  }

  // let the state pick the animation corresponding to the movement tried by the player
  state->notify_movement_changed();

  // check the collisions
  if (map != NULL) {
    notify_position_changed();
  }
}

/**
 * @brief Stops the movement of the player and lets the player restart it when he can.
 *
 * This function is typically called when the player loses temporarily the control
 * (e.g. because of a script or a teletransporter) whereas the movement remains the same.
 * Then the movement may want to move a few pixels more as soon as it is resumed.
 * This function removes such residual effects of the player's movement.
 * If the current movement is not controlled by the player, this function has no effect.
 */
void Hero::reset_movement(void) {

  if (state->can_control_movement()) {
    get_movement()->stop();
  }
}

/**
 * @brief Starts activating the ground specified by the last call to set_ground().
 */
void Hero::notify_ground_changed(void) {

  switch (ground) {

  case GROUND_NORMAL:
    // normal ground: remove any special sprite displayed under the hero
    sprites->destroy_ground();
    set_walking_speed(normal_walking_speed);
    break;

  case GROUND_DEEP_WATER:
    // deep water: plunge if the hero is not jumping
    if (!state->can_avoid_deep_water()) {
      start_deep_water();
    }
    break;

  case GROUND_HOLE:
    // hole: attract the hero towards the hole
    if (!state->can_avoid_hole()) {
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

      set_walking_speed(normal_walking_speed * 4 / 5);
    }
    break;

  case GROUND_LADDER:
    set_walking_speed(normal_walking_speed * 3 / 5);
    break;
  }

  // notify the state
  state->notify_ground_changed();
}

/**
 * @brief Returns the ground displayed under the hero.
 * @return ground the ground under the hero
 */
Ground Hero::get_ground(void) {
  return ground;
}

/**
 * @brief Sets the ground under the hero.
 *
 * This function is called when the hero walks on an entity
 * having a special ground.
 *
 * @param ground the ground to set
 */
void Hero::set_ground(Ground ground) {

  if (ground != this->ground) {
    this->ground = ground;
  }
  // we don't call notify_ground_changed() from here because set_ground()
  // may be called several times if several grounds are stacked on the
  // same location (e.g. one from a tile and one from a dynamic entity)
}

/**
 * @brief Returns whether the hero is in a state such that
 * a ground can be displayed under him.
 */
bool Hero::is_ground_visible(void) {

  return (ground == GROUND_GRASS || ground == GROUND_SHALLOW_WATER)
    && state->is_touching_ground();
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
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Hero::is_obstacle_for(MapEntity *other) {
  return other->is_hero_obstacle(this);
}

/**
 * @brief Returns whether a deep water tile is currently considered as an obstacle for the hero.
 * @return true if the deep water tiles are currently an obstacle for the hero
 */
bool Hero::is_water_obstacle(void) {
  return state->is_water_obstacle();
}

/**
 * @brief Returns whether a hole is currently considered as an obstacle for the hero.
 * @return true if the holes are currently an obstacle for the hero
 */
bool Hero::is_hole_obstacle(void) {
  return state->is_hole_obstacle();
}

/**
 * @brief Returns whether a ladder is currently considered as an obstacle for the hero.
 * @return true if the ladders are currently an obstacle for the hero
 */
bool Hero::is_ladder_obstacle(void) {
  return state->is_ladder_obstacle();
}

/**
 * @brief Returns whether a block is currently considered as an obstacle by this entity.
 * @param block a block
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Hero::is_block_obstacle(Block *block) {
  return block->is_hero_obstacle(this);
}

/**
 * @brief Returns whether a teletransporter is currently considered as an obstacle.
 *
 * This depends on the hero's state.
 *
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for the hero
 */
bool Hero::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return state->is_teletransporter_obstacle(teletransporter);
}

/**
 * @brief Returns whether a conveyor belt is currently considered as an obstacle for the hero.
 *
 * This depends on the hero's state.
 *
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool Hero::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return state->is_conveyor_belt_obstacle(conveyor_belt);
}

/**
 * @brief Returns whether some stairs are currently considered as an obstacle for this entity.
 * @param stairs an stairs entity
 * @return true if the stairs are currently an obstacle for this entity
 */
bool Hero::is_stairs_obstacle(Stairs *stairs) {
  return true;
}

/**
 * @brief Returns whether a sensor is currently considered as an obstacle for the hero.
 * @param sensor a sensor (not used here)
 * @return true if this sensor is currently an obstacle for the hero
 */
bool Hero::is_sensor_obstacle(Sensor *sensor) {
  return state->is_sensor_obstacle(sensor);
}

/**
 * @brief Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * @param raised_block a crystal switch block raised
 * @return true if the raised block is currently an obstacle for this entity
 */
bool Hero::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {
  return !is_on_raised_blocks();
}

/**
 * @brief Returns whether a non-diagonal jump sensor is currently considered as an obstacle for this entity.
 * @param jump_sensor a non-diagonal jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool Hero::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return !state->can_take_jump_sensor(); // if the jump sensors cannot be used in this state, consider them as obstacles
}

/**
 * @brief This function is called when a destructible item detects a non-pixel perfect collision with this entity.
 * @param destructible_item the destructible item
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_destructible_item(DestructibleItem *destructible_item, CollisionMode collision_mode) {
  destructible_item->notify_collision_with_hero(this, collision_mode);
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
 * @brief This function is called when a teletransporter detects a collision with the hero.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_teletransporter(Teletransporter *teletransporter, CollisionMode collision_mode) {

  if (teletransporter->is_on_map_side() || !state->can_avoid_teletransporter()) {

    bool on_hole = map->get_tile_ground(get_layer(), get_x(), get_y()) == GROUND_HOLE;
    if (on_hole || state->is_teletransporter_delayed()) {
      this->delayed_teletransporter = teletransporter; // fall into the hole (or do something else) first, transport later
    }
    else {
      teletransporter->transport_hero(this); // usual case: transport right now
    }
  }
}

/**
 * @brief Returns a teletransporter that has detected a collision with the hero
 * bu will be activated when the current action is finished
 * (e.g. falling into a hole or taking stairs).
 * @return the delayed teletransporter
 */
Teletransporter * Hero::get_delayed_teletransporter(void) {
  return delayed_teletransporter;
}

/**
 * @brief This function is called when a conveyor belt detects a collision with this entity.
 * @param conveyor_belt a conveyor belt
 * @param dx direction of the x move in pixels (0, 1 or -1)
 * @param dy direction of the y move in pixels (0, 1 or -1)
 */
void Hero::notify_collision_with_conveyor_belt(ConveyorBelt *conveyor_belt, int dx, int dy) {

  on_conveyor_belt = true;

  if (!state->can_avoid_conveyor_belt()) {

    // check that a significant part of the hero is on the conveyor belt
    Rectangle center = get_center_point();
    center.add_xy(-1, -1);
    center.set_size(2, 2);

    if (conveyor_belt->overlaps(center)) {

      // check that the movement is possible for at least one pixel
      Rectangle collision_box = get_bounding_box();
      collision_box.add_xy(dx, dy);
 
      if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	// move the hero
	set_state(new ConveyorBeltState(this, conveyor_belt));
      }
    }
  }
}

/**
 * @brief This function is called when a stairs entity detects a collision with this entity.
 * @param stairs the stairs
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_stairs(Stairs *stairs, CollisionMode collision_mode) {

  if (state->can_take_stairs()) {

    Stairs::Way stairs_way;
    if (stairs->is_inside_floor()) {
      stairs_way = (get_layer() == LAYER_LOW) ? Stairs::NORMAL_WAY : Stairs::REVERSE_WAY;
    }
    else {
      stairs_way = (collision_mode == COLLISION_FACING_POINT) ? Stairs::NORMAL_WAY : Stairs::REVERSE_WAY;
    }

    // check whether the hero is trying to move in the direction of the stairs
    int correct_direction = stairs->get_movement_direction(stairs_way);
    if (is_moving_towards(correct_direction / 2)) {
      set_state(new StairsState(this, stairs, stairs_way));
    }
  }
}

/**
 * @brief This function is called when a jump sensor detects a collision with this entity.
 * @param jump_sensor the jump sensor
 */
void Hero::notify_collision_with_jump_sensor(JumpSensor *jump_sensor) {
  
  if (state->can_take_jump_sensor()) {

    if (jump_sensor->get_direction() % 2 == 0) {
      // this non-diagonal jump sensor is not currently an obstacle for the hero
      // (in order to allow his smooth collision movement),
      // so the hero may have one pixel inside the sensor before jumping
      set_aligned_to_grid();
    }

    // jump
    start_jumping(jump_sensor->get_direction(), jump_sensor->get_jump_length(), true, true, 0, LAYER_LOW);
  }
}

/**
 * @brief This function is called when a sensor detects a collision with this entity.
 * @param sensor a sensor
 */
void Hero::notify_collision_with_sensor(Sensor *sensor) {

  if (!state->can_avoid_sensor()) {
    sensor->activate(this);
  }
}

/**
 * @brief This function is called when an explosion's sprite detects a collision with a sprite of the hero.
 * @param explosion the explosion
 * @param sprite_overlapping the sprite of the hero that collides with the explosion
 */
void Hero::notify_collision_with_explosion(Explosion *explosion, Sprite *sprite_overlapping) {

  if (!state->can_avoid_explosion()) {
    if (sprite_overlapping->contains("tunic")) {
      hurt(explosion, 2, 0);
    }
  }
}

/**
 * @brief This function is called when a crystal switch detects a collision with this entity.
 * @param crystal_switch the crystal switch
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_crystal_switch(CrystalSwitch *crystal_switch, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_FACING_POINT) {
    // the hero is touching the crystal switch and is looking in its direction

    KeysEffect *keys_effect = game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& is_free()) {

      // we show the action icon
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * @brief This function is called when a the sprite of a crystal switch 
 * detects a pixel-perfect collision with a sprite of this entity.
 * @param crystal_switch the crystal switch
 * @param sprite_overlapping the sprite of the current entity that collides with the crystal switch
 */
void Hero::notify_collision_with_crystal_switch(CrystalSwitch *crystal_switch, Sprite *sprite_overlapping) {

  if (sprite_overlapping->contains("sword") // the hero's sword is overlapping the crystal switch
      && state->can_sword_hit_crystal_switch()) {
    
    crystal_switch->activate(this);
  }
}

/**
 * @brief Makes the hero escape from an entity that is overlapping it.
 *
 * This function is called when an entity that just appeared may overlap the hero
 * (e.g. a chest or a door).
 *
 * @param entity the entity that just appeared
 * @param direction the direction of the hero relative to the entity
 * (the hero will be moved into this direction): 0 to 3
 */
void Hero::avoid_collision(MapEntity *entity, int direction) {

  // fix the hero's position, whatever the entity size is
  switch (direction) {

    case 0:
      set_top_left_x(entity->get_top_left_x() + entity->get_width());
      set_y(entity->get_center_point().get_y());
      break;

    case 1:
      set_top_left_y(entity->get_top_left_y() - this->get_height());
      set_x(entity->get_center_point().get_x());
      break;

    case 2:
      set_top_left_x(entity->get_top_left_x() - this->get_width());
      set_y(entity->get_center_point().get_y());
      break;

    case 3:
      set_top_left_y(entity->get_top_left_y() + entity->get_height());
      set_x(entity->get_center_point().get_x());
      break;

    default:
      Debug::die(StringConcat() << "Invalid direction in Hero::avoid_collision(): " << direction);
      break;
  }
  reset_movement();
}

/**
 * @brief Notifies the hero that the entity he is pushing or pulling
 * cannot move anymore because of a collision.
 */
void Hero::notify_grabbed_entity_collision(void) {
  state->notify_grabbed_entity_collision();
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 *
 * When the sword sprite collides with a detector,
 * this function can be called to determine whether the hero is
 * really cutting this particular detector precisely.
 * This depends on the hero's state, his direction and his
 * distance to the detector.
 * This function assumes that there is already a collision
 * between the sword sprite and the detector's sprite.
 * This function should be called to check whether the
 * hero wants to cut a bush or some grass.
 * Returns false by default.
 *
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::is_striking_with_sword(Detector *detector) {
  return state->is_cutting_with_sword(detector);
}

/**
 * @brief Snaps the hero to the entity he is facing.
 *
 * The hero is snapped if there is no collision and if he is not too far.
 */
void Hero::try_snap_to_facing_entity(void) {

  Rectangle collision_box = get_bounding_box();

  if (get_animation_direction() % 2 == 0) {
    if (abs(collision_box.get_y() - facing_entity->get_top_left_y()) <= 5) {
      collision_box.set_y(facing_entity->get_top_left_y());
    }
  }
  else {
    if (abs(collision_box.get_x() - facing_entity->get_top_left_x()) <= 5) {
      collision_box.set_x(facing_entity->get_top_left_x());
    }
  }

  if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
    set_bounding_box(collision_box);
    notify_position_changed();
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
void Hero::notify_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed) {

  state->notify_attacked_enemy(attack, victim, result, killed);
}

/**
 * @brief Returns the damage power of the sword for the current attack.
 *
 * The value returned takes into account the power of the current sword
 * and the fact that a spin attack is more powerful than other attacks.
 *
 * @return the current damage factor of the sword
 */
int Hero::get_sword_damage_factor(void) {

  return state->get_sword_damage_factor();
}

/**
 * @brief Hurts the hero if possible.
 * @param source the entity that hurts the hero (usually an enemy)
 * @param life_points number of heart quarters to remove (this number may be reduced by the tunic)
 * @param magic_points number of magic points to remove
 */
void Hero::hurt(MapEntity *source, int life_points, int magic_points) {

  if (!sprites->is_blinking() && state->can_be_hurt()) {
    set_state(new HurtState(this, source, life_points, magic_points));
  }
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
 * @brief Makes the hero drown or swim.
 */
void Hero::start_deep_water(void) {

  if (!state->is_touching_ground()) {
    // plunge into the water
    set_state(new PlungingState(this));
  }
  else {
    // move to state swimming or jumping
    if (equipment->has_ability("swim")) {
      set_state(new SwimmingState(this));
    }
    else {
      start_jumping(get_wanted_movement_direction8(), 32, false, true, 13);
    }
  }
}

/**
 * @brief Makes the hero move towards a hole of fall into it.
 */
void Hero::start_hole(void) {

  delayed_teletransporter = NULL;

  if (!state->can_control_movement()) {
    // the player has no control (e.g. he is running or being hurt):
    // fall immediately
    set_state(new FallingState(this));
  }
  else {
    // otherwise, push the hero towards the hole

    next_ground_date = System::now();

    /* version where the attraction direction is calculated based on the wanted movement:
     * problem because the wanted movement may be different from the real one
    if (is_moving_towards(0)) {
      hole_dxy.set_x(1);
    }
    else if (is_moving_towards(2)) {
      hole_dxy.set_x(-1);
    }
    else {
      hole_dxy.set_x(0);
    }

    if (is_moving_towards(1)) {
      hole_dxy.set_y(-1);
    }
    else if (is_moving_towards(3)) {
      hole_dxy.set_y(1);
    }
    else {
      hole_dxy.set_y(0);
    }

    if (hole_dxy.get_x() == 0 && hole_dxy.get_y() == 0) {
      // fall immediately because the hero was not moving but directly placed on the hole
      set_state(new FallingState(this));
    }
    else {
      set_walking_speed(normal_walking_speed / 3);
    }
    */

    if (last_solid_ground_coords.get_x() == -1 ||
	(last_solid_ground_coords.get_x() == get_x() && last_solid_ground_coords.get_y() == get_y())) {
      // fall immediately because the hero was not moving but directly placed on the hole
      set_state(new FallingState(this));
    }
    else {

      hole_dxy.set_xy(0, 0);

      if (get_x() > last_solid_ground_coords.get_x()) {
        hole_dxy.set_x(1);
      }
      else if (get_x() < last_solid_ground_coords.get_x()) {
        hole_dxy.set_x(-1);
      }

      if (get_y() > last_solid_ground_coords.get_y()) {
        hole_dxy.set_y(1);
      }
      else if (get_y() < last_solid_ground_coords.get_y()) {
        hole_dxy.set_y(-1);
      }
      set_walking_speed(normal_walking_speed / 3);
    }
  }
}

/**
 * @brief Returns whether the hero can walk normally and interact with entities.
 * @return true if the hero can walk normally
 */
bool Hero::is_free(void) {
  return state->is_free();
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 *
 * If he is not grabbing any entity, false is returned.
 *
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::is_moving_grabbed_entity(void) {
  return state->is_moving_grabbed_entity();
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity.
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::is_grabbing_or_pulling(void) {
  return state->is_grabbing_or_pulling();
}

/**
 * @brief Lets the hero walk normally.
 */
void Hero::start_free(void) {

  if (!state->is_free()) {
    set_state(new FreeState(this));
  }
}

/**
 * @brief Makes the hero brandish a treasure.
 * @param treasure the treasure to give him (you have to delete it after the hero brandishes it)
 */
void Hero::start_treasure(Treasure *treasure) {
  set_state(new TreasureState(this, treasure));
}

/**
 * @brief Makes the hero jump into a direction.
 *
 * While he is jumping, the player does not control him anymore.
 *
 * @param direction8 direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param ignore_obstacles true make the movement ignore obstacles
 * @param with_sound true to play the "jump" sound
 * @param movement_delay delay between each one-pixel move in the jump movement in milliseconds (0: default)
 * @param layer_after_jump the layer to set when the jump is finished
 * (or LAYER_NB to keep the same layer)
 */
void Hero::start_jumping(int direction8, int length, bool ignore_obstacles, bool with_sound, uint32_t movement_delay, Layer layer_after_jump) {

  JumpingState *state = new JumpingState(this, direction8, length, ignore_obstacles, with_sound, movement_delay, layer_after_jump);
  set_state(state);
}

/**
 * @brief Makes the hero brandish his sword meaning a victory.
 */
void Hero::start_victory() {
  set_state(new VictoryState(this));
}

/**
 * @brief Freezes the hero.
 *
 * When the hero is freezed, he cannot move.
 * The current animation of the hero's sprites is stopped and the "stopped" animation is played.
 * You can call start_free() to unfreeze him.
 */
void Hero::start_freezed(void) {
  set_state(new FreezedState(this));
}

/**
 * @brief Makes the hero lift a destructible item.
 * @param item_to_lift the destructible item to lift
 */
void Hero::start_lifting(DestructibleItem *item_to_lift) {
  set_state(new LiftingState(this, item_to_lift));
}

/**
 * @brief Starts running with the speed shoes.
 */
void Hero::start_running(void) {
  set_state(new RunningState(this));
}

/**
 * @brief Starts grabbing an obstacle.
 */
void Hero::start_grabbing(void) {
  set_state(new GrabbingState(this));
}

