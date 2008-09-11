#include "movements/TargetMovement.h"
#include <cmath>

/**
 * Creates a new target movement.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 */
TargetMovement::TargetMovement(int speed, int target_x, int target_y):
  speed(speed), target_x(target_x), target_y(target_y) {
  
}

/**
 * Destructor.
 */
TargetMovement::~TargetMovement(void) {

}

/**
 * Updates the movement.
 */
void TargetMovement::update(void) {

  int dx = target_x - get_x();
  int dy = target_y - get_y();

  // see if we are on the target
  if (abs(dx) <= 8 && abs(dy) <= 8) {
    stop();
  }
  else if (is_stopped()) {

    // not started yet: compute the direction

    double angle;
    if (dx != 0) {
      angle = atan((double) -dy / (double) dx);

      if (dx < 0) {
	angle += PI;
      }
    }
    else {
      // special case (cannot divide by zero and compute atan)
      angle = (dx > 0) ? 0 : PI_OVER_2;
    }
    set_speed(speed);
    set_direction(angle);
  }

  Movement::update();
}

/**
 * Returns whether the movement is finished, i.e.
 * whether the target was reached.
 */
bool TargetMovement::is_finished(void) {
  return is_stopped();
}
