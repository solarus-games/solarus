/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_PLAYER_MOVEMENT
#define SOLARUS_PLAYER_MOVEMENT

#include "Common.h"
#include "movements/SmoothCollisionMovement.h"

/**
 * @brief Movement of an entity controlled by the player.
 *
 * This movement is an 8-direction movement typically used by the hero.
 */
class PlayerMovement: public SmoothCollisionMovement {

 protected:

  // movement

  bool started;                                    /**< true if the entity is moving */
  int moving_speed;                                /**< speed of the entity when it is moving */
  bool moving_enabled;                             /**< true if the player can move the entity, i.e. if the game is not interrupted
						    * and the entity is in a state such that the player has the control */
  bool moving_enabled_before_suspended;            /**< indicates whether moving_enabled was true when the game was suspended */
  bool direction_enabled;                          /**< true if the direction arrows pressed are taken into account */
  bool direction_enabled_before_suspended;         /**< indicates whether direction_enabled was true when the game was suspended */

  // keyboard

  uint16_t direction_mask;                         /**< indicates which ones of the 4 directional keys are currently pressed by the player
						    * (an OR combination of the four direction masks) */

  static const uint16_t direction_masks[4];        /**< bit mask representing of each one of the 4 directions */

  void add_direction_mask(uint16_t direction_mask);
  void remove_direction_mask(uint16_t direction_mask);
  void set_direction_mask(uint16_t direction_mask);

  void add_direction(int direction);
  void remove_direction(int direction);

 public:

  // creation and destruction
  PlayerMovement(int speed);
  ~PlayerMovement(void);

  // direction
  int get_direction(void);
  uint16_t get_direction_mask(void);

  void directional_key_pressed(int direction4);
  void directional_key_released(int direction4);

  // movement
  bool is_moving_enabled(void);
  void set_moving_enabled(bool moving_enabled, bool direction_enabled);
  int get_moving_speed(void);
  void set_moving_speed(int moving_speed);
  bool is_direction_enabled(void);

  bool is_started(void);
  void set_suspended(bool suspended);

  void compute_movement(void);
};

#endif

