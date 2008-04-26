/**
 * This module defines the class Moving.
 */

#ifndef ZSDX_MOVING_H
#define ZSDX_MOVING_H

#include "Common.h"
#include "MapEntity.h"

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
   * Updates the x position of the entity if it wants to move on x (i.e. x_move != 0).
   */
  virtual void update_x(void);

  /**
   * Updates the y position of the entity if it wants to move on y (i.e. y_move != 0).
   */
  virtual void update_y(void);

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

  /**
   * Date of the last x or y move.
   */
  Uint32 last_move_date;

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
   * Number of pixels of the next x move : 0, 2 or -2.
   * The smallest move is two pixels because a sprite
   * doesn't need to make a move of only one pixel. 
   */
  int x_move;

  /**
   * Number of pixels of the next y move : 0, 2 or -2.
   */
  int y_move;

  /**
   * Moves the entity on x.
   * @param dx number of pixels of the move
   */
  void translate_x(int dx);

  /**
   * Moves the entity on y.
   * @param dy number of pixels of the move
   */
  void translate_y(int dy);

  /**
   * Moves the entity.
   * @param dx number of pixels of the move on x
   * @param dy number of pixels of the move on y
   */
  inline void translate(int dx, int dy) { translate_x(dx); translate_y(dy); }

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
   * Sets the x position of the entity.
   * @param x the new x position
   */
  virtual void set_x(int x);

  /**
   * Sets the y position of the entity.
   * @param y the new y position
   */
  virtual void set_y(int y);

  /**
   * Sets the position of the entity.
   * @param x the new x position
   * @param y the new y position
   */
  inline void set_position(int x, int y) { set_x(x); set_y(y); }

  /**
   * Updates the position (x and y) of the entity if it has changed.
   */
  virtual void update(void);
  bool has_to_move_now(void);

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

  int get_speed(void);
  void set_x_speed(int x_speed);
  void set_y_speed(int y_speed);

  void set_direction(int direction);
  void set_speed(int speed);
  void stop(void);
};

#endif
