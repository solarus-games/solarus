#ifndef ZSDX_PICKABLE_ITEM_FAIRY_H
#define ZSDX_PICKABLE_ITEM_FAIRY_H

#include "Common.h"
#include "PickableItem.h"

/**
 * Represents a fairy placed on the map, that Link can take.
 */
class PickableItemFairy: public PickableItem {

 public:

  PickableItemFairy(Map *map, Layer layer, int x, int y);
  ~PickableItemFairy(void);
};

#endif
