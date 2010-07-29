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

/**
 * @brief Creates a hero.
 * @param equipment the equipment
 */
Hero::Hero(Equipment *equipment):
  state(NULL), state_id(FREE), equipment(equipment), facing_entity(NULL),
  ground(GROUND_NORMAL), next_ground_date(0), {

  // position
  set_size(16, 16);
  set_origin(8, 13);

  // create the states
  states[FREE] = new StateFree(this);
  // TODO create all states
  
  set_state(FREE);

  // sprites
  sprites = new HeroSprites(this, equipment);
  rebuild_equipment();
}

/**
 * @brief Destructor.
 */
Hero::~Hero(void) {

  for (int i = 0; i < NB_STATES; i++) {
    delete states[i];
  }

  delete sprites;
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
 * @brief Returns the id of the hero's current state.
 * @return the state of the hero
 */
Hero::StateId Hero::get_state(void) {
  return state_id;
}

/**
 * @brief Returns the specified state.
 * @param state_id id of a state
 * @return the corresponding state
 */
Hero::State Hero::get_state(StateId state_id) {
  return states[state_id];
}

/**
 * @brief Sets the hero's state.
 *
 * The previous state should have been stopped.
 *
 * @param state_id id of the new state of the hero
 */
void Hero::set_state(StateId state_id) {

  if (state_id != this->state_id || state == NULL) {

    if (state != NULL) {
      state->stop();
    }

    this->state_id = state_id;
    
    state = get_state(state_id);
    state->start();
  }
}

/**
 * @brief Returns whether the hero is currently in the specified state.
 * @param state a state
 * @return true if the hero is in this state
 */
bool Hero::is_state(StateId state_id) {
  return get_state() == state_id;
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

  // sprites
  sprites->set_suspended(suspended);

  // state
  state->set_suspended(suspended);
}

/**
 * @brief Updates the hero's position, movement and animation.
 *
 * This function is called repeteadly by the game loop.
 */
void Hero::update(void) {

  if (!suspended) {

    state->update();
    sprites->update();
    update_movement();
    update_ground();
    map->check_collision_with_detectors(this);
    check_gameover();
  }
}

/**
 * @brief Updates the hero's position according to its movement.
 *
 * This function is called repeatedly by update().
 */
void Hero::update_movement(void) {

  bool trying_to_move = get_movement()->has_to_move_now();

  // save the current coordinates
  Rectangle old_xy(get_xy());

  // make the movement (this might call notify_position_changed())
  get_movement()->update();

  // see if the movement was successful (i.e. if the hero's coordinates have changed)
  bool success = (get_x() != old_xy.get_x() || get_y() != old_xy.get_y());

  // always notify the state
  state->notify_movement_result(trying_to_move, success);
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

    if (is_ground_visible()) {
      // a special ground is displayed under the hero and it's time to play a sound
      if (sprites->is_walking() && state->is_touching_ground()) {
        sprites->play_ground_sound();
      }

      next_ground_date = now + 300;
    }

    else if (ground == GROUND_HOLE && state->can_fall_into_hole()) {
      // the hero is being attracted by a hole and it's time to move one more pixel into the hole

      if (get_distance(last_solid_ground_coords.get_x(), last_solid_ground_coords.get_y()) >= 8) {
	// too far from the solid ground: make the hero fall
        set_state(FALLING);
      }
      else {

        // not too far yet
        Rectangle collision_box = get_bounding_box();
        collision_box.add_xy(hole_dx, hole_dy);

        if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
          set_bounding_box(collision_box);
          notify_position_changed();
        }
        else {
          collision_box = get_bounding_box();
          collision_box.add_xy(hole_dx, 0);
          if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
            set_bounding_box(collision_box);
            notify_position_changed();
          }
          else {
           collision_box = get_bounding_box();
            collision_box.add_xy(0, hole_dy);
            if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
              set_bounding_box(collision_box);
              notify_position_changed();
            }
          }
        }

        next_ground_date = now + 60;
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

  if (equipment->get_hearts() <= 0 && state->can_start_gameover_sequence()) {
    sprites->stop_blinking();
    game->start_gameover_sequence();
  }
}

