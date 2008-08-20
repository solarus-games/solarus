#ifndef ZSDX_FOLLOW_MOVEMENT_H
#define ZSDX_FOLLOW_MOVEMENT_H

#include "Common.h"
#include "Movement.h"

/**
 * Movement for an entity attached to another entity.
 * At each moment, the entity is placed at a position
 * relative to the entity followed.
 */
class FollowMovement: public Movement {

 private:

  MapEntity *entity_followed;
  int x;
  int y;

 protected:

  void update_x(void);

 public:

  FollowMovement(MapEntity *entity_followed, int x, int y);
  ~FollowMovement(void);

};

#endif
