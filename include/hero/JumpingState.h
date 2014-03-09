/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_HERO_JUMPING_STATE_H
#define SOLARUS_HERO_JUMPING_STATE_H

#include "hero/State.h"

namespace solarus {

/**
 * \brief The state "Jumping" of the hero.
 */
class Hero::JumpingState: public Hero::State {

  public:

    JumpingState(
        Hero& hero,
        int direction8,
        int distance,
        bool ignore_obstacles,
        bool with_sound,
        uint32_t movement_delay);
    ~JumpingState();

    void start(const State* previous_state);
    void stop(const State* next_state);
    void set_map(Map& map);
    void update();
    void set_suspended(bool suspended);
    void notify_layer_changed();
    int get_wanted_movement_direction8() const;
    bool can_start_gameover_sequence() const;
    bool is_touching_ground() const;
    bool is_stairs_obstacle(const Stairs& stairs) const;
    bool is_sensor_obstacle(const Sensor& sensor) const;
    bool is_separator_obstacle(const Separator& separator) const;
    bool can_avoid_deep_water() const;
    bool can_avoid_hole() const;
    bool can_avoid_ice() const;
    bool can_avoid_lava() const;
    bool can_avoid_prickle() const;
    bool can_avoid_teletransporter() const;
    bool can_avoid_stream() const;
    bool can_avoid_sensor() const;
    bool can_avoid_switch() const;
    bool can_be_hurt(MapEntity* attacker) const;
    CarriedItem* get_carried_item() const;
    CarriedItem::Behavior get_previous_carried_item_behavior() const;

  private:

    void destroy_carried_item();

    JumpMovement* movement;       /**< the movement applied to the hero */
    int direction8;               /**< direction of the jump (0 to 7) */
    bool with_sound;              /**< indicates that a jump sound is played */
    CarriedItem* carried_item;    /**< an item carried by the hero while making this jump, or NULL */

};

}

#endif

