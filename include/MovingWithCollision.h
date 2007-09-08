/**
 * This module defines the class MovingWithCollision.
 */

#ifndef ZSDX_MOVING_WITH_COLLISION_H
#define ZSDX_MOVING_WITH_COLLISION_H

#include "Moving.h"
#include "Map.h"

/**
 * Abstract class for all moving objects on a map, visible or not,
 * sensible to the obstacles.
 */
class MovingWithCollision: public Moving {
  
 protected:

  /**
   * The map containing the object.
   */
  Map *map;

  /**
   * A rectangle defining the collision box of the object.
   * The collision box is positioned from the upper-left corner
   * of the object.
   */
  SDL_Rect collision_box;

  /**
   * Sets the collision box of the object.
   * @param collision_box the collision box, positioned
   * from the object's upper-left corner
   */
  void set_collision_box(SDL_Rect &collision_box);

  /**
   * Sets the current map of the object.
   * @param map the map
   */
  void set_map(Map *map);

 public:

  /**
   * Constructor.
   */
  inline MovingWithCollision() { }

  /**
   * Destructor.
   */
  virtual ~MovingWithCollision(void) { }

  /**
   * Updates the position (x and y) of the entity if it wants to move
   * (i.e. if x_move or y_move are not zero).
   * This is a redefinition of Moving::update_position to make the move
   * only if there is no collision with the map.
   */
  virtual void update_position(void);

};

#endif
