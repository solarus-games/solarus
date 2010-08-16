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
 * These states include FreeState (the normal walk), SwordLoadingState (the sword is
 * loading), CarryingState (the hero is carrying something) and SwimmingState.
 */
class Hero::PlayerMovementState: public Hero::State {

  protected:

    PlayerMovementState(Hero *hero);

  public:

    virtual ~PlayerMovementState(void);
    virtual void start(State *previous_state);
    virtual void stop(State *next_state);
    void set_map(Map *map);
    PlayerMovement *get_player_movement(void);

    virtual void set_animation_stopped(void);
    virtual void set_animation_walking(void);
    void directional_key_pressed(int direction4);
    void directional_key_released(int direction4);
    bool can_control_movement(void);
    int get_wanted_movement_direction8(void);
    void notify_walking_speed_changed(void);
    void notify_movement_changed(void);
    void set_stop_on_obstacles(bool stop_on_obstacles);
    bool can_be_hurt(void);
};

#endif

