#ifndef ZSDX_CAMERA_H
#define ZSDX_CAMERA_H

#include "Common.h"

/**
 * The camera determines the visible area of the map.
 */
class Camera {

 private:

  Map *map;                 /**< the map */
  bool fixed_on_hero;       /**< true if the camera is fixed on the hero */
  bool restoring;           /**< true if the camera is moving back to the hero */
  SDL_Rect position;        /**< position of the camera on the map */
  int speed;                /**< speed of the movement */
  TargetMovement *movement; /**< movement of the camera, or NULL for no movement */

 public:

  Camera(Map *map);
  ~Camera(void);

  void update(void);
  SDL_Rect *get_position(void);

  bool is_fixed_on_hero(void);
  void set_speed(int speed);
  void move(int target_x, int target_y);
  void move(MapEntity *entity);
  void restore(void);
};

#endif
