#ifndef ZSDX_MOVABLE_H
#define ZSDX_MOVABLE_H

#include <SDL/SDL.h>
#include "map_object.h"

class Movable: public MapObject {

 private:
  void update_x(void);
  void update_y(void);

 protected:
  // speed vector
  int x_speed; // 0: stopped, 100 or -100 : max
  int y_speed;

  Uint32 next_move_date_x; // next move date in ticks
  Uint32 next_move_date_y;

  // these fields can be computed with x_speed and y_speed
  Uint32 x_delay; // delay (in ticks) between an x move of 2 pixels
  Uint32 y_delay;
  int x_move; // x move in pixels: 0, 2 or -2
  int y_move;

  inline void update_position(void) { update_x(); update_y(); }

 public:
  Movable(void);
  virtual ~Movable(void) { }

  // position
  int get_x(void);
  int get_y(void);
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
