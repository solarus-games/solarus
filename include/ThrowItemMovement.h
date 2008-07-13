#ifndef ZSDX_THROW_ITEM_MOVEMENT_H
#define ZSDX_THROW_ITEM_MOVEMENT_H

#include "Common.h"
#include "Movement.h"

/**
 * Movement used for an item thrown by Link
 * (a pot, a bush, etc.).
 */
class ThrowItemMovement: public Movement {



 protected:

  void update_x(void);
  void update_y(void);

 public:

  ThrowItemMovement(int direction);
  ~ThrowItemMovement(void);
};

#endif
