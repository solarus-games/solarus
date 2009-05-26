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
#include "entities/MapEntities.h"
#include "entities/DestinationPoint.h"
#include "entities/CarriedItem.h"
#include "entities/Detector.h"
#include "movements/PlayerMovement.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "ZSDX.h"
#include "ResourceManager.h"
#include "Game.h"
#include "Equipment.h"
#include "Map.h"

/**
 * Indicates the direction of the hero's animation (from 0 to 3, or -1 for no change)
 * depending on the arrows pressed on the keyboard.
 */
const int Hero::animation_directions[] = {
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

const int Hero::walking_speed = 9;

/**
 * Constructor.
 */
Hero::Hero(Equipment *equipment):
  equipment(equipment), tunic_sprite(NULL), sword_sprite(NULL),
  sword_stars_sprite(NULL), shield_sprite(NULL), ground_sprite(NULL),
  normal_movement(new PlayerMovement(walking_speed)),
  state(FREE), facing_entity(NULL),
  end_blink_date(0), counter(0), next_counter_date(0),
  pushing_direction_mask(0xFFFF), grabbed_entity(NULL), walking(false), 
  lifted_item(NULL), thrown_item(NULL), treasure(NULL),
  ground(GROUND_NORMAL), next_ground_sound_date(0), current_inventory_item(NULL) {

  set_size(16, 16);
  set_origin(8, 13);

  set_movement(normal_movement);
  rebuild_equipment();

  last_solid_ground_coords.x = 0;
  last_solid_ground_coords.y = 0;
}

/**
 * Destructor.
 */
Hero::~Hero(void) {
  delete tunic_sprite;
  delete shadow_sprite;
  delete sword_sprite;
  delete sword_stars_sprite;
  delete shield_sprite;
  delete ground_sprite;

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
 * Returns whether a teletransporter is currently considered as an obstacle.
 * This depends on the hero's state.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for the hero
 */
bool Hero::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return state > SWIMMING;
}

/**
 * Returns whether a hole is currently considered as an obstacle for the hero.
 * @param sensor a sensor
 * @return true if the sensors are currently an obstacle for the hero
 */
bool Hero::is_sensor_obstacle(Sensor *sensor) {
  return state == JUMPING || state == HURT;
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

  SDL_Rect collision_box = get_position_in_map();

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

  if (!map->collision_with_obstacles(get_layer(), collision_box, this)) {
    set_position_in_map(collision_box);
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

  int direction = get_animation_direction();
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

  /*
  stop_displaying_sword();

  // remove the "throw" (or other) icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

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
  */
}

/**
 * Sets the hero's current map.
 * This function is called when the map is changed.
 * @param map the map
 * @param initial_direction the direction of the hero (0 to 3)
 */
void Hero::set_map(Map *map, int initial_direction) {

  set_map(map);

  // take the specified direction
  set_animation_direction(initial_direction);
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

  // timers
  if (!suspended) {
    Uint32 now = SDL_GetTicks();
    next_counter_date += now - when_suspended;

    if (end_blink_date != 0) {
      end_blink_date += now - when_suspended;
    }
  }
}

/**
 * Updates the hero's position, movement and animation.
 * This function is called repeteadly by the game.
 */
void Hero::update(void) {

  if (!suspended) {

    // update the movement
    get_normal_movement()->set_moving_enabled(state < PUSHING, state <= GRABBING);

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

    default:
      break;
    }

    update_position();
    update_pushing();
    update_moving_grabbed_entity();
    update_sprites();

    if (treasure != NULL) {
      update_treasure();
    }

    if (is_ground_visible()) {
      update_ground();
    }

    map->check_collision_with_detectors(this);

    if (equipment->get_hearts() <= 0 && can_start_gameover_sequence()) {
      stop_blinking();
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

  if (!is_visible()) {
    return; // the hero is directly displayed by the game over sequence
  }

  int x = get_x();
  int y = get_y();

  if (state == JUMPING) {
    map->display_sprite(shadow_sprite, x, y);
    map->display_sprite(tunic_sprite, x, jump_y);
    if (equipment->has_shield()) {
      map->display_sprite(shield_sprite, x, jump_y);
    }
  }
  else {
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

    if (is_ground_visible()) {
      map->display_sprite(ground_sprite, x, y);
    }
  }

  display_carried_items();

  if (treasure != NULL) {
    display_treasure();
  }
}

/**
 * Loads (or reloads) the sprites and sounds of the hero and his equipment,
 * depending on its tunic, sword and shield as specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as the hero's equipment is changed.
 */
void Hero::rebuild_equipment(void) {

  int animation_direction = -1;

  // the hero
  if (tunic_sprite != NULL) {
    // save the animation direction
    animation_direction = tunic_sprite->get_current_direction();
    delete tunic_sprite;
  }

  int tunic_number = equipment->get_tunic();

  tunic_sprite = new Sprite(tunic_sprite_ids[tunic_number]);
  tunic_sprite->get_animation_set()->enable_pixel_collisions();

  shadow_sprite = new Sprite("entities/shadow");
  shadow_sprite->set_current_animation("big");

  // the hero's sword
  if (sword_sprite != NULL) {
    delete sword_sprite;
    delete sword_stars_sprite;
    sword_sprite = NULL;
    sword_stars_sprite = NULL;
  }

  int sword_number = equipment->get_sword();

  if (sword_number > 0) {
    // the hero has a sword: get the sprite and the sound
    sword_sprite = new Sprite(sword_sprite_ids[sword_number - 1]);
    sword_sprite->stop_animation();
    sword_sprite->get_animation_set()->enable_pixel_collisions();

    sword_sound = ResourceManager::get_sound(sword_sound_ids[sword_number - 1]);

    sword_stars_sprite = new Sprite(sword_stars_sprite_ids[sword_number - 1]);
    sword_stars_sprite->stop_animation();
  }

  // the hero's shield
  if (shield_sprite != NULL) {
    delete shield_sprite;
    shield_sprite = NULL;
  }

  int shield_number = equipment->get_shield();

  if (shield_number > 0) {
    // the hero has a shield
    shield_sprite = new Sprite(shield_sprite_ids[shield_number - 1]);
  }

  // restore the animation direction
  if (animation_direction != -1) {
    set_animation_direction(animation_direction);
  }

  // animation walking or stopped
  set_state(FREE);
  if (get_normal_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
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
    int old_animation_direction = tunic_sprite->get_current_direction();
    int animation_direction = animation_directions[direction_mask];

    if (animation_direction != old_animation_direction
	&& animation_direction != -1
	&& !is_direction_locked()) {
      // if the direction defined by the arrows has changed,
      // update the sprite's direction of animation
      // (unless the hero is loading his sword)
      set_animation_direction(animation_direction);
    }
  }

  // show the animation corresponding to the movement tried by the player
  if (get_normal_movement()->is_moving_enabled()) {

    // the player can move
    bool started = get_normal_movement()->is_started();

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
 * This function is called when the hero's position is changed,
 * or when his direction changes.
 */
void Hero::just_moved(void) {

  if (state == RETURNING_TO_SOLID_GROUND) {
    // do not take care of the ground when returning to a solid ground point
    return;
  }

  Ground previous_ground = ground;
  set_ground(GROUND_NORMAL);

  // see if the hero is on a special ground
  Ground tiles_ground = map->get_tile_ground(get_layer(), get_x(), get_y());

  if (state == SWIMMING && tiles_ground != GROUND_DEEP_WATER) {
    stop_swimming();
  }

  if (ground != GROUND_GRASS && tiles_ground != ground) {
    // set the ground indicated by the tiles
    set_ground(tiles_ground);
  }

  set_facing_entity(NULL);
  MapEntity::just_moved(); // to set the ground indicated by the entities

  if (ground != previous_ground) {
    start_ground();
  }

  if (ground < GROUND_DEEP_WATER && state != JUMPING && state != RETURNING_TO_SOLID_GROUND) {
    // save the hero's last valid position
    /*
    std::cout << SDL_GetTicks() << "\tsaving coords (" << get_coordinates().x << ","
	      << get_coordinates().y << "), ground = " << ground
	      << ", state = " << state << "\n";
    */
    last_solid_ground_coords = get_coordinates();
  }
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

  SDL_Rect collision_box = get_position_in_map();
  switch (get_animation_direction()) {

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
    DIE("Invalid animation direction '" << get_animation_direction() << "'");
    break;
  }

  return map->collision_with_obstacles(layer, collision_box, this);

  /* old version with only one point: problems when the hero
     cannot pass but the facing point can
  SDL_Rect facing_point = get_facing_point();
  return map->collision_with_obstacles(layer, facing_point.x, facing_point.y, this);
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

  int destination_point_index = map->get_destination_point_index();
  
  if (destination_point_index >= 0) {

    // the location is specified by a destination point object
    DestinationPoint *destination_point =
      map->get_entities()->get_destination_point(destination_point_index);

    set_map(map, destination_point->get_direction());
    set_x(destination_point->get_x());
    set_y(destination_point->get_y());
    map->get_entities()->set_hero_layer(destination_point->get_layer());

    destroy_carried_items();
    start_free();
  }
  else if (destination_point_index == -1) {

    // the hero's coordinates are the same as on previous map
    set_map(map);

    destroy_carried_items();
    start_free();
  }
  else {

    // only one coordinate is changed
    set_map(map);

    switch (map->get_destination_side()) {

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
      DIE("Invalid destination side: " << map->get_destination_side());
    }
  }
}

/**
 * This function is called when the opening transition of the map is finished.
 * The position of the hero is updated if necessary.
 */
void Hero::opening_transition_finished(void) {

  if (map->get_destination_point_index() == -2) {
    // the hero was placed on the side of the map:
    // there was a scrolling between the previous map and this one

    switch (map->get_destination_side()) {

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
      DIE("Invalid destination side: " << map->get_destination_side());
    }
  }
}

/**
 * When the sword sprite collides with a detector,
 * this function can be called to determine whether the hero is
 * really striking this particular detector only.
 * This depends on the hero state, his direction and his
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
  int animation_direction = get_animation_direction();

  switch (state) {

  case SPIN_ATTACK:
    // during a spin attack, any sprite collision is considered as a strike
    result = true;
    break;

  case SWORD_TAPPING:
    // when the hero is tapping his sword against a wall, this wall should be detector
    result = detector->is_obstacle_for(this)
      && facing_entity == detector
      && tunic_sprite->get_current_frame() >= 3;
    break;

  case SWORD_SWINGING:
    // for a normal sword swing, check the distance to the detector

    {
      int distance = detector->is_obstacle_for(this) ? 14 : 4;
      const SDL_Rect &facing_point = get_facing_point();
      const SDL_Rect &detector_position = detector->get_position_in_map();

      switch (animation_direction) {

      case 0:
	result = facing_point.y >= detector_position.y
	  && facing_point.y < detector_position.y + detector_position.h
	  && facing_point.x >= detector_position.x - distance
	  && facing_point.x < detector_position.x + detector_position.w - distance;
	break;

      case 1:
	result = facing_point.x >= detector_position.x
	  && facing_point.x < detector_position.x + detector_position.w
	  && facing_point.y >= detector_position.y + distance
	  && facing_point.y < detector_position.y + detector_position.h + distance;
	break;

      case 2:
	result = facing_point.y >= detector_position.y
	  && facing_point.y < detector_position.y + detector_position.h
	  && facing_point.x >= detector_position.x + distance
	  && facing_point.x < detector_position.x + detector_position.w + distance;
	break;

      case 3:
	result = facing_point.x >= detector_position.x
	  && facing_point.x < detector_position.x + detector_position.w
	  && facing_point.y >= detector_position.y - distance
	  && facing_point.y < detector_position.y + detector_position.h - distance;
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
