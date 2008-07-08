#ifndef ZSDX_TRANSPORTABLE_ITEM_H
#define ZSDX_TRANSPORTABLE_ITEM_H

#include "Common.h"
#include "EntityDetector.h"
#include "TransportableItemType.h"
#include "PickableItemType.h"

/**
 * The possible states of the transportable item.
 */
enum TransportableItemState {
  TRANSPORTABLE_ITEM_STATE_STOPPED,
  TRANSPORTABLE_ITEM_STATE_LIFTING,
  TRANSPORTABLE_ITEM_STATE_CARRIED,
  TRANSPORTABLE_ITEM_STATE_THROWN,
};

/**
 * Represents an entity that Link can lift, transport and throw.
 */
class TransportableItem: public EntityDetector {

 private:

  /**
   * The map.
   */
  Map *map;
  
  /**
   * The type of transportable item.
   */
  TransportableItemType type;

  /**
   * The type of pickable item that appears when the item is lifted.
   */
  PickableItemType pickable_item;

  /**
   * Current state of the transportable item.
   */
  TransportableItemState state;

 public:
  
  // creation and destruction
  TransportableItem(Map *map, Layer layer, int x, int y,
		    TransportableItemType type, PickableItemType pickable_item);
  ~TransportableItem(void);

  void entity_collision(MapEntity *entity_overlapping);
  void action_key_pressed(void);

  // update
  void update(void);
};


#endif
