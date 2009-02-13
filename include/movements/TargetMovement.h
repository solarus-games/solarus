#ifndef ZSDX_TARGET_MOVEMENT_H
#define ZSDX_TARGET_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"

/**
 * Movement of an object that goes from an initial point to a target point.
 */
class TargetMovement: public Movement {

 protected:

  int target_x;
  int target_y;
  int sign_x;   /**< sign of the x movement (1: right, -1: left) */
  int sign_y;   /**< sign of the y movement (1: up, -1: down) */

  int speed;

 public:

  TargetMovement(int target_x, int target_y, int speed);
  ~TargetMovement(void);

  // possible extension: target directly and entity

  bool is_finished(void);
  void update(void);
};

#endif

