#ifndef ZSDX_MOVABLE_WITH_COLLISION_H
#define ZSDX_MOVABLE_WITH_COLLISION_H

#include "movable.h"
#include "map.h"

/**
 * Abstract class for all moving objects on a map, visible or not,
 * sensible to the obstacles.
 */
class MovableWithCollision: public Movable {
  
 private:
  /**
   * The map containing the object.
   */
  Map *map;

  /**
   * A rectangle defining the collision box of the object.
   * The collision box is positionned from the upper-left corner
   * of the object's position (where_in_map).
   */
  SDL_Rect collision_box;

 protected:
  /**
   * Sets the collision box of the object.
   */
  void set_collision_box(SDL_Rect &collision_box);

  /**
   * Sets the current map of the object.
   */
  void set_map(Map *map);

 public:
  /* Constructor
   */
  MovableWithCollision();

  /* Destructor
   */
  virtual ~MovableWithCollision(void) { }

  /* Update the x and y position of the entity if necessary,
   * stopping the movement if a collision is detected.
   */
  virtual void update_position(void);

};

#endif
