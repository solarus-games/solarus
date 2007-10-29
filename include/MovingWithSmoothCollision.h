/**
 * This module defines the class MovingWithSmoothCollision.
 */

#ifndef ZSDX_MOVING_WITH_SMOOTH_COLLISION_H
#define ZSDX_MOVING_WITH_SMOOTH_COLLISION_H

#include "Common.h"
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
   * Updates the x position of the entity if it wants to move
   * (i.e. if x_move != 0).
   * This is a redefinition of MovingWithCollision::update_x to
   * handle the smooth collisions.
   */
  virtual void update_x(void);
  
  /**
   * Updates the y position of the entity if it wants to move
   * (i.e. if y_move != 0).
   * This is a redefinition of MovingWithCollision::update_y to to
   * handle the smooth collisions.
   */
  virtual void update_y(void);
};

#endif
