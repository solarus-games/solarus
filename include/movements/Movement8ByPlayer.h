#ifndef ZSDX_MOVEMENT_8_BY_PLAYER
#define ZSDX_MOVEMENT_8_BY_PLAYER

#include "Common.h"
#include "MovementWithSmoothCollision.h"

/**
 * Abstract class for a moving entity with the 8 basic directions
 * and controlled by the player.
 * This class is designed for an entity controlled by the keyboard.
 */
class Movement8ByPlayer: public MovementWithSmoothCollision {

 protected:

  // movement

  /**
   * True if the entity is moving.
   */
  bool started;

  /**
   * Speed of the entity when it is moving.
   */
  const int speed;

  /**
   * True if the player can move the entity, i.e. if the game is not interrupted
   * and the entity is in a state such that the player has the control.
   */
  bool can_move;

  // keyboard

  /**
   * Indicates which ones of the 4 keyboard arrows are currently pressed by the player.
   * This bit field is an OR combination of right_mask, up_mask, left_mask and down_mask.
   */
  Uint16 direction_mask;

  static const Uint16 direction_masks[4];

  void add_direction_mask(Uint16 direction_mask);
  void remove_direction_mask(Uint16 direction_mask);
  void set_direction_mask(Uint16 direction_mask);

 public:

  // creation and destruction
  Movement8ByPlayer(int speed);
  ~Movement8ByPlayer(void);

  // direction
  int get_direction(void);
  Uint16 get_direction_mask(void);

  void add_direction(int direction);
  void remove_direction(int direction);

  // movement
  void set_moving_enabled(bool can_move);
  bool is_started(void);
  bool is_moving_enabled(void);
  void set_suspended(bool suspended);

  void compute_movement(void);
};

#endif
