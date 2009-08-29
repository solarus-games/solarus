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
#include "entities/HeroSprites.h"
#include "entities/MapEntities.h"
#include "entities/DestinationPoint.h"
#include "entities/CarriedItem.h"
#include "entities/Detector.h"
#include "entities/ConveyorBelt.h"
#include "entities/Chest.h"
#include "movements/PlayerMovement.h"
#include "movements/JumpMovement.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "ZSDX.h"
#include "ResourceManager.h"
#include "Game.h"
#include "Equipment.h"
#include "Map.h"
#include "InventoryItem.h"

const int Hero::walking_speed = 9;

/**
 * Constructor.
 */
Hero::Hero(Equipment *equipment):
  equipment(equipment), normal_movement(new PlayerMovement(walking_speed)),
  state(FREE), facing_entity(NULL),
  counter(0), next_counter_date(0), pushing_direction_mask(0xFFFF),
  grabbed_entity(NULL), on_conveyor_belt(false), 
  lifted_item(NULL), thrown_item(NULL), treasure(NULL),
  ground(GROUND_NORMAL), next_ground_date(0),
  current_inventory_item(NULL), when_can_use_inventory_item(0) {

  set_size(16, 16);
  set_origin(8, 13);

  set_movement(normal_movement);
  sprites = new HeroSprites(this, equipment);
  rebuild_equipment();

  last_solid_ground_coords.x = 0;
  last_solid_ground_coords.y = 0;
  last_solid_ground_layer = LAYER_LOW;
}

/**
 * Destructor.
 */
