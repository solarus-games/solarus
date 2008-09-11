#ifndef ZSDX_TARGET_MOVEMENT_H
#define ZSDX_TARGET_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"

/**
 * Movement of an object that goes from an initial point to a target point.
 */
class TargetMovement: public Movement {

 protected:

  int speed;
  int target_x;
  int target_y;

 public:

  TargetMovement(int speed, int target_x, int target_y);
  ~TargetMovement(void);

  // possible extension: target directly and entity

  bool is_finished(void);
  void update(void);
};

#endif

