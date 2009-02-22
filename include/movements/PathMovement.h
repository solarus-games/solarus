#ifndef ZSDX_PATH_MOVEMENT_H
#define ZSDX_PATH_MOVEMENT_H

#include "Common.h"
#include "movements/MovementWithCollision.h"

/**
 * Movement for an entity that follows a predetermined path.
 * A path is a succession of translation vectors.
 */
class PathMovement: public MovementWithCollision {

 private:

  Map *map; /**< the map */

  // movement properties

  /**
   * The succession of translations.
   * Each element of the array represents a move
   * in pixels (only the fields x and y of the SDL_Rect are used).
   */
  const SDL_Rect *translation_vectors;

  /**
   * Number of translation vectors: this is the size of the
   * translation_vectors array.
   */
  const int nb_vectors;

  /**
   * Delay in milliseconds between two translations.
   */
  const Uint32 delay;

  /**
   * Should the movement return to the beginning once finished?
   */
  const bool loop;

  /**
   * Indicates whether this movement is sensitive to the obstacles.
   */
  bool with_collisions;

  // current state

  /**
   * Current translation vector in the array.
   */
  int vector_index;

  /**
   * Indicates whether the entity has reached the end of the path
   * (only when loop is false, i.e. the path does have an end).
   */
  bool path_ended;

 protected:

  PathMovement(Map *map, int nb_vectors, Uint32 delay, bool loop, bool with_collisions);
  void set_translation_vectors(const SDL_Rect *translation_vectors);
  virtual void make_next_move(void);
  int get_vector_index(void);
  int get_length(void);

 public:

  PathMovement(Map *map, const SDL_Rect *translation_vectors,
	       int nb_vectors, Uint32 delay, bool loop, bool with_collisions);
  virtual ~PathMovement(void);

  virtual void update(void);
  bool is_finished(void);

};

#endif
