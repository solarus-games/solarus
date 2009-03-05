#include "entities/Hero.h"
#include "entities/CarriedItem.h"
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

/**
 * Constructor.
 */
Hero::Hero(Equipment *equipment):
  equipment(equipment), tunic_sprite(NULL), sword_sprite(NULL),
  sword_stars_sprite(NULL), shield_sprite(NULL), ground_sprite(NULL),
  normal_movement(new PlayerMovement(12)), state(FREE), facing_entity(NULL),
  end_blink_date(0), counter(0), next_counter_date(0),
  pushing_direction_mask(0xFFFF), grabbed_entity(NULL), walking(false), 
  lifted_item(NULL), thrown_item(NULL), treasure(NULL),
  last_ground_x(0), last_ground_y(0), ground(Map::NORMAL_GROUND), next_ground_sound_date(0) {

  set_size(16, 16);
  set_origin(8, 13);
  set_movement(normal_movement);
  rebuild_equipment();
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
MapEntity::EntityType Hero::get_type() {
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
 * Returns the coordinates of a point in the direction the hero's sprite is looking at.
 * This point is 1 pixel outside the hero's collision box. It is used
 * to determine the actions he can do depending on the entity he is facing
 * (a bush, a pot, a PNJ...)
 * @return the point the hero is facing
 */
SDL_Rect Hero::get_facing_point(void) {

  int direction = get_animation_direction();
  return get_facing_point(direction);
}

/**
 * Returns the point located just outside the hero's collision box,
 * in the specified direction.
 * @param direction a direction (0 to 3)
 */
SDL_Rect Hero::get_facing_point(int direction) {

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

  get_normal_movement()->set_map(map);

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
    get_normal_movement()->set_moving_enabled(get_state() <= SWIMMING);

    // specific updates in some states
    switch (state) {

    case SWORD_SWINGING:
      update_sword_swinging();
      break;

    case SWORD_LOADING:
      update_sword_loading();
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

    case DROWNING:
      update_drowning();
      break;

    default:
      break;
    }

    update_position();
    update_pushing();
    update_moving_grabbed_entity();
    update_sprites();
    update_carried_items();

    if (treasure != NULL) {
      update_treasure();
    }

    if (is_ground_visible()) {
      update_ground();
    }

    map->check_collision_with_detectors(this);

    if (thrown_item != NULL) {
      map->check_collision_with_detectors(thrown_item);
    }

    if (equipment->get_hearts() <= 0 && can_start_gameover_sequence()) {
      stop_blinking();
      zsdx->game->start_gameover_sequence();
    }
  }
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
 * pressed or released an arrow). It updates the hero's animations and collisions according to the new movement.
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
    string animation = tunic_sprite->get_current_animation();

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

  Map::Ground previous_ground = (Map::Ground) ground;
  set_ground(Map::NORMAL_GROUND);

  Map::Ground tiles_ground = map->get_tile_ground(get_layer(), get_x(), get_y());

  if (state == SWIMMING && tiles_ground != Map::DEEP_WATER) {
    stop_swimming();
  }

  if (ground != Map::GRASS && tiles_ground != ground) {
    set_ground(tiles_ground);
  }

  set_facing_entity(NULL);
  MapEntity::just_moved();

  if (ground != previous_ground) {
    start_ground();
  }

  if (ground != Map::DEEP_WATER) {
    // save the position
    last_ground_x = get_x();
    last_ground_y = get_y();
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
 * Returns whether the hero is facing an obstacle, i.e. whether
 * its facing point is overlapping an obstacle of the map.
 */
bool Hero::is_facing_obstacle(void) {

  SDL_Rect facing_point = get_facing_point();
  return map->collision_with_obstacles(layer, facing_point.x, facing_point.y, this);
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
