#ifndef ZSDX_THROWN_ITEM_MOVEMENT_H
#define ZSDX_THROWN_ITEM_MOVEMENT_H

#include "Common.h"
#include "MovementWithCollision.h"

/**
 * Movement used for an item thrown by Link
 * (a pot, a bush, etc.).
 */
class ThrownItemMovement: public MovementWithCollision {

 private:

  int y_increment;
  Uint32 next_down_date;
  int item_height;

 protected:

  void update_x(void);
  void update_y(void);

  bool collision_with_map(int dx, int dy);

 public:

  ThrownItemMovement(Map *map, int direction);
  ~ThrownItemMovement(void);

  int get_item_height(void);

  void set_suspended(bool suspended);
};

#endif
