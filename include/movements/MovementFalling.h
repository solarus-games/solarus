#ifndef ZSDX_MOVEMENT_FALLING_H
#define ZSDX_MOVEMENT_FALLING_H

#include "Common.h"
#include "PathMovement.h"

/**
 * Movement of an item that falls on the floor and
 * bounces one or several times before it stops.
 * This movement is typically used by the pickable items
 * (rupees, hearts...) that appear when the player kill an enemy.
 */
class MovementFalling: public PathMovement {

 public:

  /**
   * Indicates from which height an entity falls.
   */
  enum FallingHeight {
    NONE = -1,
    SMALL = 0,
    MEDIUM,
    BIG
  };

  // construction and destruction
  MovementFalling(FallingHeight height);
  ~MovementFalling(void);

};

#endif
