#include "PickableItem.h"

/**
 * Creates a pickable item with the specified type.
 * This method is like a constructor, except that it can return NULL in two cases:
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