Hero::~Hero(void) {
  delete sprites;
  destroy_carried_items();
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Hero::get_type() {
  return HERO;
}

/**
 * Returns whether entities of this type can be obstacles for other entities.
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 * @return true if this type of entity can be obstacle for other entities
 */
bool Hero::can_be_obstacle(void) {
  return true; 
}

/**
 * Returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * collision() will be called when a collision is detected.
 * @return true if this type of entity can detect other entities
 */
bool Hero::can_detect_entities(void) {
  return false;
}

/**
 * Returns whether entities of this type can be displayed.
 * If enabled, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 * @return true if this type of entity can be displayed
 */
bool Hero::can_be_displayed(void) {
  return true; 
}

/**
 * Returns whether an entity of this type should be displayed above
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
 * Returns whether the hero is currently on raised crystal switch blocks.
 * @return true if the hero is currently on raised crystal switch blocks
 */
bool Hero::is_on_raised_blocks(void) {
  return map->get_entities()->is_hero_on_raised_blocks();
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Hero::is_obstacle_for(MapEntity *other) {

  EntityType type = other->get_type();

  if (type == INTERACTIVE_ENTITY) {
    return true;
  }

  if (type == BLOCK && other->get_movement() == NULL) {
    return true; // let the hero overlap (for 1 pixel actually) the block he is pushing
  }

  return false;
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
 * Returns whether a ladder is currently considered as an obstacle for the hero.
 * @return true if the ladders are currently an obstacle for the hero
 */
bool Hero::is_ladder_obstacle(void) {
  return false;
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle.
 * This depends on the hero's state.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for the hero
 */
bool Hero::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return state > SWIMMING && state != JUMPING;
}

/**
 * Returns whether a conveyor belt is currently considered as an obstacle for the hero.
 * This depends on the hero's state.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool Hero::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return state >= SWIMMING && state != JUMPING && state != CONVEYOR_BELT && state != PUSHING;
}

/**
 * Returns whether a sensor is currently considered as an obstacle for the hero.
 * @param sensor a sensor
 * @return true if this sensor is currently an obstacle for the hero
 */
bool Hero::is_sensor_obstacle(Sensor *sensor) {

  if (state == HURT) {
    return false;
  }

  if (state == JUMPING) {
    JumpMovement *movement = (JumpMovement*) get_movement();
    return movement->get_length() > 8; // this allows small jumps (e.g. falling from a raised block) but not jumping with the feather
  }

  return false;
}

/**
 * Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * @param raised_block a crystal switch block raised
 * @return true if the raised block is currently an obstacle for this entity
 */
bool Hero::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {

  // the raised block is an obstacle when the hero is not on it
  return !is_on_raised_blocks();
}

/**
 * Returns whether a jump sensor is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @param jump_sensor a jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool Hero::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return false;
}

/**
 * Returns the 8-direction movement controlled by the player,
 * even if it is not the current movement of the hero.
 * @return the player's movement
 */
PlayerMovement * Hero::get_normal_movement(void) {
  return normal_movement;
}

/**
 * Returns the direction of the hero's movement.
 * If he is not moving, -1 is returned.
 * @return the hero's movement direction between 0 and 360, or -1 if he is stopped
 */
int Hero::get_movement_direction(void) {
  return get_normal_movement()->get_direction();
}

/**
 * Returns whether the hero is moving towards the specified direction.
 * If the hero is not moving, false is returned.
 * @param direction one of the four main directions (0 to 3)
 * @return true if the hero is moving in that direction,
 * even if he is actually doing a diagonal move
 */
bool Hero::is_moving_towards(int direction) {

  if (get_movement()->is_stopped()) {
    return false;
  }

  int direction8 = direction * 2;
  int movement_direction8 = get_movement_direction() / 45;
  return movement_direction8 == direction8
    || (movement_direction8 + 1) % 8 == direction8
    || movement_direction8 - 1 == direction8;
}

/**
 * Snaps the hero to the entity he is facing if there is no
 * collision and if he is not too far.
 */
void Hero::try_snap_to_facing_entity(void) {

  SDL_Rect collision_box = get_rectangle();

  if (get_animation_direction() % 2 == 0) {
    if (abs(collision_box.y - facing_entity->get_top_left_y()) <= 5) {
      collision_box.y = facing_entity->get_top_left_y();
    }
  }
  else {
    if (abs(collision_box.x - facing_entity->get_top_left_x()) <= 5) {
      collision_box.x = facing_entity->get_top_left_x();
    }
  }

  if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
    set_rectangle(collision_box);
    just_moved();
  }
}

/**
 * Returns the coordinates of a point in the direction the hero's sprite is looking at.
 * This point is 1 pixel outside the hero's collision box. It is used
 * to determine the actions he can do depending on the entity he is facing
 * (a bush, a pot, a PNJ...)
 * @return the point the hero is facing
 */
const SDL_Rect Hero::get_facing_point(void) {

  int direction = sprites->get_animation_direction();
  return get_facing_point(direction);
}

/**
 * Returns the point located just outside the hero's collision box,
 * in the specified direction.
 * @param direction a direction (0 to 3)
 */
const SDL_Rect Hero::get_facing_point(int direction) {

  SDL_Rect facing_point;

  switch (direction) {

    // right
  case 0:
    facing_point.x = rectangle.x + 16;
    facing_point.y = rectangle.y + 8;
    break;

    // up
  case 1:
    facing_point.x = rectangle.x + 8;
    facing_point.y = rectangle.y - 1;
    break;

    // left
  case 2:
    facing_point.x = rectangle.x - 1;
    facing_point.y = rectangle.y + 8;
    break;

    // down
  case 3:
    facing_point.x = rectangle.x + 8;
    facing_point.y = rectangle.y + 16;
    break;

  default:
    DIE("Invalid direction for get_facing_point(): " << direction);
  }

  return facing_point;
}

/**
 * Sets the hero's current map.
 * This function is called when the map is changed.
 * @param map the map
 */
void Hero::set_map(Map *map) {

  MapEntity::set_map(map);

  target_solid_ground_coords.x = -1;
  target_solid_ground_coords.y = -1;

  if (lifted_item != NULL) {
    lifted_item->set_map(map);
  }

  if (current_inventory_item != NULL) {
    current_inventory_item->set_map(map);
  } 
}

/**
 * Sets the hero's current map.
 * This function is called when the map is changed.
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
}

/**
 * Suspends or resumes the animation and the movements of the hero.
 * This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the hero, false to resume it
 */
void Hero::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended);

  // movement
  get_normal_movement()->set_suspended(suspended);

  // sprites
  sprites->set_suspended(suspended);

  // carried items
  set_suspended_carried_items(suspended);

  // timers
  if (!suspended) {
    Uint32 now = SDL_GetTicks();
    next_counter_date += now - when_suspended;
  }
}

