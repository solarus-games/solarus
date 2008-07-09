#ifndef ZSDX_TRANSPORTABLE_ITEM_H
#define ZSDX_TRANSPORTABLE_ITEM_H

#include "Common.h"
#include "EntityDetector.h"
#include "TransportableItemType.h"
#include "PickableItemType.h"

/**
 * Represents an entity that Link can lift
 * (a pot, a bush, a stone, etc.).
 * When Link lifts the item, it is destroyed
 * and replaced by an instance of CarriedItem that is
 * attached to Link.
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

 public:
  
  // creation and destruction
  TransportableItem(Map *map, Layer layer, int x, int y,
		    TransportableItemType type, PickableItemType pickable_item);
  ~TransportableItem(void);

  string get_sprite_animations_id(void);

  void entity_collision(MapEntity *entity_overlapping);
  void lift(void);
};


#endif
