#include "MovementFalling.h"

/**
 * Y move at each time frame.
 */
const int moves[3][26] = {
  {-1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // small
  {-1, -1, -1, -1,  1,  1,  1,  1, -1, -1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // medium
  {-4, -3, -3, -2, -2, -1,  1,  2,  2,  3,  3,  4, -2, -1, -1, -1, -1,  1,  1,  1,  1,  2, -1, -1,  1,  1}, // big
};

/**
 * Creates a movement falling object.
 * @param height height the entity will fall from
 */
MovementFalling::MovementFalling(MovementFallingHeight height):
  height(height), frame(0) {

  this->next_move_date_y = SDL_GetTicks();
}

/**
 * Destructor.
 */
MovementFalling::~MovementFalling(void) {

}

#include "MapEntity.h" // TODO remove

/**
 * Updates the y position.
 */
void MovementFalling::update_y(void) {
  
  Uint16 now = SDL_GetTicks();
  
  while (now >= next_move_date_y && frame < 26) {
    translate_y(moves[height][frame]);
    frame++;
    next_move_date_y += 30;
  }
}