/**
 * Updates the hero's position, movement and animation.
 * This function is called repeteadly by the game.
 */
void Hero::update(void) {

  if (!suspended) {

    //std::cout << SDL_GetTicks() << " state " << state << std::endl;

    // update the movement
    get_normal_movement()->set_moving_enabled(state < PUSHING, state <= CONVEYOR_BELT);

    // specific updates in some states
    switch (state) {

    case SWORD_SWINGING:
      update_sword_swinging();
      break;

    case SWORD_LOADING:
      update_sword_loading();
      break;

    case SWORD_TAPPING:
      update_sword_tapping();
      break;

    case SPIN_ATTACK:
      update_spin_attack();
      break;

    case GRABBING:
    case PULLING:
      update_grabbing_pulling();
      break;

    case CONVEYOR_BELT:
      update_conveyor_belt();
      break;

    case JUMPING:
      update_jumping();
      break;

    case HURT:
      update_hurt();
      break;

    case PLUNGING:
      update_plunging();
      break;

    case RETURNING_TO_SOLID_GROUND:
      update_returning_to_solid_ground();
      break;

    case FALLING:
      update_falling();
      break;

    case USING_INVENTORY_ITEM:
      update_inventory_item();
      break;

    case FREEZED:
      update_freezed();
      break;

    default:
      break;
    }

    update_position();
    update_pushing();
    update_moving_grabbed_entity();
    sprites->update();

    if (treasure != NULL) {
      update_treasure();
    }

    update_ground();

    map->check_collision_with_detectors(this);

    if (equipment->get_hearts() <= 0 && can_start_gameover_sequence()) {
      sprites->stop_blinking();
      zsdx->game->start_gameover_sequence();
    }
  }

  // the carried items have to be updated even when the game is suspended
  // because the hero moves when scrolling between two maps
  update_carried_items();
}

/**
 * Displays the hero on the map with its current animation and
 * at its current position.
 */
void Hero::display_on_map(void) {

  if (!sprites->is_visible()) {
    return; // the hero is directly displayed by the game over sequence
  }

  sprites->display_on_map();

  display_carried_items();

  if (treasure != NULL) {
    display_treasure();
  }
}

/**
 * Returns the direction of the hero's sprites.
 * It is different from the movement direction.
 * @return the direction of the sprites (0 to 3)
 */
int Hero::get_animation_direction(void) {
  return sprites->get_animation_direction();
}

/**
 * Changes the direction of the hero's sprites.
 * It is different from the movement direction.
 * @param direction the direction to set (0 to 3)
 */
void Hero::set_animation_direction(int direction) {
  sprites->set_animation_direction(direction);
}

/**
 * Returns whether the sprites animations are finished.
 * @return true if the animation is finished
 */
bool Hero::is_animation_finished(void) {
  return sprites->is_animation_finished();
}

/**
 * Loads (or reloads) the sprites and sounds of the hero and his equipment,
 * depending on its tunic, sword and shield as specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as the hero's equipment is changed.
 */
void Hero::rebuild_equipment(void) {

  sprites->rebuild_equipment();

  // animation walking or stopped
  set_state(FREE);
  if (get_normal_movement()->is_started()) {
    sprites->set_animation_walking();
  }
  else {
    sprites->set_animation_stopped();
  }
}

/**
 * Updates the hero depending on the arrows pressed.
 * This function is called when the hero's direction changes (typically, because the player
 * pressed or released an arrow). It updates the hero's animations and 
 * collisions according to the new movement.
 */
