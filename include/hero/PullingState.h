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
#ifndef SOLARUS_HERO_PULLING_STATE_H
#define SOLARUS_HERO_PULLING_STATE_H

#include "hero/State.h"

/**
 * @brief The state "Pulling" of the hero.
 */
class Hero::PullingState: public Hero::State {

  private:

    Detector *pulled_entity;			/**< the entity the hero is pulling (or NULL) */

    void stop_moving_pulled_entity();

  public:

    PullingState(Hero &hero);
    ~PullingState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    void notify_grabbed_entity_collision();
    bool is_grabbing_or_pulling();
    bool is_moving_grabbed_entity();
    bool is_deep_water_obstacle();
    bool is_hole_obstacle();
    bool is_lava_obstacle();
    bool is_prickle_obstacle();

    bool can_be_hurt(Enemy* attacker);
};

#endif

