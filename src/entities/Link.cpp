#include "entities/Link.h"
#include "entities/CarriedItem.h"
#include "movements/Movement8ByPlayer.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "ZSDX.h"
#include "ResourceManager.h"
#include "Game.h"
#include "Equipment.h"
#include "Map.h"

/**
 * Indicates the direction of link's animation (from 0 to 4, or -1 for no change)
 * depending on the arrows pressed on the keyboard.
 */
static const int animation_directions[] = {
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
Link::Link(Equipment *equipment):
  equipment(equipment),
  tunic_sprite(NULL), sword_sprite(NULL), sword_stars_sprite(NULL), shield_sprite(NULL),
  player_movement(new Movement8ByPlayer(12)),
  state(FREE), facing_entity(NULL), counter(0), next_counter_date(0),
  walking(false), pushing_direction_mask(0xFFFF),
  lifted_item(NULL), thrown_item(NULL), treasure(NULL) {

  set_size(16, 16);
  set_origin(8, 13);
  set_movement(player_movement);
  rebuild_equipment();
}

/**
 * Destructor.
 */
Link::~Link(void) {
  delete tunic_sprite;

  if (sword_sprite != NULL) {
    delete sword_sprite;
    delete sword_stars_sprite;
  }

  if (shield_sprite != NULL) {
    delete shield_sprite;
  }

  destroy_carried_items();
}

/**
 * Returns whether this entity is the hero
 * controlled by the player.
 * @return true
 */
bool Link::is_hero(void) {
  return true;
}

/**
 * Returns the 8-directions movement controlled by the player,
 * even if it is not the current movement of Link.
 * @return the player's movement
 */
Movement8ByPlayer * Link::get_player_movement(void) {
  return player_movement;
}

/**
 * Returns the direction of Link's movement.
 * If he is not moving, -1 is returned.
 * @return Link's movement direction between 0 and 360, or -1 if he is stopped
 */
int Link::get_movement_direction(void) {

  return get_player_movement()->get_direction();
}

/**
 * Returns the point Link is looking at.
 * @return the point Link is looking at
 */
SDL_Rect Link::get_facing_point(void) {

  int direction = tunic_sprite->get_current_direction();
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
  }

  return facing_point;
}

/**
 * Sets Link's current map.
 * This function is called when the map is changed.
 * @param map the map
 */
