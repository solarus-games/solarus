#ifndef ZSDX_THROW_ITEM_MOVEMENT_H
#define ZSDX_THROW_ITEM_MOVEMENT_H

#include "Common.h"
#include "MovementWithCollision.h"

/**
 * Movement used for an item thrown by Link
 * (a pot, a bush, etc.).
 */
class ThrowItemMovement: public MovementWithCollision {

 private:

  int y_increment;
  Uint16 next_down_date;

 protected:

  void update_x(void);
  void update_y(void);

 public:

  ThrowItemMovement(Map *map, int direction);
  ~ThrowItemMovement(void);

  int get_item_height(void);

  void set_suspended(bool suspended);
};

#endif
