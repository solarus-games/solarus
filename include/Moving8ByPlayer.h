/**
 * This module defines the class Moving8ByPlayer.
 */

#ifndef ZSDX_MOVING_8_BY_PLAYER
#define ZSDX_MOVING_8_BY_PLAYER

#include <SDL/SDL.h>
#include "moving_with_collision.h"

/**
 * Abstract class for a moving entity with the 8 basic directions.
 * This class is designed for an entity controlled by the keyboard
 * but could be also used to control the entity with a joypad.
 */
class Moving8ByPlayer: public MovingWithCollision {

 protected:

  /**
   * True if the entity is moving.
   */
  bool started;

  /**
   * Which ones of the 4 keyboard arrows are pressed by the player.
   */
  Uint16 direction_mask;

  /**
   * Updates the movement of the entity.
   * This function is called when an arrow is pressed or released
   * on the keyboard.
   */
  virtual void update_movement(void);

  /**
   * Speed of the entity when it is moving.
   */
  const int speed;

 public:

  /**
   * Constructor.
   * @param speed movement speed
   */
  Moving8ByPlayer(int speed);

  /**
   * Destructor.
   */
  virtual ~Moving8ByPlayer(void) { }

  /**
   * Returns the current direction of the movement.
   * The returned direction is an angle (0 to 359), or -1 if the
   * movement is stopped.
   */
  int get_direction(void);

  /**
   * Function called when the user presses the right arrow
   */
  void start_right(void);

  /**
   * Function called when the user presses the up arrow
   */
  void start_up(void);

  /**
   * Function called when the user presses the left arrow
   */
  void start_left(void);

  /**
   * Function called when the user presses the down arrow
   */
  void start_down(void);

  /**
   * Function called when the user releases the right arrow
   */
  void stop_right(void);

  /**
   * Function called when the user releases the up arrow
   */
  void stop_up(void);

  /**
   * Function called when the user releases the left arrow
   */
  void stop_left(void);

  /**
   * Function called when the user releases the down arrow
   */
  void stop_down(void);
};

#endif
