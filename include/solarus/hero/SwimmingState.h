/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_HERO_SWIMMING_STATE_H
#define SOLARUS_HERO_SWIMMING_STATE_H

#include "solarus/hero/PlayerMovementState.h"

namespace Solarus {

/**
 * \brief The state "Swimming" of the hero.
 */
class Hero::SwimmingState: public Hero::PlayerMovementState {

  public:

    SwimmingState(Hero& hero);

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;

    virtual void set_animation_stopped() override;
    virtual void set_animation_walking() override;

    virtual void notify_action_command_pressed() override;
    virtual void notify_attack_command_pressed() override;

    virtual bool is_stairs_obstacle(const Stairs& stairs) const override;
    virtual bool can_pick_treasure(EquipmentItem& item) const override;

  private:

    int get_slow_swimming_speed() const;
    int get_fast_swimming_speed() const;
    void try_swim_faster();

    bool fast_swimming;            /**< indicates that the hero is currently swimming faster */
    uint32_t end_fast_swim_date;   /**< date when the fast swimming stops */

};

}

#endif

