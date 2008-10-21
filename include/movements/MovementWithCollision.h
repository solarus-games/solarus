#ifndef ZSDX_MOVEMENT_WITH_COLLISION_H
#define ZSDX_MOVEMENT_WITH_COLLISION_H

#include "Common.h"
#include "Movement.h"

/**
 * Abstract class for all moving entities on a map, visible or not,
 * sensible to the obstacles.
 */
class MovementWithCollision: public Movement {

 protected:

  Map *map; /**< the map containing the object */

  virtual bool collision_with_map(int dx, int dy);

 public:

  MovementWithCollision(void);
  virtual ~MovementWithCollision(void);

  void set_map(Map *map);
  void set_x(int x);
  void set_y(int y);

  virtual void update_x(void);
  virtual void update_y(void);

};

#endif
