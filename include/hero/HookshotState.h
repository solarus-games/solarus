/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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

namespace solarus {

/**
 * \brief The state "hookshot" of the hero.
 */
class Hero::HookshotState: public Hero::State {

  public:

    HookshotState(Hero& hero);
    ~HookshotState();

    void start(const State* previous_state);
    void stop(const State* next_state);

    bool is_touching_ground() const;
    bool can_avoid_deep_water() const;
    bool can_avoid_hole() const;
    bool can_avoid_ice() const;
    bool can_avoid_lava() const;
    bool can_avoid_prickle() const;
    bool can_avoid_teletransporter() const;
    bool can_avoid_conveyor_belt() const;
    bool is_stairs_obstacle(const Stairs& stairs) const;
    bool is_sensor_obstacle(const Sensor& sensor) const;
    bool is_jumper_obstacle(const Jumper& jumper) const;
    bool can_avoid_switch() const;
    bool can_be_hurt(Enemy* attacker) const;
    bool can_pick_treasure(EquipmentItem& item) const;
    void notify_obstacle_reached();

  private:

    void finish_movement();

    Hookshot* hookshot;    /**< the hookshot thrown by the hero */
};

}

#endif

