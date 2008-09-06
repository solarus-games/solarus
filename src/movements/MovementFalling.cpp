#include "movements/MovementFalling.h"

/**
 * Y move at each time frame.
 */
static const SDL_Rect moves[3][26] = {

  // small
  {{0, -1},  {0, 1}},

  // medium
  {{0, -1},  {0, -1},  {0, -1},  {0, -1},  {0,  1},  {0,  1},  {0,  1}, 
   {0,  1},  {0, -1},  {0, -1},  {0,  1},  {0,  1}},

  // big
  {{0, -4},  {0, -3},  {0, -3},  {0, -2},  {0, -2},  {0, -1},  {0,  1}, 
   {0,  2},  {0,  2},  {0,  3},  {0,  3},  {0,  4},  {0, -2},  {0, -1}, 
   {0, -1},  {0, -1},  {0, -1},  {0,  1},  {0,  1},  {0,  1},  {0,  1}, 
   {0,  2},  {0, -1},  {0, -1},  {0,  1},  {0,  1}},
};

/**
 * Number of moves for each height.
 */
static const int nb_moves[3] = {2, 12, 26};

/**
 * Creates a movement falling object.
 * @param height height the entity will fall from
 */
MovementFalling::MovementFalling(MovementFalling::FallingHeight height):
  MovementPath(moves[height], nb_moves[height], 30, false) {

}

/**
 * Destructor.
 */
MovementFalling::~MovementFalling(void) {

}
