#ifndef ZSDX_RANDOM_WALK_MOVEMENT_H
#define ZSDX_RANDOM_WALK_MOVEMENT_H

#include "Common.h"
#include "movements/PathMovement.h"

/**
 * Movement for an entity that walks randomly.
 */
class RandomWalkMovement: public PathMovement {

 private:

  static const std::string random_directions[];

 public:

  RandomWalkMovement(Map *map, int speed);
  ~RandomWalkMovement(void);

  void update(void);
  void start_next_move(void);
  bool is_finished(void);

 protected:

  static std::string get_random_direction(void);

};

#endif
