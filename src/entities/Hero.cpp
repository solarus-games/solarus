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
#include "entities/Switch.h"
#include "entities/Crystal.h"
#include "entities/Chest.h"
#include "entities/Block.h"
#include "entities/Jumper.h"
#include "entities/Sensor.h"
#include "entities/Bomb.h"
#include "hero/HeroSprites.h"
#include "hero/CarryingState.h"
#include "hero/ConveyorBeltState.h"
#include "hero/FallingState.h"
#include "hero/FreeState.h"
#include "hero/FreezedState.h"
#include "hero/GrabbingState.h"
#include "hero/HurtState.h"
#include "hero/JumpingState.h"
#include "hero/ForcedWalkingState.h"
#include "hero/LiftingState.h"
#include "hero/PlungingState.h"
#include "hero/BackToSolidGroundState.h"
#include "hero/RunningState.h"
#include "hero/StairsState.h"
#include "hero/SwimmingState.h"
#include "hero/TreasureState.h"
#include "hero/VictoryState.h"
#include "hero/BoomerangState.h"
#include "hero/HookshotState.h"
#include "hero/BowState.h"
#include "movements/StraightMovement.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"
#include "Game.h"
#include "Map.h"
#include "Equipment.h"
#include "KeysEffect.h"
#include "Sprite.h"

const int Hero::normal_walking_speed = 88; // pixels per second

/**
 * @brief Creates a hero.
 * @param equipment the equipment (needed to build the sprites even outside a game)
 */
Hero::Hero(Equipment &equipment):

  state(NULL),
  walking_speed(normal_walking_speed),
  on_conveyor_belt(false),
  on_raised_blocks(false),
  ground(GROUND_NORMAL),
  next_ground_date(0) {

  // position
  set_size(16, 16);
  set_origin(8, 13);
  last_solid_ground_coords.set_xy(-1, -1);
  last_solid_ground_layer = LAYER_LOW;
 
  // sprites
  sprites = new HeroSprites(*this, equipment);
  rebuild_equipment();

  // state
  set_state(new FreeState(*this));
}

/**
 * @brief Destructor.
 */
