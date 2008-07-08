#include "TransportableItem.h"
#include "ZSDX.h"
#include "GameResource.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Map.h"
#include "Sound.h"

/**
 * This structure defines the properties of a transportable item type.
 */
struct TransportableItemProperties {
  SpriteAnimationsId sprite_animations_id; /**< animation set used for this type of transportable item */
  // TODO weight
};

/**
 * Properties of each type of transportable item.
 */
static const TransportableItemProperties properties[] = {
  {"pot"},
  {"skull"},
  {"stone_white_small"},
  {"stone_white_big"},
  {"stone_black_small"},
  {"stone_black_big"},
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
 */
TransportableItem::TransportableItem(Map *map, Layer layer, int x, int y,
				     TransportableItemType type, PickableItemType pickable_item):
  EntityDetector(COLLISION_WITH_ENTITY_FACING_POINT, "", layer, x, y, 16, 16),
  type(type), pickable_item(pickable_item) {

  set_origin(8, 13);
}

/**
 * Destructor.
 */
TransportableItem::~TransportableItem(void) {

}

/**
 * This function is called by the engine when an entity overlaps the transportable item.
 * This is a redefinition of EntityDetector::entity_collision().
 * If the entity is the player, we allow him to lift the item.
 * @param entity_overlapping the entity overlapping the detector
 */
void TransportableItem::entity_collision(MapEntity *entity_overlapping) {

  if (entity_overlapping == (MapEntity*) zsdx->game_resource->get_link()) {
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    keys_effect->set_action_key_effect(ACTION_KEY_LIFT);
  }
}

/**
 * This function is called by the game when the player
 * pressed the action key and Link is facing this transportable item.
 * The transportable item is then lifted by Link (if possible).
 * @param map the map
 */
void TransportableItem::action_key_pressed(Map *map) {

  // TODO check that Link can lift the object
  map->get_entities()->remove_transportable_item(this);
  zsdx->game_resource->get_sound("lift")->play();
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_entity(NULL);
}
