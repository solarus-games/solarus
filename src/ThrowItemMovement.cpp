#include "ThrowItemMovement.h"

/**
 * Creates a throw item movement.
 * @param direction direction where the item is thrown (0 to 3)
 */
ThrowItemMovement::ThrowItemMovement(int direction):
  Movement() {
  
  set_speed(10);
  set_direction(direction * 90);
}

/**
 * Destructor.
 */
ThrowItemMovement::~ThrowItemMovement(void) {

}

/**
 * Updates the x position.
 */
void ThrowItemMovement::update_x(void) {
  Movement::update_x();
}

/**
 * Updates the y position.
 */
void ThrowItemMovement::update_y(void) {
  Movement::update_y();
}
