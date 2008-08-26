#ifndef ZSDX_MOVEMENT_WITH_SMOOTH_COLLISION_H
#define ZSDX_MOVEMENT_WITH_SMOOTH_COLLISION_H

#include "Common.h"
#include "MovementWithCollision.h"

/**
 * Class for a moving map entity sensible to the obstacles
 * but capable of fixing its direction to continue walking.
 */
class MovementWithSmoothCollision: public MovementWithCollision {
  
 public:

  MovementWithSmoothCollision();
  virtual ~MovementWithSmoothCollision(void);

  virtual void update(void);
  virtual void update_x(void);
  virtual void update_y(void);
};

#endif
