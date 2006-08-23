#ifndef ZSDX_MOVABLE_H
#define ZSDX_MOVABLE_H

#include <SDL/SDL.h>
#include "map_object.h"

/* Abstract class for every moving object on the map.
 * An instance of Movable has a speed vector that modifies
 * its coordinates. This class provides functions to get or
 * set the position, the speed and the direction of the entity.
 */
class Movable: virtual public MapObject {

 private:
  /* Update the x position of the entity if necessary
   */
  void update_x(void);

  /* Update the y position of the entity if necessary
   */
  void update_y(void);

 protected:
  // speed vector

  /* X speed
   * 0: stopped
   * positive value: moving to the right
   * negative value: moving to the left
   * The maximum value is 100 or -100.
   */
  int x_speed;

  /* Y speed
   * 0: stopped
   * positive value: moving downwards
   * negative value: moving upwards
   * The maximum value is 100 or -100.
   */
  int y_speed;

  /* Date of the next x move in ticks
   */
  Uint32 next_move_date_x;

  /* Date of the next y move in ticks
   */
  Uint32 next_move_date_y;

  // these fields can be computed with x_speed and y_speed

  /* Delay in ticks between an x move of 2 pixels
   * x_delay = 200 / |x_speed|
   */
  Uint32 x_delay;

  /* Delay in ticks between an y move of 2 pixels
   * y_delay = 200 / |y_speed|
   */
  Uint32 y_delay;

  /* Number of pixels of an x move : 0, 2 or -2
   * Yes, the smallest move is two pixels.
   * A sprite doesn't need to make a move of only one pixel. 
   */
  int x_move;

  /* Number of pixels of an y move : 0, 2 or -2
   */
  int y_move;

  /* Update the x and y position of the sprite if necessary
   */
  inline void update_position(void) { update_x(); update_y(); }

 public:
  /* Constructor
   */
  Movable(void);

  /* Destructor
   */
  virtual ~Movable(void) { }

  // position

  /* Return the x position of the entity
   * This is a redefinition of MapObject::get_x() because we may need
   * to update the x position is enough time has passed.
   */
  int get_x(void);

  /* Return the y position of the entity
   * This is a redefinition of MapObject::get_y() because we may need
   * to update the y position is enough time has passed.
   */
  int get_y(void);

  /* Set the x position of the entity
   */
  inline void set_x(int x) { where_in_map.x = x; }

  /* Set the y position of the entity
   */
  inline void set_y(int y) { where_in_map.y = y; }

  /* Set the position of the entity
   */
  inline void set_position(int x, int y) { where_in_map.x = y; where_in_map.y = y; }

  // moving

  /* Return the x speed of the entity
   */
  inline int get_x_speed() { return x_speed; }

  /* Return the y speed of the entity
   */
  inline int get_y_speed() { return y_speed; }

  /* Return the total speed of the entity
   * speed = sqrt(x_speed*x_speed + y_speed*y_speed)
   */
  int get_speed(void);

  /* Set the x speed
   */
  void set_x_speed(int x_speed);
  
  /* Set the y speed
   */
  void set_y_speed(int y_speed);

  /* Change the direction, keeping the same speed
   * The given direction should be a number between 0 and 359.
   * x_speed and y_speed are recalculated but the total speed is unchanged.
   * If x_speed and y_speed are both equal to zero, this function has no effect.
   */
  void set_direction(int direction);

  /* Change the speed, keeping the same direction
   * x_speed and y_speed are recalculated but the movement direction is
   * unchanged.
   * Warning: if x_speed and y_speed are both equal to zero, there is no
   * direction so you're getting into trouble.
   */
  void set_speed(int speed);

  /* Set the speed to zero.
   */
  inline void stop(void) { set_x_speed(0); set_y_speed(0); }
};

#endif
