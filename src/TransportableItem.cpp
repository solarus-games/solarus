#include "TransportableItem.h"
#include "ZSDX.h"
#include "GameResource.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Map.h"
#include "Sound.h"
#include "Link.h"
#include "MovementPath.h"

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
  {"entities/pot"},
  {"entities/skull"},
  {"entities/stone_white_small"},
  {"entities/stone_white_big"},
  {"entities/stone_black_small"},
  {"entities/stone_black_big"},
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
 * This function is called by the engine when an entity overlaps the transportable item.
 * This is a redefinition of EntityDetector::entity_collision().
 * If the entity is the player, we allow him to lift the item.
 * @param entity_overlapping the entity overlapping the detector
 */
void TransportableItem::entity_collision(MapEntity *entity_overlapping) {

  if (entity_overlapping == zsdx->game_resource->get_link()) {
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
void TransportableItem::action_key_pressed(void) {

  // TODO check that Link can lift the object

  Link *link = zsdx->game_resource->get_link();

  // remove the "lift" action icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_entity(NULL);

  // create the pickable item
  if (pickable_item != PICKABLE_ITEM_NONE) {
    map->get_entities()->add_pickable_item(get_layer(), get_x(), get_y(), pickable_item,
					   0, MOVEMENT_FALLING_MEDIUM, true);
    
  }

  // make Link lift the item
  link->start_lifting(this);

  // remove the item from the map
  map->get_entities()->remove_transportable_item(this);
}