/**
 * @brief Displays this entity on the map.
 *
 * The hero is displayed with its current animation and at its current position.
 */
void Hero::display_on_map(void) {

  if (!sprites->is_visible() || game->is_showing_gameover()) {
    return; // the hero is not displayed
  }

  state->display_on_map();
}

/**
 * @brief This function is called when a game key is pressed
 * and the game is not suspended.
 * @param key the key pressed
 */
void Hero::key_pressed(GameControls::GameKey key) {

  switch (key) {

    // action key
  case GameControls::ACTION:
    action_key_pressed();
    break;

    // sword key
  case GameControls::SWORD:
    sword_key_pressed();
    break;

    // move the hero
  case GameControls::RIGHT:
    directional_key_pressed(0);
    break;

  case GameControls::UP:
    directional_key_pressed(1);
    break;

  case GameControls::LEFT:
    directional_key_pressed(2);
    break;

  case GameControls::DOWN:
    directional_key_pressed(3);
    break;

  case GameControls::ITEM_1:
    item_key_pressed(0);
    break;

  case GameControls::ITEM_2:
    item_key_pressed(1);
    break;

  default:
    break;
  }
}

/**
 * @brief This function is called when a key is released
 * if the game is not suspended.
 * @param key the key released
 */
void Hero::key_released(GameControls::GameKey key) {

  /*
   * When the action key or the sword key are released,
   * we do nothing here: the update() functions will detect that
   * the key is not pressed anymore. Thus, the key will be
   * detected even if it is released while the game is suspended.
   */

  switch (key) {

  case GameControls::RIGHT:
    state->directional_key_released(0);
    break;

  case GameControls::UP:
    state->directional_key_released(1);
    break;

  case GameControls::LEFT:
    state->directional_key_released(2);
    break;

  case GameControls::DOWN:
    state->directional_key_released(3);
    break;

  case GameControls::ITEM_1:
    state->item_key_released(0);
    break;

  case GameControls::ITEM_2:
    state->item_key_released(1);
    break;

  default:
    break;
  }
}

/**
 * @brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * @return the direction of the sprites (0 to 3)
 */
int Hero::get_animation_direction4(void) {
  return sprites->get_animation_direction();
}

/**
 * @brief Changes the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * @param direction the direction to set (0 to 3)
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
  state->notify_equipment_changed();
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

  set_map(map);

  // take the specified direction
  if (initial_direction != -1) {
    sprites->set_animation_direction(initial_direction);
  }

  // notify the state
  state->set_map(map);
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

    set_state(FREE);
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
	  DIE("Invalid destination side: " << side);
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
	StateStairs *state_stairs = (StateStairs*) get_state(STAIRS);
	state_stairs->set_stairs(stairs, Stairs::REVERSE_WAY);
	set_state(STAIRS);
      }
      else {
	// the hero arrived on the map by a usual destination point
        set_state(FREE);
      }
    }
  }
}

/**
 * @brief This function is called when the opening transition of the map is finished.
 *
 * The position of the hero is changed if necessary.
 */
void Hero::opening_transition_finished(void) {

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
	DIE("Invalid destination side: " << side);
    }
  }
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
      DIE("Invalid direction for Hero::get_facing_point(): " << direction);
  }

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
      DIE("Invalid animation direction '" << sprites->get_animation_direction() << "'");
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
 * @brief Returns whether the hero is currently on raised crystal switch blocks.
 * @return true if the hero is currently on raised crystal switch blocks
 */
bool Hero::is_on_raised_blocks(void) {
  return map->get_entities()->is_hero_on_raised_blocks();
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
  state->notify_walking_speed_changed(walking_speed);
}

