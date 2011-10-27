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
#ifndef SOLARUS_HERO_HOOKSHOT_STATE_H
#define SOLARUS_HERO_HOOKSHOT_STATE_H

#include "hero/State.h"

/**
 * @brief The state "hookshot" of the hero.
 */
class Hero::HookshotState: public Hero::State {

  private:

    Hookshot *hookshot;    /**< the hookshot thrown by the hero */

  public:

    HookshotState(Hero &hero);
    ~HookshotState();

    void start(State *previous_state);
    void stop(State *next_state);

    bool is_touching_ground();
    bool can_avoid_deep_water();
    bool can_avoid_hole();
    bool can_avoid_lava();
    bool can_avoid_prickle();
    bool can_avoid_teletransporter();
    bool can_avoid_conveyor_belt();
    bool is_stairs_obstacle(Stairs& stairs);
    bool is_sensor_obstacle(Sensor& sensor);
    bool is_jumper_obstacle(Jumper& jumper);
    bool can_avoid_switch();
    bool can_be_hurt(Enemy* attacker);
    void notify_obstacle_reached();
};

#endif

