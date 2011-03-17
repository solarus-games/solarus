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
#ifndef SOLARUS_HERO_RUNNING_STATE_H
#define SOLARUS_HERO_RUNNING_STATE_H

#include "hero/State.h"

/**
 * @brief The state "Running" of the hero.
 */
class Hero::RunningState: public Hero::State {

  private:

    int phase;				/**< current phase of the run */
    uint32_t next_phase_date;		/**< date of the next phase */
    uint32_t next_sound_date;		/**< date of the next time a sound is played during the run */

    bool is_bouncing();

  public:

    RunningState(Hero &hero);
    ~RunningState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    void set_suspended(bool suspended);
    bool is_pressing_running_key();
    void directional_key_pressed(int direction4);
    void notify_movement_tried(bool success);
    int get_wanted_movement_direction8();

    bool can_take_stairs();
    bool can_take_jump_sensor();
    bool can_be_hurt();
    bool can_start_gameover_sequence();
    bool is_touching_ground();
    bool can_avoid_deep_water();
    bool can_avoid_hole();
    bool can_avoid_lava();
    bool can_avoid_prickle();
    bool can_avoid_teletransporter();
    bool can_avoid_conveyor_belt();
    bool is_sensor_obstacle(Sensor &sensor);
    bool is_cutting_with_sword(Detector &detector);
    int get_sword_damage_factor();
};

#endif

