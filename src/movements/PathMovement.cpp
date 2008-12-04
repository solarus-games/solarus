#include "movements/PathMovement.h"

/**
 * Creates a movement path object, not specifying the path for now.
 * @param nb_vectors number of translation vectors in the array
 * @param delay delay in milliseconds between two translations
 * @param loop true to make the movement return to the beginning
 * once finished, false to stop it
 */
PathMovement::PathMovement(int nb_vectors, Uint32 delay, bool loop):
  nb_vectors(nb_vectors), delay(delay), loop(loop),
  vector_index(0), path_ended(false) {

}

/**
 * Creates a movement path object.
 * @param translation_vectors the succession of translations
 * composing this movement (each element of the array represents
 * a translation vector in pixels; only the fields x and y of the
 * SDL_Rect are used.
 * @param nb_vectors number of translation vectors in the array
 * @param delay delay in milliseconds between two translations
 * @param loop true to make the movement return to the beginning
 * once finished, false to stop it
 */
PathMovement::PathMovement(const SDL_Rect *translation_vectors,
			   int nb_vectors, Uint32 delay, bool loop):
  translation_vectors(translation_vectors), nb_vectors(nb_vectors),
  delay(delay), loop(loop), vector_index(0), path_ended(false) {

}

/**
 * Destructor.
 */
PathMovement::~PathMovement(void) {

}

/**
 * Sets the translation vectors of the path.
 * @param translation vectors the path
 */
void PathMovement::set_translation_vectors(const SDL_Rect *translation_vectors) {
  this->translation_vectors = translation_vectors;
}

/**
 * Updates the position.
 */
void PathMovement::update_x(void) {

  Uint32 now = SDL_GetTicks();

  while (now >= next_move_date_x && !path_ended) {
    translate(translation_vectors[vector_index].x, translation_vectors[vector_index].y);
    next_move_date_x += delay;

    vector_index++;
    if (vector_index >= nb_vectors) {
      if (loop) {
	vector_index = 0;
      }
      else {
	path_ended = true;
      }
    }
  }
}

/**
 * Returns whether the movement is finished, i.e.
 * whether the end of the path was reached.
 */
bool PathMovement::is_finished(void) {
  return path_ended;
}
