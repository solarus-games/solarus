#include "entities/TransportableItem.h"
#include "entities/Link.h"
#include "entities/MapEntities.h"
#include "movements/PathMovement.h"
#include "ResourceManager.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "Map.h"
#include "Sound.h"
#include "ZSDX.h"

/**
 * Properties of each type of transportable item.
 */
const TransportableItem::ItemProperties TransportableItem::properties[] = {
  {"entities/pot", "stone", 0},
  {"entities/skull", "stone", 0},
  {"entities/bush", "bush", 0},
  {"entities/stone_small_white", "stone", 1},
  {"entities/stone_small_white", "stone", 2},

  /* not implemented
  {"entities/stone_big_white", "stone", 1},
  {"entities/stone_big_white", "stone", 2},
  */
};

/**
 * Creates a new transportable item with the specified type.
 * @param map the map
 * @param layer layer of the transportable item to create on the map
 * @param x x coordinate of the transportable item to create
 * @param y y coordinate of the transportable item to create
 * @param type type of transportable item to create
 * @param pickable_item the type of pickable item that appears when the transportable
 * item is lifted
 * @param pickable_item_savegame_index savegame index of the possession state of the pickable item,
 * for certain kinds of pickable items only (a key, a piece of heart...)
 */
TransportableItem::TransportableItem(Map *map, Layer layer, int x, int y, TransportableItem::ItemType type,
				     PickableItem::ItemType pickable_item, int pickable_item_savegame_index):
  EntityDetector(COLLISION_FACING_POINT, "", layer, x, y, 16, 16),
  map(map), type(type), pickable_item(pickable_item), pickable_item_savegame_index(pickable_item_savegame_index) {

  set_origin(8, 13);
  create_sprite(get_sprite_animations_id());
}

/**
 * Destructor.
 */
TransportableItem::~TransportableItem(void) {

}

/**
 * Returns the animation set of this transportable item.
 * @return the animations of the sprite
 */
string TransportableItem::get_sprite_animations_id(void) {
  return properties[type].sprite_animations_id;
}

/**
 * Returns the sound to play when this item is destroyed.
 * @return the sound to play when this item is destroyed
 */
Sound * TransportableItem::get_breaking_sound(void) {
  return ResourceManager::get_sound(properties[type].breaking_sound_id);
}

/**
 * This function is called by the engine when an entity overlaps the transportable item.
 * This is a redefinition of EntityDetector::entity_collision().
 * If the entity is the hero, we allow him to lift the item.
 * @param entity_overlapping the entity overlapping the detector
 */
void TransportableItem::entity_collision(MapEntity *entity_overlapping) {

  if (entity_overlapping->is_hero()) {

    Link *link = zsdx->game->get_link();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    Equipment *equipment = zsdx->game->get_equipment();

    int weight = properties[type].weight;
    
    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& link->get_state() == Link::FREE
	&& equipment->can_lift(weight)) {

      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LIFT);
    }
  }
}

/**
 * This function is called when the player presses the action key
 * when Link is facing this detector, and the action icon lets him do this.
 * Link lifts the item if possible.
 */
void TransportableItem::action_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  Link *link = zsdx->game->get_link();

  if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_LIFT) {

    link->start_lifting(this);

    // play the sound
    ResourceManager::get_sound("lift")->play();

    // create the pickable item
    if (pickable_item != PickableItem::NONE) {
      bool will_disappear = (pickable_item <= PickableItem::ARROW_10);
      map->get_entities()->add_pickable_item(get_layer(), get_x(), get_y(), pickable_item,
					     pickable_item_savegame_index, MovementFalling::MEDIUM, will_disappear);
    }

    // remove the item from the map
    map->get_entities()->remove_transportable_item(this);
  }
}
