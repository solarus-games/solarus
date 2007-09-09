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
   * A rectangle defining the position of the collision box on the object.
   * The collision box is positioned from the upper-left corner
   * of the object.
   */
  SDL_Rect collision_box;

  /**
   * A rectangle defining the collision box of the object on the map.
   * The collision box is positioned from the upper-left corner
   * of the map.
   */
  SDL_Rect absolute_collision_box;

  /**
   * Sets the x position of the entity.
   * This is a redefinition of Moving::set_x() because we also need
   * to update the position of absolute_collision_box.
   * @param x the new x position
   */
  void set_x(int x);
  
  /**
   * Sets the y position of the entity.
   * This is a redefinition of Moving::set_y() because we also need
   * to update the position of absolute_collision_box.
   * @param y the new y position
   */
  void set_y(int y);
  
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

  /**
   * Returns whether the entity would collide with the map
   * if it was moved a few pixels from its position.
   * @param dx x distance between the current position and the position to check
   * @param dy y distance between the current position and the position to check
   * @return true if the entity would overlap the map tiles in this position
   */
  bool collision_with_map(int dx, int dy);

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
   * Updates the x position of the entity if it wants to move
   * (i.e. if x_move != 0).
   * This is a redefinition of Moving::update_x to make the move
   * only if there is no collision with the map.
   */
  virtual void update_x(void);
  
  /**
   * Updates the y position of the entity if it wants to move
   * (i.e. if y_move != 0).
   * This is a redefinition of Moving::update_y to make the move
   * only if there is no collision with the map.
   */
  virtual void update_y(void);

};

#endif