void Hero::movement_just_changed(void) {

  // update the animation direction according to the movement direction
  int direction = get_movement_direction();
  if (direction != -1) {

    Uint16 direction_mask = get_normal_movement()->get_direction_mask();
    int old_animation_direction = sprites->get_animation_direction();
    int animation_direction = HeroSprites::get_animation_direction(direction_mask);

    if (animation_direction != old_animation_direction
	&& animation_direction != -1
	&& !is_direction_locked()) {
      // if the direction defined by the arrows has changed,
      // update the sprite's direction of animation
      // (unless the hero is loading his sword)
      sprites->set_animation_direction(animation_direction);
    }
  }

  // show the animation corresponding to the movement tried by the player
  if (get_normal_movement()->is_moving_enabled()) {

    // the player can move
    bool started = get_normal_movement()->is_started();

    // stopped to walking
    if (started && !sprites->is_walking()) {
      sprites->set_animation_walking();
    }

    // walking to stopped
    else if (!started && sprites->is_walking()) {
      sprites->set_animation_stopped();
    }
  }

  // check the collisions
  if (map != NULL && !zsdx->game->is_suspended()) {
    just_moved();
  }
}

/**
 * This function is called when the hero's position is changed,
 * or when his direction changes.
 */
void Hero::just_moved(void) {

  if (state == RETURNING_TO_SOLID_GROUND) {
    // do not take care of the ground or detectors when returning to a solid ground point
    return;
  }

  // save the current ground
  Ground previous_ground = this->ground;

  // see the ground indicated by the tiles
  Ground tiles_ground = map->get_tile_ground(get_layer(), get_x(), get_y());
  set_ground(tiles_ground);

  set_facing_entity(NULL);
  MapEntity::just_moved(); // to see the special ground indicated by the dynamic entities

  if (this->ground != previous_ground) {
    start_ground();
  }

  if (ground < GROUND_DEEP_WATER
      && state != JUMPING
      && state != RETURNING_TO_SOLID_GROUND) {
    // save the hero's last valid position
 
    if (get_x() != last_solid_ground_coords.x || get_y() != last_solid_ground_coords.y) {
      last_solid_ground_coords = get_xy();
      last_solid_ground_layer = get_layer();
    }
  }
}

/**
 * Returns whether the hero is currently on a hole.
 * @return true if the hero is currently on a hole.
 */
bool Hero::is_on_hole(void) {
  return ground == GROUND_HOLE;
}

/**
 * Sets the entity the hero is currently facing.
 * This function is called when the hero is just being
 * facing another entity.
 * @param detector the detector the hero is facing
 */
void Hero::set_facing_entity(Detector *detector) {

  this->facing_entity = detector;

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();

  // if the hero stops facing an entity that showed an action icon
  if (facing_entity == NULL &&
      keys_effect->is_action_key_acting_on_facing_entity()) {

    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  }
}

/**
 * Returns the entity the hero is currently facing.
 * This function is called when the hero is just being
 * facing another entity.
 * @return the detector the hero is facing
 */
Detector * Hero::get_facing_entity(void) {
  return facing_entity;
}

/**
 * Returns whether the hero is facing an obstacle, i.e. whether
 * its facing point is overlapping an obstacle of the map.
 * This information is calculated and not stored, so it is
 * always up to date.
 * @return true if the hero is facing an obstacle.
 */
bool Hero::is_facing_obstacle(void) {

  SDL_Rect collision_box = get_rectangle();
  switch (sprites->get_animation_direction()) {

  case 0:
    collision_box.x++;
    break;

  case 1:
    collision_box.y--;
    break;

  case 2:
    collision_box.x--;
    break;

  case 3:
    collision_box.y++;
    break;

  default:
    DIE("Invalid animation direction '" << sprites->get_animation_direction() << "'");
    break;
  }

  return map->test_collision_with_obstacles(get_layer(), collision_box, this);

  /* old version with only one point: problems when the hero
     cannot pass but the facing point can
  SDL_Rect facing_point = get_facing_point();
  return map->test_collision_with_obstacles(layer, facing_point.x, facing_point.y, this);
  */
}

/**
 * Updates the hero's position.
 * This function is called repeatedly by update().
 */