/**
 * @brief Returns the direction of the hero's movement as defined by the directional keys pressed by the player.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * @return the hero's wanted direction between 0 and 359, or -1 if he is stopped
 */
int Hero::get_wanted_movement_direction(void) {
  return state->get_wanted_movement_direction();
}

/**
 * @brief Returns the direction of the hero's movement as defined by the directional keys pressed by the player.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * @return the hero's wanted direction between 0 and 7, or -1 if he is stopped
 */
int Hero::get_wanted_movement_direction8(void) {

  int wanted_direction = get_wanted_movement_direction();
  return (wanted_direction != -1) ? (wanted_direction / 45) : -1;
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
 * @return the hero's actual direction between 0 and 359, or -1 if he is stopped
 */
int Hero::get_real_movement_direction(void) {

  int real_movement_direction8 = get_real_movement_direction8();
  return (real_movement_direction8 != -1) ? (real_movement_direction8 * 45) : -1;
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
 * @param direction one of the four main directions (0 to 3)
 * @return true if the hero is moving in that direction, even if he is actually doing a diagonal move
 */
bool Hero::is_moving_towards(int direction4) {

  if (get_movement()->is_stopped()) {
    return false;
  }

  int direction8 = direction4 * 2;
  int movement_direction8 = get_wanted_movement_direction8();
  return movement_direction8 == direction8
    || (movement_direction8 + 1) % 8 == direction8
    || movement_direction8 - 1 == direction8;
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
  Ground tiles_ground = map->get_tile_ground(get_layer(), get_x(), get_y() - 2);
  set_ground(tiles_ground);

  // see the ground indicated by the dynamic entities, also find the facing entity
  MapEntity::notify_position_changed();

  if (this->ground != previous_ground) {
    notify_ground_changed();
  }

  if (is_on_solid_ground()
      && (get_x() != last_solid_ground_coords.get_x() || get_y() != last_solid_ground_coords.get_y())) {

    // save the hero's last valid position
    last_solid_ground_coords = get_xy();
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
  int direction = get_wanted_movement_direction();
  if (direction != -1) {

    int old_animation_direction = sprites->get_animation_direction();
    int animation_direction = sprites->get_animation_direction(get_wanted_movement_direction8(), get_real_movement_direction8());

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
  if (map != NULL && !game->is_suspended()) {
    notify_position_changed();
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
 * @brief Sets the ground under the hero.
 *
 * This function may be called by dynamic entities
 * having a special ground when the hero walks on them.
 *
 * @param ground the ground to set
 */
void Hero::set_ground(Ground ground) {

  if (ground != this->ground) {
    this->ground = ground;
  }
}

/**
 * @brief Starts activating the ground specified by the last call to set_ground().
 */
void Hero::start_ground(void) {

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
  state->notify_ground_changed(ground);
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
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Hero::is_obstacle_for(MapEntity *other) {
  return other->is_hero_obstacle(this);
}

/**
 * @brief Returns whether a water tile is currently considered as an obstacle for the hero.
 * @return true if the water tiles are currently an obstacle for the hero
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
  return state->is_stairs_obstacle(stairs);
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
  return state->is_raised_block_obstacle(raised_block);
}

/**
 * @brief Returns whether a jump sensor is currently considered as an obstacle for this entity.
 * @param jump_sensor a jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool Hero::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return state->is_jump_sensor_obstacle(jump_sensor);
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
void Hero::notify_collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode) {

  if (!state->can_avoid_teletransporter()) {

    bool on_hole = map->get_tile_ground(get_layer(), get_x(), get_y()) == GROUND_HOLE;
    if (on_hole || state->is_teletransporter_delayed(this)) {
      this->delayed_teletransporter = teletransporter; // fall into the hole (or do something else) first, transport later
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
	// set the state
	set_state(CONVEYOR_BELT);
      }
    }
  }
}

/**
 * @brief This function is called when a stairs entity detects a collision with this entity.
 * @param stairs the stairs
 * @param collision_mode the collision mode that detected the event
 */
void Hero::notify_collision_with_stairs(Stairs *stairs, int collision_mode) {

  if (state->can_take_stairs()) {

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
      StateStairs *state = (StateStairs*) get_state(STAIRS);
      state->set_stairs(stairs, stairs_way);
      set_stairs(STAIRS);
    }
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
      DIE("Invalid direction in Hero::avoid_collision(): " << direction);
      break;
  }
  reset_movement();
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
void Hero::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed) {

  state->just_attacked_enemy(attack, victim, result, killed);
}

/**
 * @brief Tests whether the hero is striking the specified detector with his sword.
 *
 * When the sword sprite collides with a detector,
 * this function can be called to determine whether the hero is
 * really striking this particular detector only.
 * This depends on the hero's state, his direction and his
 * distance to the detector.
 * This function assumes that there is already a collision
 * between the sword sprite and the detector's sprite.
 * This function should be called to check whether the
 * hero wants to cut a bush or some grass.
 * Don't use this function for enemies since any sprite
 * collision is enough to hurt an enemy.
 *
 * @param detector the detector to check
 * @return true if the sword is striking this detector
 */
bool Hero::is_striking_with_sword(Detector *detector) {
  return state->is_striking_with_sword(detector);
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
 * @brief Hurts the hero if possible.
 * @param source the entity that hurts the hero (usually an enemy)
 * @param life_points number of heart quarters to remove (this number may be reduced by the tunic)
 * @param magic_points number of magic points to remove
 */
void Hero::hurt(MapEntity *source, int life_points, int magic_points) {

  if (state->can_be_hurt()) {
    set_state(HURT);
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
 * @brief Notifies the hero that the entity he is pushing or pulling
 * cannot move any more because of a collision.
 */
void Hero::notify_grabbed_entity_collision(void) {
  state->notify_grabbed_entity_collision();
}

/**
 * @brief Makes the hero brandish a treasure.
 * @param treasure the treasure to give him (you have to delete it after the hero brandishes it)
 */
void Hero::give_treasure(Treasure *treasure) {

  TreasureState *state = (TreasureState*) get_state(TREASURE);
  state->set_treasure(treasure);
  set_state(TREASURE);
}

/**
 * @brief Makes the hero jump into a direction.
 *
 * While he is jumping, the player does not control him anymore.
 *
 * @param direction8 direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param with_collisions true to stop the movement if there is a collision
 * @param with_sound true to play the "jump" sound
 * @param layer_after_jump the layer to set when the jump is finished
 * (or LAYER_NB to keep the same layer)
 * @param movement_delay delay between each one-pixel move in the jump movement (0: default)
 */
void Hero::jump(int direction8, int length, bool with_collisions, bool with_sound, Layer layer_after_jump, uint32_t movement_delay) {

  StateJumping *state = (StateJumping*) get_state(JUMPING);
  state->set_jump(direction8, length, with_collision, with_sound, layer_after_jump, movement_delay);
  set_state(JUMPING);
}

/**
 * @brief Makes the hero drown or swim.
 */
void Hero::start_deep_water(void) {

  // stop the sword
  sprites->stop_displaying_sword();

  if (!is_touching_ground())
    // plunge into the water
    set_state(PLUNGING);
  }
  else {
    // move to state swimming or jumping
    if (equipment->has_inventory_item(INVENTORY_FLIPPERS)) {
      set_state(SWIMMING);
    }
    else {
      jump(get_wanted_movement_direction8(), 32, true, true, LAYER_NB, 13);
    }
  }
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

  walking_speed = normal_walking_speed / 3;
  state->notify_walking_speed_changed();
}

/**
 * @brief Makes the hero brandish his sword meaning a victory.
 */
void Hero::start_victory() {
  set_state(VICTORY);
}

