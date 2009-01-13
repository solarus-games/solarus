#ifndef ZSDX_JUMP_SENSOR_H
#define ZSDX_JUMP_SENSOR_H

#include "Common.h"
#include "Detector.h"

/**
 * A jump sensor is a detector placed on a map.
 * When Link touches this detetor, he makes a jump with a specified length.
 */
class JumpSensor: public Detector {

 private:

  int jump_length; /**< length of the jump in pixels (should be a multiple of 8) */

 public:

  JumpSensor(string name, Layer layer, int x, int y, int width, int height,
	     int direction, int jump_length);
  ~JumpSensor(void);

  bool check_collision_custom(MapEntity *entity);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);

};

#endif
