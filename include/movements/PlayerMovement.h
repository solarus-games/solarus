#ifndef ZSDX_PLAYER_MOVEMENT
#define ZSDX_PLAYER_MOVEMENT

#include "Common.h"
#include "MovementWithSmoothCollision.h"

/**
 * Abstract class for an 8-direction movement controlled by the player.
 */
class PlayerMovement: public MovementWithSmoothCollision {

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
  bool moving_enabled;
  bool moving_enabled_before_suspended;

  /**
   * True if the direction arrows pressed are taken into account.
   */
  bool direction_enabled;
  bool direction_enabled_before_suspended;

  // keyboard

  /**
   * Indicates which ones of the 4 arrows are currently pressed by the player.
   * This bit field is an OR combination of right_mask, up_mask, left_mask and down_mask.
   */
  Uint16 direction_mask;

  static const Uint16 direction_masks[4];

  void add_direction_mask(Uint16 direction_mask);
  void remove_direction_mask(Uint16 direction_mask);
  void set_direction_mask(Uint16 direction_mask);

 public:

  // creation and destruction
  PlayerMovement(int speed);
  ~PlayerMovement(void);

  // direction
  int get_direction(void);
  Uint16 get_direction_mask(void);

  void add_direction(int direction);
  void remove_direction(int direction);

  // movement
  bool is_moving_enabled(void);
  void set_moving_enabled(bool moving_enabled, bool direction_enabled);

  bool is_direction_enabled(void);
  // TODO remove void set_direction_enabled(bool direction_enabled);

  bool is_started(void);
  void set_suspended(bool suspended);

  void compute_movement(void);
};

#endif
