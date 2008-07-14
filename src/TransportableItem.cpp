#include "TransportableItem.h"
#include "ZSDX.h"
#include "GameResource.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Map.h"
#include "Link.h"
#include "MovementPath.h"
#include "Sound.h"

/**
 * This structure defines the properties of a transportable item type.
 */
struct TransportableItemProperties {
  SpriteAnimationsId sprite_animations_id; /**< animation set used for this type of transportable item */
  SoundId breaking_sound_id;
  // TODO weight
};

/**
 * Properties of each type of transportable item.
 */
static const TransportableItemProperties properties[] = {
  {"entities/pot", "stone"},
  {"entities/skull", "stone"},
  {"entities/bush", "bush"},
  {"entities/stone_white_small", "stone"},
  {"entities/stone_white_big", "stone"},
  {"entities/stone_black_small", "stone"},
  {"entities/stone_black_big", "stone"},
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
 * @param unique_id unique id of the pickable item, for certain kinds of pickable
 * items only (a key, a piece of heart...)
 */
TransportableItem::TransportableItem(Map *map, Layer layer, int x, int y,
				     TransportableItemType type,
				     PickableItemType pickable_item, int unique_id):
  EntityDetector(COLLISION_WITH_ENTITY_FACING_POINT, "", layer, x, y, 16, 16),
  map(map), type(type), pickable_item(pickable_item) {

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
 * @param the animations of the sprite
 */
string TransportableItem::get_sprite_animations_id(void) {
  return properties[type].sprite_animations_id;
}

/**
 * Returns the sound to play when this item is destroyed.
 * @return the sound to play when this item is destroyed
 */
Sound * TransportableItem::get_breaking_sound(void) {
  return zsdx->game_resource->get_sound(properties[type].breaking_sound_id);
}

/**
 * This function is called by the engine when an entity overlaps the transportable item.
 * This is a redefinition of EntityDetector::entity_collision().
 * If the entity is the player, we allow him to lift the item.
 * @param entity_overlapping the entity overlapping the detector
 */
void TransportableItem::entity_collision(MapEntity *entity_overlapping) {

  if (entity_overlapping == zsdx->game_resource->get_link()) {

    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    if (keys_effect->get_action_key_effect() == ACTION_KEY_NONE) {
      keys_effect->set_action_key_effect(ACTION_KEY_LIFT);
    }
  }
}

/**
 * This function is called by Link when he lifts this item.
 * @param map the map
 */
void TransportableItem::lift(void) {

  // TODO check that Link can lift the object

  // play the sound
  zsdx->game_resource->get_sound("lift")->play();

  // create the pickable item
  if (pickable_item != PICKABLE_ITEM_NONE) {
    map->get_entities()->add_pickable_item(get_layer(), get_x(), get_y(), pickable_item,
					   0, MOVEMENT_FALLING_MEDIUM, true);
  }

  // remove the item from the map
  map->get_entities()->remove_transportable_item(this);
}
