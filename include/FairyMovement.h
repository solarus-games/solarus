#ifndef ZSDX_FAIRY_MOVEMENT_H
#define ZSDX_FAIRY_MOVEMENT_H

#include "Common.h"
#include "Movement.h"

/**
 * Defines the movement of a pickable fairy.
 */
class FairyMovement: public Movement {

 private:
  
  /**
   * Date of the next direction change
   */
  Uint16 next_direction_change;

  void set_random_direction(void);

 public:

  FairyMovement(void);
  ~FairyMovement(void);

  void update(void);
};

#endif