void Hero::update_position(void) {

  // no position change when the game is suspended
  if (zsdx->game->is_suspended()) {
    return;
  }

  move_tried = get_movement()->has_to_move_now();
  old_x = 0;
  old_y = 0;
  if (move_tried) {
    // save the current coordinates
    old_x = get_x();
    old_y = get_y();

    // try to move the hero
    get_movement()->update();
  }
}

/**
 * Places the hero on the map specified and at its destination point selected.
 * @param map the new map
 */
void Hero::place_on_destination_point(Map *map) {

  std::string destination_point_name = map->get_destination_point_name();
  
  if (destination_point_name == "_same") {

    // the hero's coordinates are the same as on the previous map
    set_map(map);

    destroy_carried_items();
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
	  DIE("Invalid destination side: " << side);
      }
    }
    else {

      // normal case: the location is specified by a destination point object

      MapEntities *entities = map->get_entities();

      DestinationPoint *destination_point = (DestinationPoint*)
	entities->get_entity(DESTINATION_POINT, destination_point_name);

      set_map(map, destination_point->get_direction());
      set_x(destination_point->get_x());
      set_y(destination_point->get_y());
      entities->set_entity_layer(this, destination_point->get_layer());

      destroy_carried_items();
      entities->remove_boomerang();
      start_free();
    }
  }
}

/**
 * This function is called when the opening transition of the map is finished.
 * The position of the hero is updated if necessary.
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
 * @param detector the detector to check
 * @return true if the sword is striking this detector
 */
bool Hero::is_stroke_by_sword(Detector *detector) {

  bool result = false;
  int animation_direction = sprites->get_animation_direction();

  switch (state) {

  case SPIN_ATTACK:
    // during a spin attack, any sprite collision is considered as a strike
    result = true;
    break;

  case SWORD_TAPPING:
    // when the hero is tapping his sword against a wall, this wall should be detector
    result = detector->is_obstacle_for(this)
      && facing_entity == detector
      && sprites->get_current_frame() >= 3;
    break;

  case SWORD_SWINGING:
    // for a normal sword swing, check the distance to the detector

    {
      int distance = detector->is_obstacle_for(this) ? 14 : 4;
      const SDL_Rect &facing_point = get_facing_point();
      const SDL_Rect &detector_rectangle = detector->get_rectangle();

      switch (animation_direction) {

      case 0:
	result = facing_point.y >= detector_rectangle.y
	  && facing_point.y < detector_rectangle.y + detector_rectangle.h
	  && facing_point.x >= detector_rectangle.x - distance
	  && facing_point.x < detector_rectangle.x + detector_rectangle.w - distance;
	break;

      case 1:
	result = facing_point.x >= detector_rectangle.x
	  && facing_point.x < detector_rectangle.x + detector_rectangle.w
	  && facing_point.y >= detector_rectangle.y + distance
	  && facing_point.y < detector_rectangle.y + detector_rectangle.h + distance;
	break;

      case 2:
	result = facing_point.y >= detector_rectangle.y
	  && facing_point.y < detector_rectangle.y + detector_rectangle.h
	  && facing_point.x >= detector_rectangle.x + distance
	  && facing_point.x < detector_rectangle.x + detector_rectangle.w + distance;
	break;

      case 3:
	result = facing_point.x >= detector_rectangle.x
	  && facing_point.x < detector_rectangle.x + detector_rectangle.w
	  && facing_point.y >= detector_rectangle.y - distance
	  && facing_point.y < detector_rectangle.y + detector_rectangle.h - distance;
	break;

      default:
	DIE("Invalid animation direction of the hero: " << animation_direction);
	break;
      }
    }
    break;

  default:
    break;
  }

  return result;
}

/**
 * Stops and restarts the current movement of the hero.
 */
void Hero::reset_movement(void) {

  PlayerMovement *movement = get_normal_movement();

  if (movement->is_moving_enabled()) {
    movement->stop();
    movement->set_moving_enabled(false, false);
    movement->set_moving_enabled(true, true);

  }
}

/**
 * This function is called when a chest overlaps the hero when it appears.
 * @param chest the chest
 */
void Hero::avoid_chest_collision(Chest *chest) {
  set_top_left_y(chest->get_top_left_y() + 16);
  reset_movement();
}

