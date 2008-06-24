#include "PickableItemFairy.h"

/**
 * Creates a pickable fairy.
 * @param map the map
 * @param layer layer of the fairy to create on the map
 * @param x x coordinate of the fairy to create
 * @param y y coordinate of the fairy to create
 */
PickableItemFairy::PickableItemFairy(Map *map, Layer layer, int x, int y):
  PickableItem(map, layer, x, y, PICKABLE_ITEM_FAIRY, false, false) {

}

/**
 * Destructor.
 */
PickableItemFairy::~PickableItemFairy(void) {

}
