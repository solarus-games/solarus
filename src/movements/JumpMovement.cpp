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
 * @param map the map (can be NULL if with_collisions is false)
 * @param direction of the movement (0 to 7)
 * @param length length of the jump
 * @param with_collisions true to make the movement sensitive to obstacles
 */
JumpMovement::JumpMovement(Map *map, int direction, int length, bool with_collisions):
  PixelMovement(map, length, 5, false, with_collisions) {

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

/**
 * Returns the height in pixels where the entity should be displayed over its shadow.
 * @return the height of the jump
 */
int JumpMovement::get_jump_height(void) {
  return jump_height;
}

/**
 * Makes a move in the path.
 */
void JumpMovement::make_next_move(void) {
  PixelMovement::make_next_move();

  int i = get_vector_index();
  if (i == 1 || i == get_length() - 1) {
    jump_height = 0;
  }
  else if (i % 2 == 0) {
    jump_height += (i < get_length() / 2) ? 1 : -1;
  }
}
