#ifndef ZSDX_MOVEMENT_FALLING_H
#define ZSDX_MOVEMENT_FALLING_H

#include "Common.h"
#include "MovementPath.h"

/**
 * Indicates from which height the entity falls.
 */
enum MovementFallingHeight {
  MOVEMENT_FALLING_NONE = -1,
  MOVEMENT_FALLING_SMALL = 0,
  MOVEMENT_FALLING_MEDIUM,
  MOVEMENT_FALLING_BIG
};

/**
 * Movement of an item that falls on the floor and
 * bounces one or several times before it stops.
 * This movement is typically used by the pickable items
 * (rupees, hearts...) that appear when the player kill and enemy.
 */
class MovementFalling: public MovementPath {

 public:

  MovementFalling(MovementFallingHeight height);
  ~MovementFalling(void);

};

#endif
