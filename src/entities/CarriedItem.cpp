#include "entities/CarriedItem.h"
#include "entities/DestructibleItem.h"
#include "entities/Link.h"
#include "movements/PathMovement.h"
#include "movements/FollowMovement.h"
#include "movements/ThrownItemMovement.h"
#include "Sprite.h"
#include "Game.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Map.h"
#include "ZSDX.h"

/**
 * Movement of the item when Link is lifting it.
 */
static const SDL_Rect lifting_translations[4][6] = {
  {{0,0}, {0,0}, {-3,-3}, {-5,-3}, {-5,-2}},
  {{0,0}, {0,0}, {0,-1}, {0,-1}, {0,0}},
  {{0,0}, {0,0}, {3,-3}, {5,-3}, {5,-2}},
  {{0,0}, {0,0}, {0,-10}, {0,-12}, {0, 0}},
};

/**
 * Creates a carried item (i.e. an item carried by Link)
 * based on a destructible item (i.e. the item placed on the map
 * before Link lifts it).
 * @param link Link
 * @param destructible_item a destructible item
 */
CarriedItem::CarriedItem(Link *link, DestructibleItem *destructible_item):
  MapEntity(), link(link), is_lifting(true), is_throwing(false), is_breaking(false) {

  // put the item on Link's layer
  set_layer(link->get_layer());

  // align correctly the item with Link
  int direction = link->get_animation_direction();
  if (direction % 2 == 0) {
    set_x(destructible_item->get_x());
    set_y(link->get_y());
  }
  else {
    set_x(link->get_x());
    set_y(destructible_item->get_y());
  }
  set_origin(destructible_item->get_origin());
  set_size(destructible_item->get_width(), destructible_item->get_height());

  // create the movement and the sprite
  PathMovement *movement = new PathMovement(lifting_translations[direction], 6, 100, false);
  create_sprite(destructible_item->get_animation_set_id());
  set_movement(movement);

  // create the breaking sound
  destruction_sound = destructible_item->get_destruction_sound();

  // create the shadow
  shadow_sprite = new Sprite("entities/shadow");
  shadow_sprite->set_current_animation("big");

  // damage on enemies
  damage_on_enemies = destructible_item->get_damage_on_enemies();
}

/**
 * Destructor.
 */
CarriedItem::~CarriedItem(void) {
  delete shadow_sprite;
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType CarriedItem::get_type() {
  return CARRIED_ITEM;
}

/**
 * Returns the damage this item can cause to ennemies.
 * @return the damage on enemies
 */
int CarriedItem::get_damage_on_enemies(void) {
  return damage_on_enemies;
}

/**
 * This function is called when Links stops walking while
 * carrying the item. The item also stops moving.
 */
void CarriedItem::set_animation_stopped(void) {

  if (!is_lifting && !is_throwing) {
    Sprite *sprite = get_sprite();
    sprite->set_current_animation("stopped");
  }
}

/**
 * This function is called when Links starts walking while
 * carrying the item. The item moves like him.
 */
void CarriedItem::set_animation_walking(void) {
  if (!is_lifting && !is_throwing) {
    Sprite *sprite = get_sprite();
    sprite->set_current_animation("walking");
  }
}

/**
 * Throws the item.
 * @param map the current map
 * @param direction direction where Link throws the item (0 to 3)
 */
void CarriedItem::throw_item(Map *map, int direction) {

  this->map = map;
  this->throwing_direction = direction;
  is_throwing = true;

  // play the sound
  ResourceManager::get_sound("throw")->play();

  // stop the sprite animation
  Sprite *sprite = get_sprite();
  sprite->set_current_animation("stopped");

  // remove the "throw" icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  // set the movement
  clear_movement();
  set_movement(new ThrownItemMovement(map, direction));
}

/**
 * Returns whether the item is broken.
 * @return true if the item is broken
 */
bool CarriedItem::is_broken(void) {
  return is_breaking && get_sprite()->is_over() ;
}

/**
 * This function is called by the map when the game is suspended or resumed.
 * This is a redefinition of MapEntity::set_suspended() to suspend the movement
 * of the shadow when the item is being thrown.
 * @param suspended true to suspend the entity, false to resume it
 */
void CarriedItem::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement

  if (is_throwing) {
    // suspend the shadow
    shadow_sprite->set_suspended(true);
  }
}

/**
 * This function is called repeatedly by Link.
 * When the lift movement finishes, Link starts carrying the item.
 */
void CarriedItem::update(void) {

  // update the sprite and the position
  MapEntity::update();

  // when Link finishes lifting the item, start carrying it
  if (is_lifting) {
    PathMovement *movement = (PathMovement*) get_movement();
    if (movement->is_finished()) {
      is_lifting = false;

      // make Link carry the item
      link->start_carrying();

      // action icon "throw"
      KeysEffect *keys_effect = zsdx->game->get_keys_effect();
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);

      // make the item follow Link
      clear_movement();
      set_movement(new FollowMovement(link, 0, -18));
    }
  }
  else if (is_throwing) {

    ThrownItemMovement *movement = (ThrownItemMovement*) get_movement();

    if (movement->is_stopped()) {
      // destroy the item

      is_throwing = false;
      is_breaking = true;
      destruction_sound->play();
      get_sprite()->set_current_animation("destroy");

      if (throwing_direction == 3) {
	// destroy the item where its shadow is
	set_y(get_y() + movement->get_item_height());
      }

      clear_movement();
    }
    else {
      map->check_collision_with_detectors(this);
    }
    shadow_sprite->update();
  }
}

/**
 * Displays the carried item on the map.
 * This is a redefinition of MapEntity::display_on_map
 * to display the shadow independently of the item movement.
 */
void CarriedItem::display_on_map(void) {

  if (is_throwing) {
    // display the shadow
    ThrownItemMovement *movement = (ThrownItemMovement*) get_movement();
    map->display_sprite(shadow_sprite, get_x(), get_y() + movement->get_item_height());
  }

  // display the sprite
  MapEntity::display_on_map();
}

/**
 * This function is called when an enemy collides with the carried item.
 * @param enemy the enemy
 */
void CarriedItem::collision_with_enemy(Enemy *enemy) {

  if (is_throwing) {
    enemy->hurt(Enemy::ATTACK_THROWN_ITEM, this);
    get_movement()->stop();    
  }
}
