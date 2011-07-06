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
#ifndef SOLARUS_HERO_FORCED_WALKING_STATE_H
#define SOLARUS_HERO_FORCED_WALKING_STATE_H

#include "hero/State.h"

/**
 * @brief State of the hero where he is walking with a predetermined path.
 *
 * The player does not control the hero in this state.
 */
class Hero::ForcedWalkingState: public Hero::State {

  private:

    PathMovement *movement;       /**< the movement applied to the hero */

  public:

    ForcedWalkingState(Hero &hero, const std::string &path, bool loop, bool ignore_obstacles);
    ~ForcedWalkingState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    bool can_start_gameover_sequence();
    bool is_touching_ground();
    bool can_avoid_deep_water();
    bool can_avoid_hole();
    bool can_avoid_lava();
    bool can_avoid_prickle();
    bool can_avoid_teletransporter();
    bool can_avoid_conveyor_belt();
    bool can_avoid_sensor();
    bool can_avoid_switch();
    bool can_be_hurt();
};

#endif

