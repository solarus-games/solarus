#ifndef ZSDX_PICKABLE_ITEM_HEART_H
#define ZSDX_PICKABLE_ITEM_HEART_H

#include "Common.h"
#include "PickableItem.h"

/**
 * Represents a heart placed on the map, that Link can take.
 */
class PickableItemHeart: public PickableItem {

 protected:
  
  void initialize_sprites(void);
  void initialize_movement(void);

 public:

  PickableItemHeart(Map *map, Layer layer, int x, int y);
  ~PickableItemHeart(void);

  void update(void);
};

#endif
