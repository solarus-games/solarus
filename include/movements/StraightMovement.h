#ifndef ZSDX_STRAIGHT_MOVEMENT_H
#define ZSDX_STRAIGHT_MOVEMENT_H

#include "Common.h"
#include "movements/MovementWithSmoothCollision.h"

/**
 * Movement for an entity that follows a straight trajectory
 * for a given amount of time.
 */
class StraightMovement: public MovementWithCollision {

 private:

  Uint32 end_movement_date;
  bool finished;

 public:

  StraightMovement(Map *map, int speed, int direction, Uint32 time);
  StraightMovement(Map *map, int speed, double direction, Uint32 time);
  ~StraightMovement(void);

  void update(void);
  void set_suspended(bool suspended);
  bool is_finished(void);
  void start(int speed, int direction, Uint32 time);
  void start(int speed, double direction, Uint32 time);
};

#endif
