#include "movements/FollowMovement.h"
#include "entities/MapEntity.h"

/**
 * Creates a follow movement.
 * @param map the map (can be NULL if with_collisions is false)
 * @param entity_followed the entity to follow
 * @param x x coordinate of where this entity should be placed (relative to the entity followed)
 * @param y y coordinate of where this entity should be placed (relative to the entity followed)
 * @param with_collisions true to make the movement sensitive to obstacles
 */
FollowMovement::FollowMovement(Map *map, MapEntity *entity_followed, int x, int y, bool with_collisions):
  MovementWithCollision(map),
  entity_followed(entity_followed), x(x), y(y), with_collisions(with_collisions), finished(false) {

}

/**
 * Destructor.
 */
FollowMovement::~FollowMovement(void) {

}

/**
 * Returns whether the movement is finished.
 * @return true if there was a collision
 */
bool FollowMovement::is_finished(void) {
  return finished;
}

/**
 * Updates the position.
 */
void FollowMovement::update(void) {

  int next_x = entity_followed->get_x() + x;
  int next_y = entity_followed->get_y() + y;

  int dx = next_x - get_x();
  int dy = next_y - get_y();

  if (with_collisions && !finished && (dx != 0 || dy != 0)) {

    if (!collision_with_map(dx, dy)) {
      set_x(next_x);
      set_y(next_y);
    }
    else {
      finished = true;
    }
  }
}
