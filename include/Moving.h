/**
 * This module defines the class Moving.
 */

#ifndef ZSDX_MOVING_H
#define ZSDX_MOVING_H

#include <SDL/SDL.h>
#include "map_entity.h"

/**
 * Mathematic constants.
 * We don't use the constants from math.h
 * because they are not ANSI.
 */
extern const double PI;
extern const double TWO_PI;
extern const double PI_OVER_2;
extern const double THREE_PI_OVER_2;
extern const double SQRT_2;

/**
 * Abstract class for every moving object on the map, visible or not.
 * An instance of Moving has a speed vector that modifies
 * its coordinates. This class provides functions to get or
 * set the position, the speed and the direction of the entity.
 */
class Moving: public MapEntity {

 private:

  /**
   * Updates the x position of the entity if it has changed.
   */
  void update_x(void);

  /**
   * Updates the y position of the entity if it has changed.
   */
  void update_y(void);

 protected:

  // speed vector

  /**
   * X speed of the entity, between -100 and 100.
   * 0: stopped
   * positive value: moving to the right
   * negative value: moving to the left
   */
  int x_speed;

  /**
   * Y speed of the entity, between -100 and 100.
   * 0: stopped
   * positive value: moving downwards
   * negative value: moving upwards
   */
  int y_speed;

  /**
   * Date of the next x move in ticks.
   */
  Uint32 next_move_date_x;

  /**
   * Date of the next y move in ticks.
   */
  Uint32 next_move_date_y;

  // the following fields are redundant and can be computed
  // with x_speed and y_speed

  /**
   * Delay in ticks between an x move of 2 pixels.
   * x_delay = 200 / |x_speed|
   */
  Uint32 x_delay;

  /**
   * Delay in ticks between an y move of 2 pixels.
   * y_delay = 200 / |y_speed|
   */
  Uint32 y_delay;

  /**
   * Number of pixels of an x move : 0, 2 or -2.
   * The smallest move is two pixels because a sprite
   * doesn't need to make a move of only one pixel. 
   */
  int x_move;

  /**
   * Number of pixels of an y move : 0, 2 or -2.
   */
  int y_move;

 public:

  /**
   * Constructor.
   */
  Moving(void);

  /**
   * Destructor.
   */
  virtual ~Moving(void) { }

  // position

  /**
   * Updates the position (x and y) of the entity if it has changed.
   */
  virtual void update_position(void);

  /**
   * Sets the x position of the entity.
   * @param x the new x position
   */
  inline void set_x(int x) { position_in_map.x = x; }

  /**
   * Sets the y position of the entity.
   * @param y the new y position
   */
  inline void set_y(int y) { position_in_map.y = y; }

  /**
   * Sets the position of the entity.
   * @param x the new x position
   * @param y the new y position
   */
  inline void set_position(int x, int y) { position_in_map.x = x; position_in_map.y = y; }

  // movement

  /**
   * Returns the x speed of the entity.
   * @return the x speed of the entity, between -100 and 100
   */
  inline int get_x_speed() { return x_speed; }

  /**
   * Returns the y speed of the entity.
   * @return the y speed of the entity, between -100 and 100
   */
  inline int get_y_speed() { return y_speed; }

  /**
   * Returns the total speed of the entity:
   * sqrt(x_speed^2 + y_speed^2)
   */
  int get_speed(void);

  /**
   * Sets the x speed.
   * @param x_speed the x speed of the entity, between -100 and 100
   */
  void set_x_speed(int x_speed);
  
  /**
   * Sets the y speed.
   * @param y_speed the y speed of the entity, between -100 and 100
   */
  void set_y_speed(int y_speed);

  /**
   * Changes the direction of the movement, keeping the same speed.
   * x_speed and y_speed are recomputed so that the total speed is unchanged.
   * Warning: if x_speed and y_speed are both equal to zero, this function has no effect.
   * @param direction the new movement direction, between 0 and 359
   */
  void set_direction(int direction);

  /**
   * Changes the speed, keeping the same direction of the movement.
   * x_speed and y_speed are recomputed so that the movement direction is unchanged.
   * Warning: if x_speed and y_speed are both equal to zero, there is no
   * direction so you're getting into trouble.
   * @param speed the new speed
   */
  void set_speed(int speed);

  /**
   * Sets the speed to zero.
   */
  inline void stop(void) { set_x_speed(0); set_y_speed(0); }
};

#endif
