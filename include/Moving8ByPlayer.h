/**
 * This module defines the class Moving8ByPlayer.
 */

#ifndef ZSDX_MOVING_8_BY_PLAYER
#define ZSDX_MOVING_8_BY_PLAYER

#include "Common.h"
#include "MovingWithSmoothCollision.h"

/**
 * Abstract class for a moving entity with the 8 basic directions.
 * This class is designed for an entity controlled by the keyboard
 * but could be also used to control the entity with a joypad.
 */
class Moving8ByPlayer: public MovingWithSmoothCollision {

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
   * True if the player can move the entity.
   */
  bool can_move;

  /**
   * Speed of the entity when it is moving.
   */
  const int speed;

  virtual void update_movement(void);
  void add_direction_mask(Uint16 direction_mask);
  void remove_direction_mask(Uint16 direction_mask);
  void set_direction_mask(Uint16 direction_mask);
  virtual void direction_mask_just_changed(void);

 public:

  Moving8ByPlayer(int speed);
  virtual ~Moving8ByPlayer(void) { }

  int get_direction(void);

  void set_moving_enabled(bool can_move);

  void start_right(void);
  void start_up(void);
  void start_left(void);
  void start_down(void);
  void stop_right(void);
  void stop_up(void);
  void stop_left(void);
  void stop_down(void);

  virtual void update(void);

};

#endif
