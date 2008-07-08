#ifndef ZSDX_TRANSPORTABLE_ITEM_H
#define ZSDX_TRANSPORTABLE_ITEM_H

#include "Common.h"
#include "EntityDetector.h"
#include "TransportableItemType.h"
#include "PickableItemType.h"

/**
 * Represents an entity that Link can lift, transport and throw.
 */
class TransportableItem: public EntityDetector {

 private:
  
  /**
   * The type of transportable item.
   */
  TransportableItemType type;

  /**
   * The type of pickable item that appears when the item is lifted.
   */
  PickableItemType pickable_item;

 public:
  
  // creation and destruction
  TransportableItem(Map *map, Layer layer, int x, int y,
		    TransportableItemType type, PickableItemType pickable_item);
  ~TransportableItem(void);

  void entity_collision(MapEntity *entity_overlapping);
  void action_key_pressed(Map *map);
};


#endif
