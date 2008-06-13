#ifndef ZSDX_PICKABLE_ITEM_H
#define ZSDX_PICKABLE_ITEM_H

#include "Common.h"
#include "EntityDetector.h"
#include "Layer.h"
#include "PickableItemType.h"

/**
 * A pickable item on the map (rupee, heart, bomb, fairy...).
 * This is a concrete class, used for any kind of pickable item.
 */
class PickableItem: public EntityDetector {

 private:

  /**
   * Type of pickable item.
   */
  PickableItemType type;

  /**
   * Indicates whether the item is falling when it appears (except for a fairy).
   */
  bool falling;

  PickableItem(Layer layer, int x, int y, PickableItemType type, bool falling);

  static PickableItemType choose_random_type(void);
  void initialize_sprite(void);

 public:

  ~PickableItem(void);

  static PickableItem * create(Layer layer, int x, int y, PickableItemType type, bool falling);

};

#endif
