#include "movements/JumpMovement.h"

/**
 * Basic movement vector of each direction.
 */
const SDL_Rect JumpMovement::basic_translations[8] = {
  { 1,  0}, // right
  { 1, -1},
  { 0, -1},
  {-1, -1},
  {-1,  0},
  {-1,  1},
  { 0,  1},
  { 1,  1}
};

/**
 * Creates a jump movement.
 * @param direction of the movement (0 to 7)
 * @param length length of the jump
 */
JumpMovement::JumpMovement(int direction, int length):
  PathMovement(length, 100, false) {

  // compute the path
  translation_vectors = new SDL_Rect[length];
  for (int i = 0; i < length; i++) {
    translation_vectors[i] = basic_translations[direction];
  }
  set_translation_vectors(translation_vectors);
}

/**
 * Destructor.
 */
JumpMovement::~JumpMovement(void) {
  delete[] translation_vectors;
}
