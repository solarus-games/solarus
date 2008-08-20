#ifndef ZSDX_MOVEMENT_FALLING_H
#define ZSDX_MOVEMENT_FALLING_H

#include "Common.h"
#include "MovementPath.h"
#include "MovementFallingHeight.h"

/**
 * Movement of an item that falls on the floor and
 * bounces one or several times before it stops.
 * This movement is typically used by the pickable items
 * (rupees, hearts...) that appear when the player kill an enemy.
 */
class MovementFalling: public MovementPath {

 public:

  MovementFalling(MovementFallingHeight height);
  ~MovementFalling(void);

};

#endif
