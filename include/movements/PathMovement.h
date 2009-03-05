#ifndef ZSDX_PATH_MOVEMENT_H
#define ZSDX_PATH_MOVEMENT_H

#include "Common.h"
#include "movements/MovementWithCollision.h"

/**
 * Movement for an entity that follows a path.
 * The path is a succession of basic moves, where
 * each basic move is an 8-pixel movement in one
 * of the 8 main directions.
 */
class PathMovement: public MovementWithCollision {

 private:

  const string path;          /**< the path: each character is a direction ('0' to '7') */
  int current_direction;      /**< direction of the current move (0 to 7) */
  unsigned int next_index;    /**< index of the next move in the path */
  int distance_covered_x;     /**< counts the number of x pixels covered during the current move */
  int distance_covered_y;     /**< counts the number of y pixels covered during the current move */

  const bool loop;            /**< should the movement return to the beginning once finished?  */

  bool finished;              /**< indicates that the path is finished
			       * (only possible if loop is false) */

 public:

  PathMovement(Map *map, string path, int speed, bool loop);

  ~PathMovement(void);

  bool is_finished(void);

  int get_current_direction(void);
  void start_next_move(void);
  virtual void set_x(int x);
  virtual void set_y(int y);

};

#endif
