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
#ifndef SOLARUS_HERO_FORCED_WALKING_STATE_H
#define SOLARUS_HERO_FORCED_WALKING_STATE_H

#include "solarus/hero/State.h"
#include <memory>
#include <string>

namespace Solarus {

class PathMovement;

/**
 * \brief State of the hero where he is walking with a predetermined path.
 *
 * The player does not control the hero in this state.
 */
class Hero::ForcedWalkingState: public HeroState {

  public:

    ForcedWalkingState(
        Hero& hero,
        const std::string& path,
        bool loop,
        bool ignore_obstacles
    );

    virtual void start(const HeroState* previous_state) override;
    virtual void stop(const HeroState* next_state) override;
    virtual void update() override;
    virtual bool can_start_gameover_sequence() const override;
    virtual bool is_touching_ground() const override;
    virtual bool can_avoid_deep_water() const override;
    virtual bool can_avoid_hole() const override;
    virtual bool can_avoid_ice() const override;
    virtual bool can_avoid_lava() const override;
    virtual bool can_avoid_prickle() const override;
    virtual bool can_avoid_teletransporter() const override;
    virtual bool can_avoid_stream(const Stream& stream) const override;
    virtual bool can_avoid_sensor() const override;
    virtual bool can_avoid_switch() const override;
    virtual bool can_be_hurt(MapEntity* attacker) const override;

  private:

    std::shared_ptr<PathMovement> movement;      /**< the movement applied to the hero */

};

}

#endif

