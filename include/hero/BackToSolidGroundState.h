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
#ifndef SOLARUS_HERO_BACK_TO_SOLID_GROUND_STATE_H
#define SOLARUS_HERO_BACK_TO_SOLID_GROUND_STATE_H

#include "hero/State.h"

/**
 * @brief The state "back to solid ground" of the hero.
 */
class Hero::BackToSolidGroundState: public Hero::State {

  private:

    Rectangle target_xy;            /**< coordinates of the solid ground location to go to*/
    Layer target_layer;             /**< layer of the target location */
    uint32_t end_delay;             /**< delay before returning control to the player */
    uint32_t end_date;              /**< date when the state ends */
    bool with_sound;                /**< true to play a sound when reaching the solid ground */

  public:

    BackToSolidGroundState(Hero &hero, bool use_memorized_xy,
        uint32_t end_delay = 0, bool with_sound = true);
    ~BackToSolidGroundState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    void set_suspended(bool suspended);
    bool can_start_gameover_sequence();
    bool is_hero_visible();
    bool are_collisions_ignored();
    bool can_avoid_deep_water();
    bool can_avoid_hole();
    bool can_avoid_lava();
    bool can_avoid_prickle();
    bool is_touching_ground();
    bool can_avoid_conveyor_belt();
    bool can_avoid_sensor();
    bool can_avoid_switch();
    bool can_avoid_explosion();

};

#endif

