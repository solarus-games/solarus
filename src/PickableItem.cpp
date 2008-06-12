#include "PickableItem.h"

/**
 * Creates a pickable item with the specified type.
 * The type must a normal one (not PICKABLE_ITEM_NONE or PICKABLE_ITEM_RANDOM).
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (must be a normal item)
 */
PickableItem::PickableItem(Layer layer, int x, int y, PickableItemType type):
  SpriteOnMap(layer, x, y), type(type) {

  // TODO  set_size, set_hotspot, set_sprite
  set_movement(NULL); // TODO sauf la fée
}

/**
 * Creates a pickable item with the specified type.
 * This method acts like a constructor, except that it can return NULL in two cases:
 * - if the specified type is PICKABLE_ITEM_NONE
 * or:
 * - if the specified type is PICKABLE_ITEM_RANDOM and the random type chosen is PICKABLE_ITEM_NONE
 * @param layer layer of the pickable item to create on the map
 * @param x x coordinate of the pickable item to create
 * @param y y coordinate of the pickable item to create
 * @param type type of pickable item to create (can be a normal item, PICKABLE_ITEM_NONE or PICKABLE_ITEM_RANDOM)
 */
PickableItem * PickableItem::create(Layer layer, int x, int y, PickableItemType type) {

  // TODO
  return NULL;
}
