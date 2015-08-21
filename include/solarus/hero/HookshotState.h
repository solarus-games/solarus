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
#ifndef SOLARUS_HERO_HOOKSHOT_STATE_H
#define SOLARUS_HERO_HOOKSHOT_STATE_H

#include "solarus/hero/State.h"
#include <memory>

namespace Solarus {

class Hookshot;

/**
 * \brief The state "hookshot" of the hero.
 */
class Hero::HookshotState: public Hero::State {

  public:

    HookshotState(Hero& hero);

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;

    virtual bool is_touching_ground() const override;
    virtual bool can_avoid_deep_water() const override;
    virtual bool can_avoid_hole() const override;
    virtual bool can_avoid_ice() const override;
    virtual bool can_avoid_lava() const override;
    virtual bool can_avoid_prickle() const override;
    virtual bool can_avoid_teletransporter() const override;
    virtual bool can_avoid_stream(const Stream& stream) const override;
    virtual bool is_stairs_obstacle(const Stairs& stairs) const override;
    virtual bool is_sensor_obstacle(const Sensor& sensor) const override;
    virtual bool is_jumper_obstacle(const Jumper& jumper, const Rectangle& candidate_position) const override;
    virtual bool can_avoid_switch() const override;
    virtual bool can_be_hurt(MapEntity* attacker) const override;
    virtual bool can_pick_treasure(EquipmentItem& item) const override;
    virtual void notify_obstacle_reached() override;

  private:

    void finish_movement();

    std::shared_ptr<Hookshot> hookshot;     /**< the hookshot thrown by the hero */
};

}

#endif

