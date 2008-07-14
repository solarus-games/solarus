#include "CarriedItem.h"
#include "TransportableItem.h"
#include "Link.h"
#include "MovementPath.h"
#include "FollowMovement.h"
#include "ThrowItemMovement.h"
#include "Layer.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "KeysEffect.h"
#include "GameResource.h"
#include "Sound.h"
#include "Map.h"

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
 * based on a transportable item (i.e. the item placed on the map
 * before Link lifts it).
 * @param transportable_item a transportable item
 */
CarriedItem::CarriedItem(Link *link, TransportableItem *transportable_item):
  MapEntity(), link(link), is_lifting(true), is_throwing(false), is_breaking(false) {

  // put the item on Link's layer
  set_layer(link->get_layer());

  // align correctly the item with Link
  int direction = link->get_animation_direction();
  if (direction % 2 == 0) {
    set_x(transportable_item->get_x());
    set_y(link->get_y());
  }
  else {
    set_x(link->get_x());
    set_y(transportable_item->get_y());
  }
  set_origin(transportable_item->get_origin());
  set_size(transportable_item->get_width(), transportable_item->get_height());

  // create the movement and the sprite
  MovementPath *movement = new MovementPath(lifting_translations[direction], 6, 100, false);
  create_sprite(transportable_item->get_sprite_animations_id());
  set_movement(movement);

  // create the breaking sound
  breaking_sound = transportable_item->get_breaking_sound();

  // create the shadow
  shadow_sprite = new Sprite("entities/shadow");
  shadow_sprite->set_current_animation("big");
}

/**
 * Destructor.
 */
CarriedItem::~CarriedItem(void) {

}

/**
 * This function is called when Links stops walking while
 * carrying the item. The item also stops moving.
 */
void CarriedItem::set_animation_stopped(void) {

  if (!is_lifting && !is_throwing) {
    Sprite *sprite = get_last_sprite();
    sprite->set_current_animation("stopped");
  }
}

/**
 * This function is called when Links starts walking while
 * carrying the item. The item moves like him.
 */
void CarriedItem::set_animation_walking(void) {
  if (!is_lifting && !is_throwing) {
    Sprite *sprite = get_last_sprite();
    sprite->set_current_animation("walking");
  }
}

/**
 * Throws the item.
 * @param map the current map
 * @param direction direction where Link throws the item (0 to 3)
 */
void CarriedItem::throw_item(Map *map, int direction) {

  is_throwing = true;

  // play the sound
  zsdx->game_resource->get_sound("throw")->play();

  // stop the sprite animation
  Sprite *sprite = get_last_sprite();
  sprite->set_current_animation("stopped");

  // remove the "throw" icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(ACTION_KEY_NONE);

  // set the movement
  clear_movement();
  set_movement(new ThrowItemMovement(map, direction));
}

/**
 * Returns whether the item is broken.
 * @return true if the item is broken
 */
bool CarriedItem::is_broken(void) {
  return is_breaking && get_last_sprite()->is_over() ;
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

  Sprite *sprite = get_last_sprite();

  // when Link finishes lifting the item, start carrying it
  if (is_lifting) {
    MovementPath *movement = (MovementPath*) get_movement();
    if (movement->is_finished()) {
      is_lifting = false;

      // make Link carry the item
      link->start_carrying();

      // action icon "throw"
      KeysEffect *keys_effect = zsdx->game->get_keys_effect();
      keys_effect->set_action_key_effect(ACTION_KEY_THROW);

      // make the item follow Link
      clear_movement();
      set_movement(new FollowMovement(link, 0, -18));
    }
  }
  else if (is_throwing) {

    if (get_movement()->is_stopped()) {
      // destroy the item
      is_throwing = false;
      is_breaking = true;
      breaking_sound->play();
      sprite->set_current_animation("destroy");
      set_movement(NULL);
    }
    else {
      shadow_sprite->update_current_frame();
    }
  }
}

/**
 * Displays the carried item on the map.
 * This is a redefinition of MapEntity::display_on_map
 * to display the shadow independently of the item movement.
 */
void CarriedItem::display_on_map(Map *map) {

  if (is_throwing) {
    // display the shadow
    ThrowItemMovement *movement = (ThrowItemMovement*) get_movement();
    map->display_sprite(shadow_sprite, get_x(), get_y() + movement->get_item_height());
  }

  // display the sprite
  MapEntity::display_on_map(map);
}
