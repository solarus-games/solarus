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
#ifndef SOLARUS_HERO_VICTORY_STATE_H
#define SOLARUS_HERO_VICTORY_STATE_H

#include "solarus/core/Common.h"
#include "solarus/hero/HeroState.h"
#include <cstdint>

namespace Solarus {

/**
 * \brief The victory state of the hero.
 */
class Hero::VictoryState: public HeroState {

  public:

    VictoryState(Hero& hero, const ScopedLuaRef& callback_ref);

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;

  private:

    uint32_t end_victory_date;         /**< Date when the victory animation stops. */
    bool finished;                     /**< Indicates that the victory sequence is finished. */
    ScopedLuaRef callback_ref;         /**< Lua ref of a function to call when the sequence finishes. */

};

}

#endif

