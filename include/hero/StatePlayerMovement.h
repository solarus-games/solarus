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
#ifndef SOLARUS_HERO_STATE_PLAYER_MOVEMENT_H
#define SOLARUS_HERO_STATE_PLAYER_MOVEMENT_H

#include "Common.h"
#include "hero/State.h"

/**
 * Abstract class for all hero states such that the movement is controlled by the player.
 * This means that the hero can move in the eight directions and the movement
 * is an instance of PlayerMovement.
 * These states include StateFree (the normal walk), StateSwordLoading (the sword is
 * loading), StateCarrying (the hero is carrying something) and StateSwimming.
 */
class Hero::StatePlayerMovement: public Hero::State {

  protected:

    StatePlayerMovement(Hero *hero);

  public:

    virtual ~StatePlayerMovement(void);
    virtual void start(State *previous_state);
    virtual void stop(State *next_state);

    PlayerMovement *get_player_movement(void);
};

#endif

