/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/hero/HeroState.h"
#include <cstdint>

namespace Solarus {

/**
 * \brief The state "back to solid ground" of the hero.
 */
class Hero::BackToSolidGroundState: public HeroState {

  public:

    BackToSolidGroundState(
        Hero& hero,
        bool use_specified_position,
        uint32_t end_delay = 0,
        bool with_sound = true
    );

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;

    virtual bool can_start_gameover_sequence() const override;
    virtual bool is_visible() const override;
    virtual bool are_collisions_ignored() const override;
    virtual bool can_avoid_deep_water() const override;
    virtual bool can_avoid_hole() const override;
    virtual bool can_avoid_ice() const override;
    virtual bool can_avoid_lava() const override;
    virtual bool can_avoid_prickle() const override;
    virtual bool is_touching_ground() const override;
    virtual bool can_avoid_teletransporter() const override;
    virtual bool can_avoid_stream(const Stream& stream) const override;
    virtual bool can_avoid_sensor() const override;
    virtual bool can_avoid_switch() const override;
    virtual bool can_avoid_explosion() const override;

  private:

    ScopedLuaRef target_position;   /**< Where to go back. */
    uint32_t end_delay;             /**< delay before returning control to the player */
    uint32_t end_date;              /**< date when the state ends */
    bool with_sound;                /**< true to play a sound when reaching the solid ground */

};

}

#endif

