#ifndef ZSDX_FALLING_ON_FLOOR_MOVEMENT_H
#define ZSDX_FALLING_ON_FLOOR_MOVEMENT_H

#include "Common.h"
#include "PathMovement.h"

/**
 * Movement of an item that falls on the floor and
 * bounces one or several times before it stops.
 * This movement is typically used by the pickable items
 * (rupees, hearts...) that appear when the player kill an enemy.
 */
class FallingOnFloorMovement: public PathMovement {

 public:

  /**
   * Indicates from which height an entity falls.
   */
  enum Height {
    NONE = -1,
    LOW = 0,
    MEDIUM,
    HIGH
  };

  // construction and destruction
  FallingOnFloorMovement(Height height);
  ~FallingOnFloorMovement(void);

};

#endif
