#ifndef ZSDX_PICKABLE_ITEM_FAIRY_H
#define ZSDX_PICKABLE_ITEM_FAIRY_H

#include "Common.h"
#include "PickableItem.h"

/**
 * Represents a fairy placed on the map, that Link can take.
 */
class PickableItemFairy: public PickableItem {

 private:

  Movement *movement;

 protected:

  // initialization
  void initialize_movement(void);

 public:

  // creation and destruction
  PickableItemFairy(Map *map, int x, int y);
  ~PickableItemFairy(void);

  // update
  virtual void give_item_to_player(void);
  void update(void);
};

#endif
