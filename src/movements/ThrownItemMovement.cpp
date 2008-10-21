#include "movements/ThrownItemMovement.h"
#include "Map.h"
#include "entities/MapEntity.h"

/**
 * Creates a throw item movement.
 * @param map the map where the item is thrown
 * @param direction direction where the item is thrown (0 to 3)
 */
ThrownItemMovement::ThrownItemMovement(int direction):
  MovementWithCollision() {

  set_speed(20);
  set_direction(direction * 90);

  y_increment = -2;
  next_down_date = SDL_GetTicks() + 40;
  item_height = 18;
}

/**
 * Destructor.
 */
ThrownItemMovement::~ThrownItemMovement(void) {

}

/**
 * Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 * This is a redefinition of MovementWithCollision::collision_with_map
 * because of the special collision box of a thrown item.
 * @param dx x distance between the current position and the position to check
 * @param dy y distance between the current position and the position to check
 * @return true if the entity would overlap the map tiles in this position
 */
bool ThrownItemMovement::collision_with_map(int dx, int dy) {

  // place the collision box where we want to check the collisions
  SDL_Rect collision_box = *entity->get_position_in_map();
  collision_box.x += dx;
  collision_box.y += get_item_height() + dy;
  collision_box.w = 16;
  collision_box.h = 16;

  bool collision = map->collision_with_obstacles(entity->get_layer(), collision_box);

  return collision;
}

/**
 * Updates the x position.
 */
void ThrownItemMovement::update_x(void) {
  MovementWithCollision::update_x();
}

/**
 * Updates the y position.
 */
void ThrownItemMovement::update_y(void) {
  MovementWithCollision::update_y();

  if (y_increment < 7 && !is_stopped()) {
    Uint32 now = SDL_GetTicks();
    while (now >= next_down_date) {
      translate_y(y_increment);
      next_down_date += 40;
      item_height -= y_increment;
      y_increment++;
    }
  }
  else {
    stop();
  }
}

/**
 * Suspends or resumes the movement.
 * This function is called by the entity when the game is suspended or resumed.
 * @param suspended true to suspend the movement, false to resume it
 */
void ThrownItemMovement::set_suspended(bool suspended) {
  Movement::set_suspended(suspended);

  if (!suspended) {
    // recalculate next_down_date
    if (when_suspended != 0) {
      Uint32 now = SDL_GetTicks();
      next_down_date = now + (next_down_date - when_suspended);
    }
  }
}

/**
 * Returns the current height of the entity.
 * @return the entity's height
 */
int ThrownItemMovement::get_item_height(void) {
  return item_height;
}
