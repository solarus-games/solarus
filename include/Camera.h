#ifndef ZSDX_CAMERA_H
#define ZSDX_CAMERA_H

#include "Common.h"

/**
 * The camera determines the visible area of the map.
 */
class Camera {

 private:

  Map *map;                 /**< the map */
  SDL_Rect position;        /**< position of the camera on the map */
  TargetMovement *movement; /**< movement of the camera, or NULL for no movement */

 public:

  Camera(Map *map);
  ~Camera(void);

  void update(void);
  SDL_Rect *get_position(void);

  bool is_moving(void);
  void set_movement(int target_x, int target_y, int speed);
};

#endif
