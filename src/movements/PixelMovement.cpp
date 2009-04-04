#include "movements/PixelMovement.h"

/**
 * Creates a pixel movement object, not specifying the trajectory for now.
 * @param map the map
 * @param nb_vectors number of translation vectors in the array
 * @param delay delay in milliseconds between two translations
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param with_collisions true to make the movement sensitive to obstacles
 */
PixelMovement::PixelMovement(Map *map, int nb_vectors, Uint32 delay,
			     bool loop, bool with_collisions):
  MovementWithCollision(map),
  nb_vectors(nb_vectors), delay(delay), loop(loop),
  with_collisions(with_collisions), vector_index(0), finished(false) {

}

/**
 * Creates a pixel movement object.
 * @param map the map (can be NULL if with_collisions is false)
 * @param translation_vectors the succession of translations
 * composing this movement (each element of the array represents
 * a translation vector in pixels; only the fields x and y of the
 * SDL_Rect are used.
 * @param nb_vectors number of translation vectors in the array
 * @param delay delay in milliseconds between two translations
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param with_collisions true to make the movement sensitive to obstacles
 */
PixelMovement::PixelMovement(Map *map, const SDL_Rect *translation_vectors,
			     int nb_vectors, Uint32 delay, bool loop, bool with_collisions):
  MovementWithCollision(map),
  translation_vectors(translation_vectors), nb_vectors(nb_vectors),
  delay(delay), loop(loop), with_collisions(with_collisions), vector_index(0), finished(false) {

}

/**
 * Destructor.
 */
PixelMovement::~PixelMovement(void) {

}

/**
 * Sets the translation vectors of the trajectory.
 * @param translation_vectors the trajectory
 */
void PixelMovement::set_translation_vectors(const SDL_Rect *translation_vectors) {
  this->translation_vectors = translation_vectors;
}

/**
 * Changes the delay between two moves.
 * @param delay the new delay
 */
void PixelMovement::set_delay(Uint32 delay) {
  this->delay = delay;
}

/**
 * Updates the position.
 */
void PixelMovement::update(void) {

  if (suspended) {
    return;
  }

  Uint32 now = SDL_GetTicks();

  while (now >= next_move_date_x && !finished) {
    make_next_move();
  }
}

/**
 * Makes a move in the path.
 */
void PixelMovement::make_next_move(void) {

  int dx = translation_vectors[vector_index].x;
  int dy = translation_vectors[vector_index].y;

  if (!with_collisions || !collision_with_map(dx, dy)) {
    translate(dx, dy);
  }

  next_move_date_x += delay;

  vector_index++;
  if (vector_index >= nb_vectors) {
    if (loop) {
      vector_index = 0;
    }
    else {
      finished = true;
    }
  }
}

/**
 * Returns the total number of moves in this trajectory.
 * @return the total number of moves in this trajectory
 */
int PixelMovement::get_length(void) {
  return nb_vectors;
}

/**
 * Returns the current iteration number.
 * @return the current iteration number of the movement
 */
int PixelMovement::get_vector_index(void) {
  return vector_index;
}

/**
 * Returns whether the movement is finished, i.e.
 * whether the end of the trajectory was reached.
 */
bool PixelMovement::is_finished(void) {
  return finished;
}
