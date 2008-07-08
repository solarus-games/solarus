#include "FollowMovement.h"
#include "MapEntity.h"

/**
 * Creates a follow movement.
 * @param entity_followed the entity to follow
 * @param x x coordinate of where this entity should be placed (relative to the entity followed)
 * @param y y coordinate of where this entity should be placed (relative to the entity followed)
 */
FollowMovement::FollowMovement(MapEntity *entity_followed, int x, int y):
  entity_followed(entity_followed), x(x), y(y) {

}

/**
 * Destructor.
 */
FollowMovement::~FollowMovement(void) {

}

/**
 * Updates the position.
 */
void FollowMovement::update_x(void) {
  set_x(entity_followed->get_x() + x);
  set_y(entity_followed->get_y() + y);  
}
