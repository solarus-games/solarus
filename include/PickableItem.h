#ifndef ZSDX_PICKABLE_ITEM_H
#define ZSDX_PICKABLE_ITEM_H

#include "Common.h"
#include "SpriteOnMap.h"
#include "Layer.h"
#include "PickableItemType.h"

/**
 * A pickable item on the map (rupee, heart, bomb, fairy...).
 * This is a concrete class, used for any kind of pickable item.
 */
class PickableItem: public SpriteOnMap {

 private:

  PickableItem(void);

 public:

  ~PickableItem(void);

  static PickableItem * create(Layer layer, int x, int y, PickableItemType type);

};

#endif