void Link::set_map(Map *map) {

  MapEntity::set_map(map);

  get_player_movement()->set_map(map);
  
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
 * Sets Link's current map.
 * This function is called when the map is changed.
 * @param map the map
 * @param initial_direction the direction of Link (0 to 3)
 */
void Link::set_map(Map *map, int initial_direction) {

  set_map(map);

  // take the specified direction
  set_animation_direction(initial_direction);
}

/**
 * Suspends or resumes the animation of Link's sprites.
 * This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the animation, false to resume it
 */
void Link::set_suspended(bool suspended) {

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

  // counter
  if (!suspended) {
    next_counter_date += SDL_GetTicks() - when_suspended;
  }
}

/**
 * Updates Link's position, movement and animation.
 * This function is called repeteadly by the game.
 */
void Link::update(void) {

  // update the movement
  if (!zsdx->game->is_suspended()) {
    get_player_movement()->set_moving_enabled(get_state() <= SWIMMING);
    
    // specific updates in some states
    switch (state) {

    case SWORD_LOADING:
      update_sword_loading();
      break;

    case GRABBING:
    case PULLING:
      update_grabbing_pulling();
      break;

    case JUMPING:
      update_jumping();
      break;

    default:
      break;
    }
  }

  update_position();
  update_sprites();

  update_carried_items();

  if (treasure != NULL) {
    update_treasure();
  }
}

/**
 * Displays Link on the map with its current animation and
 * at its current position.
 */
void Link::display_on_map(void) {

  int x = get_x();
  int y = get_y();

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

  display_carried_items();

  if (treasure != NULL) {
    display_treasure();
  }
}

/**
 * Loads (or reloads) the sprites and sounds of Link and his equipment,
 * depending on its tunic, sword and shield as specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as Link's equipment is changed.
 */
void Link::rebuild_equipment(void) {

  int animation_direction = -1;

  // Link
  if (tunic_sprite != NULL) {
    // save the animation direction
    animation_direction = tunic_sprite->get_current_direction();
    delete tunic_sprite;
  }

  int tunic_number = equipment->get_tunic();

  tunic_sprite = new Sprite(tunic_sprite_ids[tunic_number]);
  tunic_sprite->set_animation_listener(this); // to be notified when an animation of Link is over

  // Link's sword
  if (sword_sprite != NULL) {
    delete sword_sprite;
    delete sword_stars_sprite;
    sword_sprite = NULL;
  }

  int sword_number = equipment->get_sword();

  if (sword_number > 0) {
    // Link has a sword: get the sprite and the sound
    sword_sprite = new Sprite(sword_sprite_ids[sword_number - 1]);
    sword_sprite->stop_animation();
    sword_sprite->get_animation_set()->enable_pixel_collisions();

    sword_sound = ResourceManager::get_sound(sword_sound_ids[sword_number - 1]);

    sword_stars_sprite = new Sprite(sword_stars_sprite_ids[sword_number - 1]);
    sword_stars_sprite->stop_animation();
  }

  // Link's shield
  if (shield_sprite != NULL) {
    delete shield_sprite;
    shield_sprite = NULL;
  }

  int shield_number = equipment->get_shield();

  if (shield_number > 0) {
    // Link has a shield
    shield_sprite = new Sprite(shield_sprite_ids[shield_number - 1]);
  }

  // restore the animation direction
  if (animation_direction != -1) {
    set_animation_direction(animation_direction);
  }

  // animation walking or stopped
  set_state(FREE);
  if (get_player_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * Updates Link depending on the arrows pressed.
 * This function is called when Link's direction changes (typically, because the player
 * pressed or released an arrow). It updates Link's animations and collisions according to the new movement.
 */
void Link::movement_just_changed(void) {

  // update the animation direction according to the movement direction
  int direction = get_direction();
  if (direction != -1) {

    Uint16 direction_mask = get_player_movement()->get_direction_mask();
    int old_animation_direction = tunic_sprite->get_current_direction();
    int animation_direction = animation_directions[direction_mask];
      
    if (animation_direction != old_animation_direction
	&& animation_direction != -1
	&& !is_direction_locked()) {
      // if the direction defined by the arrows has changed,
      // update the sprite's direction of animation
      // (unless Link is loading his sword)
      set_animation_direction(animation_direction);
    }
  }

  // show the animation corresponding to the movement tried by the player
  if (get_player_movement()->is_moving_enabled()) {
    // the player can move
    string animation = tunic_sprite->get_current_animation();

    bool started = get_player_movement()->is_started();

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
 * This function is called when Link's position is changed,
 * or when his direction changes. 
 */
void Link::just_moved(void) {

  set_facing_entity(NULL);
  MapEntity::just_moved();
}

/**
 * Sets the entity Link is currently facing.
 * This function is called when Link is just being
 * facing another entity.
 * @param detector the detector Link is facing
 */
void Link::set_facing_entity(Detector *detector) {

  this->facing_entity = detector;

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  KeysEffect::ActionKeyEffect action_key_effect = keys_effect->get_action_key_effect();

  // if Link stops facing an entity that showed an action icon
  if (facing_entity == NULL &&
      (action_key_effect == KeysEffect::ACTION_KEY_LIFT
       || action_key_effect == KeysEffect::ACTION_KEY_OPEN)) {

    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  }
}

/**
 * Returns whether Link is facing an obstacle, i.e. whether
 * its facing point is overlapping an obstacle of the map.
 */
bool Link::is_facing_obstacle(void) {

  SDL_Rect facing_point = get_facing_point();
  return map->collision_with_obstacles(layer, facing_point.x, facing_point.y);   
}

/**
 * Updates Link's position.
 * This function is called repeatedly by update().
 */
void Link::update_position(void) {

  // no position change when the game is suspended
  if (zsdx->game->is_suspended()) {
    return;
  }

  bool move_tried = get_movement()->has_to_move_now();
  int old_x = 0, old_y = 0;
  if (move_tried) {
    // save the current coordinates
    old_x = get_x();
    old_y = get_y();

    // try to move Link
    get_movement()->update();
  }
  
  // the rest of the function handles the "pushing" animation

  Uint16 direction_mask = get_player_movement()->get_direction_mask();

  if (state == FREE && move_tried) {
    // Link is trying to move with animation "walking"

    // see if the move has failed (i.e. if Link's coordinates have not changed)
    if (get_x() == old_x && get_y() == old_y) {

      // Link is facing an obstacle

      Uint32 now = SDL_GetTicks();
      if (pushing_direction_mask == 0xFFFF) { // we start counting to trigger animation "pushing"
	counter = 0;
	next_counter_date = now;
	pushing_direction_mask = direction_mask;
      }

      while (now >= next_counter_date) {
	counter++;
	next_counter_date += 100;
      }
      
      if (counter >= 8) {
	start_pushing(); // start animation "pushing" when the counter gets to 8
      }
    }
    else {
      // Link has just moved successfuly
      counter = 0;
      pushing_direction_mask = 0xFFFF;
    }
  }
  else {

    // stop pushing or trying to push if the state changes (for example when Link swing his sword)
    // of if the player changes his direction
    if (pushing_direction_mask != 0xFFFF && // Link is pushing or about to push
	direction_mask != pushing_direction_mask) {

      counter = 0;
      pushing_direction_mask = 0xFFFF;

      if (state == PUSHING) {
	start_free();
      }
    }
  }
}
