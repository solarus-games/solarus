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

 protected:

  const string initial_path;  /**< the path: each character is a direction ('0' to '7') */
  string remaining_path;      /**< the remaining part of the path */
  const int initial_speed;    /**< the movement speed */

 private:

  int current_direction;      /**< direction of the current move (0 to 7) */
  int distance_covered_x;     /**< counts the number of x pixels covered during the current move */
  int distance_covered_y;     /**< counts the number of y pixels covered during the current move */

  const bool loop;            /**< should the movement return to the beginning once finished?  */
  const bool with_collisions; /**< indicates whether this movement is sensitive to the obstacles */

  bool finished;              /**< indicates that the path is finished
			       * (possible if loop is false or when there is a collision) */

 protected:

  bool collision_with_map(int dx, int dy);
  virtual void start_next_move(void);


 public:

  PathMovement(Map *map, string path, int speed, bool loop, bool with_collisions);
  ~PathMovement(void);

  virtual bool is_finished(void);

  int get_current_direction(void);
  void set_x(int x);
  void set_y(int y);
};

#endif
