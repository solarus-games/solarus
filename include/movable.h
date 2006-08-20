#ifndef ZSDX_MOVABLE_H
#define ZSDX_MOVABLE_H

#include "map_object.h"

class Movable: public MapObject {
  
 protected:
  // speed vector
  int x_speed; // 0: stopped, 100: max
  int y_speed;

  SDL_TimerID x_timer_id;
  SDL_TimerID y_timer_id;

 public:
  Movable(void);
  virtual ~Movable(void) { }

  static void init(void);

  // position
  inline void set_x(int x) { where_in_map.x = x; }
  inline void set_y(int y) { where_in_map.y = y; }
  inline void set_position(int x, int y) { where_in_map.x = y; where_in_map.y = y; }

  // moving
  inline int get_x_speed() { return x_speed; }
  inline int get_y_speed() { return y_speed; }

  int get_speed(void);

  void set_x_speed(int x_speed);
  void set_y_speed(int y_speed);

  void set_direction(int direction);
  void set_speed(int speed);

  inline void stop(void) { set_x_speed(0); set_y_speed(0); }
};

#endif
