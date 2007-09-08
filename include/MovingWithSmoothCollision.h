/**
 * This module defines the class MovingWithSmoothCollision.
 */

#ifndef ZSDX_MOVING_WITH_SMOOTH_COLLISION_H
#define ZSDX_MOVING_WITH_SMOOTH_COLLISION_H

#include "MovingWithCollision.h"

/**
 * Abstract class for a moving map entity sensible to the obstacles
 * but capable of fixing its direction to continue walking.
 */
class MovingWithSmoothCollision: public MovingWithCollision {
  
 public:

  /**
   * Constructor.
   */
  inline MovingWithSmoothCollision() { }

  /**
   * Destructor.
   */
  virtual ~MovingWithSmoothCollision(void) { }

  /**
   * Updates the position (x and y) of the entity if it wants to move
   * (i.e. if x_move or y_move are not zero).
   * This is a redefinition of MovingWithCollision::update_position to
   * handle the smooth collisions.
   */
  virtual void update_position(void);

};

#endif
