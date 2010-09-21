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
#ifndef SOLARUS_PLAYER_MOVEMENT_H
#define SOLARUS_PLAYER_MOVEMENT_H

#include "Common.h"
#include "movements/SmoothCollisionMovement.h"

/**
 * @brief Movement of an entity controlled by the player.
 *
 * This movement is an 8-direction movement typically used by the hero.
 * This class basically converts the combination of directional keys pressed
 * (from the class GameControls) into a movement.
 */
class PlayerMovement: public SmoothCollisionMovement {

  protected:

    int moving_speed;					/**< speed of the entity when it is moving */
    int direction8;					/**< current direction of the movement (0 to 7), as defined by the directional keys
							 * currently pressed by the player (when the movement allows them) or -1*/

    void set_wanted_direction();

  public:

    // creation and destruction
    PlayerMovement(int speed);
    ~PlayerMovement();

    void update();

    int get_moving_speed();
    void set_moving_speed(int moving_speed);
    int get_wanted_direction8();
    void compute_movement();
};

#endif