Hero::~Hero() {

  delete sprites;
  delete state;

  std::list<State*>::iterator it;
  for (it = old_states.begin(); it != old_states.end(); it++) {
    delete *it;
  }
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
bool Hero::can_be_obstacle() {
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
bool Hero::can_detect_entities() {
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
bool Hero::can_be_displayed() {
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
bool Hero::is_displayed_in_y_order() {
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
void Hero::set_state(State* new_state) {

  // stop the previous state
  State* old_state = this->state;
  if (old_state != NULL) {
    old_state->stop(new_state);
  }

  // don't delete the previous state immediately since it may be the caller of this function
  this->old_states.push_back(old_state);

  this->state = new_state;
  this->state->start(old_state);

  check_position();
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
  }

  sprites->set_suspended(suspended);
  state->set_suspended(suspended);
}

/**
 * @brief Updates the hero's position, movement and animation.
 *
 * This function is called repeteadly by the game loop.
 */
void Hero::update() {

  update_movement();
  update_state();
  sprites->update();
  if (!is_suspended()) {
    update_ground();
    get_map().check_collision_with_detectors(*this);
    check_gameover();
  }
}

/**
 * @brief Updates the hero's internal state.
 *
 * This function is called repeatedly by update().
 */
void Hero::update_state() {

  // update the current state
  state->update();

  // see if there is old states to cleanup
  std::list<State*>::iterator it;
  for (it = old_states.begin(); it != old_states.end(); it++) {
    delete *it;
  }
  old_states.clear();
}

/**
 * @brief Updates the hero's position according to its movement.
 *
 * This function is called repeatedly by update().
 */
void Hero::update_movement() {

  on_raised_blocks = get_entities().overlaps_raised_blocks(get_layer(), get_bounding_box());

  if (get_movement() == NULL) {
    return;
  }
  get_movement()->update();
}

/**
 * @brief Updates the ground under the hero.
 *
 * This function is called repeatedly.
 */
void Hero::update_ground() {

  // see if it's time to do something (depending on the ground)
  uint32_t now = System::now();
  if (now >= next_ground_date) {

    if (is_ground_visible() && get_movement() != NULL) {

      // a special ground is displayed under the hero and it's time to play a sound
      double speed = ((StraightMovement*) get_movement())->get_speed();
      next_ground_date = now + std::max(150, (int) (20000 / speed));
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
        set_state(new FallingState(*this));
      }
      else {

        // not too far yet
        bool moved = false;
        Rectangle collision_box = get_bounding_box();
        collision_box.add_xy(hole_dxy);

        if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {
          set_bounding_box(collision_box);
          notify_position_changed();
          moved = true;
        }

        if (!moved && hole_dxy.get_x() != 0) { // try x only
          collision_box = get_bounding_box();
          collision_box.add_xy(hole_dxy.get_x(), 0);
          if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {
            set_bounding_box(collision_box);
            notify_position_changed();
            moved = true;
          }
        }

        if (!moved && hole_dxy.get_y() != 0) { // try y only
          collision_box = get_bounding_box();
          collision_box.add_xy(0, hole_dxy.get_y());
          if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {
            set_bounding_box(collision_box);
            notify_position_changed();
            moved = true;
          }
        }

        if (!moved) {
          // the hero cannot be moved towards the direction previously calculated
          set_walking_speed(normal_walking_speed);
          set_state(new FallingState(*this));
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
void Hero::check_gameover() {

  if (get_equipment().get_life() <= 0 && state->can_start_gameover_sequence()) {
    sprites->stop_blinking();
    get_game().start_gameover_sequence();
  }
}

/**
 * @brief Displays this entity on the map.
 *
 * This function should draw the entity only if is_visible() returns true.
 * The hero is displayed with its current animation and at its current position.
 */
void Hero::display_on_map() {

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
HeroSprites& Hero::get_sprites() {
  return *sprites;
}

/**
 * @brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * @return the direction of the sprites (0 to 3)
 */
int Hero::get_animation_direction() {
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
bool Hero::is_animation_finished() {
  return sprites->is_animation_finished();
}

/**
 * @brief Loads (or reloads) the sprites and sounds of the hero and his equipment.
 *
 * The sprites and sounds depend on its tunic, sword and shield as specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as the hero's equipment is changed.
 */
void Hero::rebuild_equipment() {

  sprites->rebuild_equipment();
}

/**
 * @brief Returns whether this entity is currently visible.
 * @return true if this entity is currently visible
 */
bool Hero::is_visible() {

  return MapEntity::is_visible()
    && state->is_hero_visible()
    && !get_game().is_showing_gameover();
}

/**
 * @brief Returns whether the shadow should be currently displayed, separate from the tunic sprite.
 * @return true if the shadow should be currently displayed.
 */
bool Hero::is_shadow_visible() {
  return get_displayed_xy().get_y() != get_y();
}

/**
 * @brief Sets the hero's current map.
 *
 * This function is called when the map is changed.
 * Warning: as this function is called when initializing the map,
 * the current map of the game is still the old one.
 *
 * @param map the map
 */
void Hero::set_map(Map &map) {

  MapEntity::set_map(map);

  last_solid_ground_coords.set_xy(-1, -1);
  target_solid_ground_coords.set_xy(-1, -1);
  ground = GROUND_NORMAL;
  get_sprites().set_clipping_rectangle();

  state->set_map(map);
}

/**
 * @brief Notifies this entity that its map has just become active.
 */
void Hero::notify_map_started() {

  MapEntity::notify_map_started();
  get_sprites().notify_map_started();
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
void Hero::set_map(Map &map, int initial_direction) {

  // take the specified direction
  if (initial_direction != -1) {
    sprites->set_animation_direction(initial_direction);
  }

  set_map(map);
}

/**
 * @brief Places the hero on the map specified and at its destination point selected.
 * @param map the new map
 * @param previous_map_location position of the previous map in its world
 * (may be needed for scrolling transitions, but the previous map is already destroyed)
 */
void Hero::place_on_destination_point(Map& map, const Rectangle& previous_map_location) {

  const std::string &destination_point_name = map.get_destination_point_name();

  if (destination_point_name == "_same") {

    // the hero's coordinates are the same as on the previous map
    // but we may have to change the layer
    
    Layer layer = LAYER_INTERMEDIATE;
    if (map.get_entities().get_obstacle_tile(LAYER_INTERMEDIATE, get_x(), get_y()) == OBSTACLE_EMPTY) {
      layer = LAYER_LOW;
    }
    set_map(map);
    last_solid_ground_coords = get_xy();
    map.get_entities().set_entity_layer(this, layer);

    start_free();
  }
  else {
    int side = map.get_destination_side();

    if (side != -1) {

      // go to a side of the other map
      set_map(map);

      switch (side) {

      case 0: // right side
        set_x(map.get_width());
        set_y(get_y() - map.get_location().get_y() + previous_map_location.get_y());
        break;

      case 1: // top side
        set_y(5);
        set_x(get_x() - map.get_location().get_x() + previous_map_location.get_x());
        break;

      case 2: // left side
        set_x(0);
        set_y(get_y() - map.get_location().get_y() + previous_map_location.get_y());
        break;

      case 3: // bottom side
        set_y(map.get_height() + 5);
        set_x(get_x() - map.get_location().get_x() + previous_map_location.get_x());
        break;

      default:
        Debug::die(StringConcat() << "Invalid destination side: " << side);
      }
      last_solid_ground_coords = get_xy();
      // note that we keep the hero's state from the previous map
    }
    else {

      // normal case: the location is specified by a destination point object

      DestinationPoint *destination_point = (DestinationPoint*)
	    map.get_entities().get_entity(DESTINATION_POINT, destination_point_name);

      set_map(map, destination_point->get_direction());
      set_xy(destination_point->get_x(), destination_point->get_y());
      last_solid_ground_coords = get_xy();
      map.get_entities().set_entity_layer(this, destination_point->get_layer());

      map.get_entities().remove_boomerang(); // useful when the map remains the same

      Stairs *stairs = get_stairs_overlapping();
      if (stairs != NULL) {
        // the hero arrived on the map by stairs
        set_state(new StairsState(*this, *stairs, Stairs::REVERSE_WAY));
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
void Hero::notify_map_opening_transition_finished() {

  MapEntity::notify_map_opening_transition_finished();

  int side = get_map().get_destination_side();  
  if (side != -1) {
    // the hero was placed on the side of the map:
    // there was a scrolling between the previous map and this one

    switch (side) {

    case 0: // right side
      set_x(get_map().get_width() - 8);
      break;

    case 1: // top side
      set_y(13);
      break;

    case 2: // left side
      set_x(8);
      break;

    case 3: // bottom side
      set_y(get_map().get_height() - 3);
      break;

    default:
      Debug::die(StringConcat() << "Invalid destination side: " << side);
    }
  }
  check_position();
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
const Rectangle Hero::get_facing_point() {

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
  const Rectangle& bounding_box = get_bounding_box();

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
 * @brief Notifies this entity that its facing entity has just changed.
 * @param facing_entity the detector this entity is now facing (possibly NULL)
 */
void Hero::notify_facing_entity_changed(Detector* facing_entity) {

  if (facing_entity == NULL &&
      get_keys_effect().is_action_key_acting_on_facing_entity()) {

    // the hero just stopped facing an entity that was showing an action icon
    get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
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
bool Hero::is_facing_obstacle() {

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

  return get_map().test_collision_with_obstacles(get_layer(), collision_box, *this);
}

/**
 * @brief Returns whether the facing point of the hero is overlapping an obstacle.
 *
 * This function returns whether his facing point is overlapping an obstacle of the map.
 * This information is calculated and not stored, so it is always up to date.
 *
 * @return true if the facing point is overlapping an obstacle
 */
bool Hero::is_facing_point_on_obstacle() {

  const Rectangle &facing_point = get_facing_point();
  return get_map().test_collision_with_obstacles(get_layer(), facing_point.get_x(), facing_point.get_y(), *this);
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
 * @brief Returns whether the hero is currently on raised crystal blocks.
 * @return true if the hero is currently on raised crystal blocks
 */
bool Hero::is_on_raised_blocks() {
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
Stairs* Hero::get_stairs_overlapping() {

  std::list<Stairs*> all_stairs = get_entities().get_stairs(get_layer());
  std::list<Stairs*>::iterator it;
  for (it = all_stairs.begin(); it != all_stairs.end(); it++) {

    Stairs *stairs = *it;

    if (overlaps(*stairs)) {
      return stairs;
    }
  }

  return NULL;
}

/**
 * @brief Returns whether the player can control his movements in the current state.
 * @return true if the player can control his movements
 */
bool Hero::can_control_movement() {
  return state->can_control_movement();
}

/**
 * @brief Returns the default walking speed applied to the hero's movements when he is walking.
 * @return the default walking speed
 */
int Hero::get_normal_walking_speed() {
  return normal_walking_speed;
}

/**
 * @brief Returns the current speed applied to the hero's movements when he is walking.
 * @return the current walking speed
 */
int Hero::get_walking_speed() {
  return walking_speed;
}

/**
 * @brief Sets the speed to apply to the hero's movements when he is walking.
 * @param walking_speed the new walking speed
 */
void Hero::set_walking_speed(int walking_speed) {

  if (walking_speed != this->walking_speed) {
    this->walking_speed = walking_speed;
    state->notify_walking_speed_changed();
  }
}

/**
 * @brief Returns the direction of the hero's movement as defined by the controls applied by the player.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * @return the hero's wanted direction between 0 and 7, or -1 if he is stopped
 */
int Hero::get_wanted_movement_direction8() {
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
int Hero::get_real_movement_direction8() {

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
    if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {
      result = wanted_direction8;
    }
    else {
      // otherwise, see if he can move in one of the two closest directions (i.e. he is sliding)

      int alternative_direction8 = (wanted_direction8 + 1) % 8;
      collision_box = get_bounding_box();
      xy_move = direction_to_xy_move(alternative_direction8);
      collision_box.add_xy(xy_move.get_x(), xy_move.get_y());
      if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {
        result = alternative_direction8;
      }
      else {
        alternative_direction8 = (wanted_direction8 - 1) % 8;
        collision_box = get_bounding_box();
        xy_move = direction_to_xy_move(alternative_direction8);
        collision_box.add_xy(xy_move.get_x(), xy_move.get_y());
        if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {
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

  return movement_direction8 != -1
      && (movement_direction8 == direction8
          || (movement_direction8 + 1) % 8 == direction8
          || (movement_direction8 + 7) % 8 == direction8);
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
bool Hero::is_direction_locked() {
  return state->is_direction_locked();
}

/**
 * @brief Notifies this entity that it has just failed to change its position
 * because of obstacles.
 */
void Hero::notify_obstacle_reached() {

  MapEntity::notify_obstacle_reached();

  state->notify_obstacle_reached();
}

/**
 * @brief This function is called when the hero's position is changed.
 */
void Hero::notify_position_changed() {

  check_position();
}

/**
 * @brief Checks collisions with detectors, determines the facing entity
 * and the ground below the hero in its current position.
 *
 * This function is called when these checks have to be done again,
 * e.g. when the position, the direction or the state of the hero changes.
 */
void Hero::check_position() {

  if (!is_on_map()) {
    return;
  }

  if (state->are_collisions_ignored()) {
    // do not take care of the ground or detectors
    return;
  }

  // the facing entity has to be recomputed
  set_facing_entity(NULL);

  // save the current ground
  Ground previous_ground = this->ground;

  // see the ground indicated by the non-dynamic tiles
  if (!is_suspended()) { // when suspended, the hero may have invalid coordinates
                         // (e.g. transition between maps)

    this->ground = GROUND_EMPTY;
    Ground tiles_ground = get_tile_ground();
    set_ground(tiles_ground);
  }

  // see the ground indicated by the dynamic entities, also find the facing entity
  check_collision_with_detectors();

  if (this->ground != previous_ground) {
    notify_ground_changed();
  }

  // save the hero's last valid position
  if (ground != GROUND_DEEP_WATER
      && ground != GROUND_HOLE
      && ground != GROUND_LAVA
      && ground != GROUND_PRICKLE
      && ground != GROUND_EMPTY
      && state->can_come_from_bad_ground()
      && (get_x() != last_solid_ground_coords.get_x() || get_y() != last_solid_ground_coords.get_y())) {

    last_solid_ground_coords.set_xy(get_xy());
    last_solid_ground_layer = get_layer();
  }

  // with empty ground, possibly go to the lower layer
  if (ground == GROUND_EMPTY && state->is_touching_ground()) {

    int x = get_top_left_x();
    int y = get_top_left_y();
    Layer layer = get_layer();
    MapEntities& entities = get_entities();

    if (layer > LAYER_LOW
        && entities.get_obstacle_tile(layer, x, y) == OBSTACLE_EMPTY
        && entities.get_obstacle_tile(layer, x + 15, y) == OBSTACLE_EMPTY
        && entities.get_obstacle_tile(layer, x, y + 15) == OBSTACLE_EMPTY
        && entities.get_obstacle_tile(layer, x + 15, y + 15) == OBSTACLE_EMPTY) {

      get_entities().set_entity_layer(this, Layer(layer - 1));
      if (state->is_free() && get_tile_ground() == GROUND_NORMAL) {
        Sound::play("hero_lands");
      }
    }
  }
}

/**
 * @brief This function is called when the layer of this entity has just changed.
 */
void Hero::notify_layer_changed() {
  state->notify_layer_changed();
}

/**
 * @brief Updates the hero depending on its movement.
 *
 * This function is called when the hero's movement direction changes (for instance
 * because the player pressed or released a directional key, or the hero just reached an obstacle).
 * It updates the hero's animations and collisions according to the new movement.
 */
void Hero::notify_movement_changed() {

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
  check_position();
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
void Hero::reset_movement() {

  if (state->can_control_movement()) {
    get_movement()->stop();
  }
}

/**
 * @brief Starts activating the ground specified by the last call to set_ground().
 */
void Hero::notify_ground_changed() {

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

  case GROUND_LAVA:
    // lava: plunge into lava
    if (!state->can_avoid_lava()) {
      start_lava();
    }
    break;

  case GROUND_PRICKLE:
    // prickles
    if (!state->can_avoid_prickle()) {
      start_prickle(500);
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

  case GROUND_EMPTY:
    break;
  }

  // notify the state
  state->notify_ground_changed();
}

/**
 * @brief Returns the ground displayed under the hero.
 * @return ground the ground under the hero
 */
Ground Hero::get_ground() {
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

  if (ground != this->ground && ground != GROUND_EMPTY) {
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
bool Hero::is_ground_visible() {

  return (ground == GROUND_GRASS || ground == GROUND_SHALLOW_WATER)
    && state->is_touching_ground();
}

/**
 * @brief Returns the type of ground of the tile below the hero.
 *
 * Only static tiles are considered by this function
 * (dynamic tiles are ignored).
 *
 * @return the type of ground of the tile below the hero
 */
Ground Hero::get_tile_ground() {
  return get_map().get_tile_ground(get_layer(), get_ground_point());
}

/**
 * @brief Returns the coordinates of the point used to determine the ground
 * below the hero.
 * @return the coordinates of the point used to determine the ground
 * (relative to the map)
 */
const Rectangle Hero::get_ground_point() {
  // we must return here the same coordinates as dynamic tiles
  // (see DynamicTile::test_collision_custom)
  return Rectangle(get_x(), get_y() - 2, 1, 1);
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
 * @brief Forgets the point of the map where the hero was supposed to go back
 * if he falls into a hole or some other bad ground.
 *
 * The hero will now get back to the last solid ground instead of going back
 * to a memorized position.
 *
 * @param target_solid_ground_coords coordinates of the position where
 * the hero will go if he falls into a hole or some other bad ground
 * @param layer the layer
 */
void Hero::reset_target_solid_ground_coords() {

  this->target_solid_ground_coords.set_xy(-1, -1);
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Hero::is_obstacle_for(MapEntity &other) {
  return other.is_hero_obstacle(*this);
}

/**
 * @brief Returns whether a deep water tile is currently considered as an obstacle for the hero.
 * @return true if the deep water tiles are currently an obstacle for the hero
 */
bool Hero::is_deep_water_obstacle() {
  return state->is_deep_water_obstacle();
}

/**
 * @brief Returns whether a hole is currently considered as an obstacle for the hero.
 * @return true if the holes are currently an obstacle for the hero
 */
bool Hero::is_hole_obstacle() {
  return state->is_hole_obstacle();
}

/**
 * @brief Returns whether lava is currently considered as an obstacle for the hero.
 * @return true if lava is currently an obstacle for the hero
 */
bool Hero::is_lava_obstacle() {
  return state->is_lava_obstacle();
}

/**
 * @brief Returns whether prickles are currently considered as an obstacle for the hero.
 * @return true if prickles are currently an obstacle for the hero
 */
bool Hero::is_prickle_obstacle() {
  return state->is_prickle_obstacle();
}

/**
 * @brief Returns whether a ladder is currently considered as an obstacle for the hero.
 * @return true if the ladders are currently an obstacle for the hero
 */
bool Hero::is_ladder_obstacle() {
  return state->is_ladder_obstacle();
}

/**
 * @brief Returns whether a block is currently considered as an obstacle by this entity.
 * @param block a block
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Hero::is_block_obstacle(Block &block) {
  return block.is_hero_obstacle(*this);
}

/**
 * @brief Returns whether a teletransporter is currently considered as an obstacle.
 *
 * This depends on the hero's state.
 *
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for the hero
 */
bool Hero::is_teletransporter_obstacle(Teletransporter &teletransporter) {
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
bool Hero::is_conveyor_belt_obstacle(ConveyorBelt &conveyor_belt) {
  return state->is_conveyor_belt_obstacle(conveyor_belt);
}

/**
 * @brief Returns whether some stairs are currently considered as an obstacle for this entity.
 * @param stairs an stairs entity
 * @return true if the stairs are currently an obstacle for this entity
 */
bool Hero::is_stairs_obstacle(Stairs& stairs) {
  return state->is_stairs_obstacle(stairs);
}

/**
 * @brief Returns whether a sensor is currently considered as an obstacle for the hero.
 * @param sensor a sensor (not used here)
 * @return true if this sensor is currently an obstacle for the hero
 */
bool Hero::is_sensor_obstacle(Sensor &sensor) {
  return state->is_sensor_obstacle(sensor);
}

/**
 * @brief Returns whether a raised crystal block is currently considered as an obstacle for this entity.
 * @param raised_block a crystal block raised
 * @return true if the raised block is currently an obstacle for this entity
 */
bool Hero::is_raised_block_obstacle(CrystalBlock &raised_block) {
  return !is_on_raised_blocks();
}

/**
 * @brief Returns whether a non-diagonal jumper is currently considered as an obstacle for this entity.
 * @param jumper a non-diagonal jumper
 * @return true if the jumper is currently an obstacle for this entity
 */
bool Hero::is_jumper_obstacle(Jumper& jumper) {
  return state->is_jumper_obstacle(jumper);
}

/**
 * @brief This function is called when a destructible item detects a non-pixel perfect collision with this entity.
 * @param destructible_item the destructible item
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_destructible_item(DestructibleItem &destructible_item, CollisionMode collision_mode) {
  destructible_item.notify_collision_with_hero(*this, collision_mode);
}

/**
 * @brief This function is called when the rectangle of an enemy collides with the hero.
 * @param enemy the enemy
 */
void Hero::notify_collision_with_enemy(Enemy &enemy) {
  // hurt the hero only on pixel-precise collisions
}

/**
 * @brief This function is called when an enemy's sprite collides with a sprite of the hero.
 * @param enemy the enemy
 * @param enemy_sprite the enemy's sprite that overlaps a sprite of the hero
 * @param this_sprite the hero's sprite that overlaps the enemy's sprite
 */
void Hero::notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite) {

  if (this_sprite.contains("sword")) {
    // the hero's sword overlaps the enemy
    enemy.try_hurt(ATTACK_SWORD, *this, &enemy_sprite);
  }
  else if (this_sprite.contains("tunic")) {
    // the hero's body overlaps the enemy: ensure that the 16*16 rectangle of the hero also overlaps the enemy
    Rectangle enemy_sprite_rectangle = enemy_sprite.get_size();
    const Rectangle &enemy_sprite_origin = enemy_sprite.get_origin();
    enemy_sprite_rectangle.set_x(enemy.get_x() - enemy_sprite_origin.get_x());
    enemy_sprite_rectangle.set_y(enemy.get_y() - enemy_sprite_origin.get_y());

    if (overlaps(enemy_sprite_rectangle)) {
      enemy.attack_hero(*this, &enemy_sprite);
    }
  }
}

/**
 * @brief This function is called when a teletransporter detects a collision with the hero.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_teletransporter(Teletransporter &teletransporter, CollisionMode collision_mode) {

  if (teletransporter.is_on_map_side() || !state->can_avoid_teletransporter()) {

    bool on_hole = get_ground() == GROUND_HOLE;
    if (on_hole || state->is_teletransporter_delayed()) {
      this->delayed_teletransporter = &teletransporter; // fall into the hole (or do something else) first, transport later
    }
    else {
      teletransporter.transport_hero(*this); // usual case: transport right now
    }
  }
}

/**
 * @brief Returns a teletransporter that has detected a collision with the hero
 * bu will be activated when the current action is finished
 * (e.g. falling into a hole or taking stairs).
 * @return the delayed teletransporter
 */
Teletransporter* Hero::get_delayed_teletransporter() {
  return delayed_teletransporter;
}

/**
 * @brief This function is called when a conveyor belt detects a collision with this entity.
 * @param conveyor_belt a conveyor belt
 * @param dx direction of the x move in pixels (0, 1 or -1)
 * @param dy direction of the y move in pixels (0, 1 or -1)
 */
void Hero::notify_collision_with_conveyor_belt(ConveyorBelt &conveyor_belt, int dx, int dy) {

  on_conveyor_belt = true;

  if (!state->can_avoid_conveyor_belt()) {

    // check that a significant part of the hero is on the conveyor belt
    Rectangle center = get_center_point();
    center.add_xy(-1, -1);
    center.set_size(2, 2);

    if (conveyor_belt.overlaps(center)) {

      // check that the hero can go in the conveyor belt's direction
      // (otherwise the hero would be trapped forever if there
      // is an obstacle 8 pixels after the conveyor belt)
      Rectangle collision_box(0, 0, 16, 16);
      if (dx != 0) { // horizontal conveyor belt
        collision_box.set_xy(get_top_left_x() + dx,
            conveyor_belt.get_top_left_y());
      }
      else { // vertical conveyor belt
        collision_box.set_xy(conveyor_belt.get_top_left_x(),
            get_top_left_y() + dy);
      }

      if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {

        // check that the conveyor belt's exit is clear
        // (otherwise the hero could not take a blocked conveyor belt the reverse way)
        collision_box.set_xy(conveyor_belt.get_bounding_box());
        collision_box.add_xy(dx, dy);

        if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {

          // move the hero
          set_state(new ConveyorBeltState(*this, conveyor_belt));
        }
      }
    }
  }
}

/**
 * @brief This function is called when a stairs entity detects a collision with this entity.
 * @param stairs the stairs
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_stairs(Stairs &stairs, CollisionMode collision_mode) {

  if (get_ground() == GROUND_EMPTY) {
    set_ground(GROUND_NORMAL); // allow the hero to stay on the intermediate layer
  }

  if (state->can_take_stairs()) {

    Stairs::Way stairs_way;
    if (stairs.is_inside_floor()) {
      stairs_way = (get_layer() == LAYER_LOW) ? Stairs::NORMAL_WAY : Stairs::REVERSE_WAY;
    }
    else {
      stairs_way = (collision_mode == COLLISION_FACING_POINT_ANY) ? Stairs::NORMAL_WAY : Stairs::REVERSE_WAY;
    }

    // check whether the hero is trying to move in the direction of the stairs
    int correct_direction = stairs.get_movement_direction(stairs_way);
    if (is_moving_towards(correct_direction / 2)) {
      set_state(new StairsState(*this, stairs, stairs_way));
    }
  }
}

/**
 * @brief This function is called when a jumper detects a collision with this entity.
 * @param jumper the jumper
 */
void Hero::notify_collision_with_jumper(Jumper& jumper) {

  if (state->can_take_jumper()) {

    int jump_direction = jumper.get_direction();
    int jump_length = jumper.get_jump_length();
    if (jump_direction % 2 == 0) {

      if (get_ground() == GROUND_DEEP_WATER) {
        jump_direction = (jump_direction + 4) % 8; // make a reverse jump to get out of water
        jump_length = 24;
      }

      // this non-diagonal jumper is not currently an obstacle for the hero
      // (in order to allow his smooth collision movement),
      // so the hero may be one pixel inside the jumper before jumping
      if (jump_direction % 4 == 0) {
        set_aligned_to_grid_x();
      }
      else {
        set_aligned_to_grid_y();
      }
    }

    // jump
    start_jumping(jump_direction, jump_length, true, true, 0);
  }
}

/**
 * @brief This function is called when a sensor detects a collision with this entity.
 * @param sensor a sensor
 * @param collision_mode the collision mode that detected the collision
 */
void Hero::notify_collision_with_sensor(Sensor &sensor, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_INSIDE    // the hero is entirely inside the sensor
      && !state->can_avoid_sensor()) {
    sensor.activate(*this);
  }
}

/**
 * @brief This function is called when a switch detects a collision with this entity.
 * @param sw the switch
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) {

  // it's normally a walkable switch
  if (sw.is_walkable()
      && !state->can_avoid_switch()) {
    sw.try_activate(*this);
  }
}

/**
 * @brief This function is called when a the sprite of a switch
 * detects a pixel-precise collision with a sprite of this entity.
 * @param sw the switch
 * @param sprite_overlapping the sprite of the current entity that collides with the crystal
 */
void Hero::notify_collision_with_switch(Switch& sw, Sprite& sprite_overlapping) {

  // it's normally a solid switch
  if (sprite_overlapping.contains("sword") // the hero's sword is overlapping the switch
      && sw.is_solid()
      && state->can_sword_hit_crystal()) {
    // note that solid switches and crystals have the same rules for the sword

    sw.try_activate();
  }
}

/**
 * @brief This function is called when a crystal detects a collision with this entity.
 * @param crystal the crystal
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_crystal(Crystal &crystal, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_FACING_POINT) {
    // the hero is touching the crystal and is looking in its direction

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& is_free()) {

      // we show the action icon
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * @brief This function is called when a the sprite of a crystal 
 * detects a pixel-precise collision with a sprite of this entity.
 * @param crystal the crystal
 * @param sprite_overlapping the sprite of the current entity that collides with the crystal
 */
void Hero::notify_collision_with_crystal(Crystal &crystal, Sprite &sprite_overlapping) {

  if (sprite_overlapping.contains("sword") // the hero's sword is overlapping the crystal
      && state->can_sword_hit_crystal()) {
    
    crystal.activate(*this);
  }
}

/**
 * @brief This function is called when a chest detects a collision with this entity.
 * @param chest the chest
 */
void Hero::notify_collision_with_chest(Chest& chest) {

  if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
      && is_free()
      && is_facing_direction4(1)
      && !chest.is_open()) {

    // we show the 'open' icon, even if this is a big chest and the player does not have the big key
    get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_OPEN);
  }
}

/**
 * @brief This function is called when a block detects a collision with this entity.
 * @param block the block
 */
void Hero::notify_collision_with_block(Block& block) {

  if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
      && is_free()) {

    // we show the action icon
    get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_GRAB);
  }
}

/**
 * @brief This function is called when a bomb detects a collision with this entity.
 * @param bomb the bomb
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_bomb(Bomb& bomb, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_FACING_POINT) {
    // the hero is touching the bomb and is looking in its direction

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
        && get_facing_entity() == &bomb
	&& is_free()) {

      // we show the action icon
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LIFT);
    }
  }
}

/**
 * @brief This function is called when an explosion's sprite detects a collision with a sprite of the hero.
 * @param explosion the explosion
 * @param sprite_overlapping the sprite of the hero that collides with the explosion
 */
void Hero::notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping) {

  if (!state->can_avoid_explosion()) {
    if (sprite_overlapping.contains("tunic")) {
      hurt(explosion, 2, 0);
    }
  }
}

/**
 * @brief Makes the hero escape from an entity that is overlapping him.
 *
 * This function is called when an entity that just appeared may overlap the hero
 * (e.g. a chest or a door).
 *
 * @param entity the entity that just appeared
 * @param direction the direction of the hero relative to the entity
 * (the hero will be moved into this direction): 0 to 3
 */
void Hero::avoid_collision(MapEntity &entity, int direction) {

  // fix the hero's position, whatever the entity's) size is
  switch (direction) {

    case 0:
      set_top_left_x(entity.get_top_left_x() + entity.get_width());
      set_top_left_y(entity.get_center_point().get_y() - 8);
      break;

    case 1:
      set_top_left_y(entity.get_top_left_y() - this->get_height());
      set_top_left_x(entity.get_center_point().get_x() - 8);
      break;

    case 2:
      set_top_left_x(entity.get_top_left_x() - this->get_width());
      set_top_left_y(entity.get_center_point().get_y() - 8);
      break;

    case 3:
      set_top_left_y(entity.get_top_left_y() + entity.get_height());
      set_top_left_x(entity.get_center_point().get_x() - 8);
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
void Hero::notify_grabbed_entity_collision() {
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
bool Hero::is_striking_with_sword(Detector &detector) {
  return state->is_cutting_with_sword(detector);
}

/**
 * @brief Snaps the hero to the entity he is facing.
 *
 * The hero is snapped if there is no collision and if he is not too far.
 */
void Hero::try_snap_to_facing_entity() {

  Rectangle collision_box = get_bounding_box();
  Detector* facing_entity = get_facing_entity();

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

  if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, *this)) {
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
void Hero::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {

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
int Hero::get_sword_damage_factor() {

  return state->get_sword_damage_factor();
}

/**
 * @brief Returns whether the hero can be hurt currently.
 * @param attacker an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 * @return true if the hero can be hurt
 */
bool Hero::can_be_hurt(Enemy* attacker) {
  return state->can_be_hurt(attacker);
}

/**
 * @brief Hurts the hero if possible.
 * @param source an entity that hurts the hero (usually an enemy)
 * @param life_points number of heart quarters to remove (this number may be reduced by the tunic)
 * @param magic_points number of magic points to remove
 */
void Hero::hurt(MapEntity& source, int life_points, int magic_points) {

  Enemy* enemy = NULL;
  if (source.get_type() == ENEMY) {
    // TODO make state->can_be_hurt(MapEntity*)
    enemy = (Enemy*) &source;
  }

  if (!sprites->is_blinking() && state->can_be_hurt(enemy)) {
    set_state(new HurtState(*this, source.get_xy(), life_points, magic_points));
  }
}

/**
 * @brief Hurts the hero if possible.
 * @param source_xy coordinates of whatever hurts the hero
 * @param life_points number of heart quarters to remove (this number may be reduced by the tunic)
 * @param magic_points number of magic points to remove
 */
void Hero::hurt(const Rectangle& source_xy, int life_points, int magic_points) {

  if (!sprites->is_blinking() && state->can_be_hurt(NULL)) {
    set_state(new HurtState(*this, source_xy, life_points, magic_points));
  }
}

/**
 * @brief This function is called when the hero was dead but saved by a fairy.
 */
void Hero::get_back_from_death() {

  sprites->blink();
  start_state_from_ground();
  when_suspended = System::now();
}

/**
 * @brief Makes the hero drown or swim.
 */
void Hero::start_deep_water() {

  if (!state->is_touching_ground()) {
    // plunge into the water
    set_state(new PlungingState(*this));
  }
  else {
    // move to state swimming or jumping
    if (get_equipment().has_ability("swim")) {
      set_state(new SwimmingState(*this));
    }
    else {
      start_jumping(get_wanted_movement_direction8(), 32, false, true, 13);
    }
  }
}

/**
 * @brief Makes the hero move towards a hole of fall into it.
 */
void Hero::start_hole() {

  if (!state->can_control_movement()) {
    // the player has no control (e.g. he is running or being hurt):
    // fall immediately
    set_state(new FallingState(*this));
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
      set_state(new FallingState(*this));
    }
    else {
      set_walking_speed(normal_walking_speed / 3);
    }
    */

    if (last_solid_ground_coords.get_x() == -1 ||
	(last_solid_ground_coords.get_x() == get_x() && last_solid_ground_coords.get_y() == get_y())) {
      // fall immediately because the hero was not moving but directly placed on the hole
      set_state(new FallingState(*this));
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
 * @brief Makes the hero drown into lava.
 */
void Hero::start_lava() {

  // plunge into the lava
  set_state(new PlungingState(*this));
}

/**
 * @brief Makes the hero being hurt by prickles.
 * @param delay delay before returning control to the player
 */
void Hero::start_prickle(uint32_t delay) {

  Sound::play("hero_hurt");
  get_equipment().remove_life(2);
  start_back_to_solid_ground(false, delay, false);
}

/**
 * @brief Returns whether the hero can walk normally and interact with entities.
 * @return true if the hero can walk normally
 */
bool Hero::is_free() {

  return state->is_free();
}

/**
 * @brief Returns whether the hero is currently using an inventory item.
 * @return true if the hero is using an inventory item
 */
bool Hero::is_using_inventory_item() {

  return state->is_using_inventory_item();
}

/**
 * @brief When the hero is using an inventory item, returns the inventory item.
 * @return the current inventory item
 */
InventoryItem& Hero::get_current_inventory_item() {

  return state->get_current_inventory_item();
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in its current state.
 *
 * If he is not grabbing any entity, false is returned.
 *
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::is_moving_grabbed_entity() {
 
  return state->is_moving_grabbed_entity();
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity.
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::is_grabbing_or_pulling() {

  return state->is_grabbing_or_pulling();
}

/**
 * @brief Lets the hero walk normally.
 */
void Hero::start_free() {

  if (!state->is_free()) {
    set_state(new FreeState(*this));
  }
}

/**
 * @brief Makes the hero brandish a treasure.
 * @param treasure the treasure to give him (you have to delete it after the hero brandishes it)
 */
void Hero::start_treasure(const Treasure &treasure) {
  set_state(new TreasureState(*this, treasure));
}

/**
 * @brief Makes the hero walk with a predetermined path.
 *
 * The player does not control him anymore.
 *
 * @param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7'), as in PathMovement
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param ignore_obstacles true to make the movement ignore obstacles
 */
void Hero::start_forced_walking(const std::string &path, bool loop, bool ignore_obstacles) {
  set_state(new ForcedWalkingState(*this, path, loop, ignore_obstacles));
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
 */
void Hero::start_jumping(int direction8, int length, bool ignore_obstacles,
    bool with_sound, uint32_t movement_delay) {

  JumpingState *state = new JumpingState(*this, direction8, length, ignore_obstacles, with_sound, movement_delay);
  set_state(state);
}

/**
 * @brief Makes the hero brandish his sword meaning a victory.
 */
void Hero::start_victory() {
  set_state(new VictoryState(*this));
}

/**
 * @brief Freezes the hero.
 *
 * When the hero is freezed, he cannot move.
 * The current animation of the hero's sprites is stopped and the "stopped" animation is played.
 * You can call start_free() to unfreeze him.
 */
void Hero::start_freezed() {
  set_state(new FreezedState(*this));
}

/**
 * @brief Makes the hero lift a destructible item.
 * @param item_to_lift the item to lift (will be destroyed automatically)
 */
void Hero::start_lifting(CarriedItem *item_to_lift) {
  set_state(new LiftingState(*this, item_to_lift));
}

/**
 * @brief Starts running with the speed shoes.
 */
void Hero::start_running() {
  set_state(new RunningState(*this));
}

/**
 * @brief Starts grabbing an obstacle.
 */
void Hero::start_grabbing() {
  set_state(new GrabbingState(*this));
}

/**
 * @brief Starts shooting a boomerang.
 * @param max_distance maximum distance of the movement in pixels
 * @param speed speed of the movement in pixels per second
 * @param tunic_preparing_animation animation name of the hero's tunic sprite
 * when preparing the boomerang
 * @param sprite_name animation set id that represents the boomerang
 */
void Hero::start_boomerang(int max_distance, int speed,
    const std::string& tunic_preparing_animation,
    const SpriteAnimationSetId& sprite_name) {

  set_state(new BoomerangState(*this, max_distance, speed,
      tunic_preparing_animation, sprite_name));
}

/**
 * @brief Starts shooting an arrow with a bow.
 */
void Hero::start_bow() {
  set_state(new BowState(*this));
}

/**
 * @brief Starts shooting the hookshot.
 */
void Hero::start_hookshot() {
  set_state(new HookshotState(*this));
}

/**
 * @brief Makes the hero return to his last solid ground position.
 * @param use_memorized_xy true to get back to the place previously memorized (if any),
 * false to get back to the last coordinates with solid ground
 * @param end_delay a delay to add at the end before returning control to the hero (default 0)
 * @param with_sound true to play a sound when returning to solid ground (default true)
 */
void Hero::start_back_to_solid_ground(bool use_memorized_xy,
    uint32_t end_delay, bool with_sound) {

  set_state(new BackToSolidGroundState(*this, use_memorized_xy, end_delay, with_sound));
}

/**
 * @brief Activates immediately the state corresponding to the current ground.
 *
 * Only the state is changed here.
 * Some other functions like start_deep_water() and start_hole()
 * are triggered when the ground changes (for example,
 * going from normal ground to deep water ground) and make more
 * complex transitions.
 * This function is supposed to called when the ground was ignored
 * and you want to apply its effect now (no matter whether it has changed or not).
 * This function is typically called at the end of a state that ignores
 * the ground (like JumpingState) to choose the
 * correct next state depending on the ground the hero lands on.
 */
void Hero::start_state_from_ground() {

  switch (ground) {

  case GROUND_DEEP_WATER:
    set_state(new PlungingState(*this));
    break;

  case GROUND_HOLE:
    set_state(new FallingState(*this));
    break;

  case GROUND_LAVA:
    set_state(new PlungingState(*this));
    break;

  case GROUND_PRICKLE:
    // there is no specific state for prickles (yet?)
    set_state(new FreeState(*this));
    start_prickle(0);
    break;

  case GROUND_NORMAL:
  case GROUND_SHALLOW_WATER:
  case GROUND_GRASS:
  case GROUND_LADDER:
  case GROUND_EMPTY:
    if (state->is_carrying_item()) {
      set_state(new CarryingState(*this, state->get_carried_item()));
    }
    else {
      set_state(new FreeState(*this));
    }
    break;
  }
}

