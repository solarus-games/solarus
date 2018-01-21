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
#ifndef SOLARUS_HERO_TREASURE_STATE_H
#define SOLARUS_HERO_TREASURE_STATE_H

#include "solarus/core/Common.h"
#include "solarus/core/Treasure.h"
#include "solarus/hero/HeroState.h"
#include "solarus/lua/ScopedLuaRef.h"

namespace Solarus {

/**
 * \brief The state "Treasure" of the hero.
 */
class Hero::TreasureState: public HeroState {

  public:

    TreasureState(
        Hero& hero,
        const Treasure& treasure,
        const ScopedLuaRef& callback_ref
    );

    void start(const State* previous_state) override;
    void stop(const State* next_state) override;
    void update() override;
    void draw_on_map() override;
    CarriedObject::Behavior get_previous_carried_object_behavior() const override;
    bool is_brandishing_treasure() const override;

  private:

    Treasure treasure;            /**< The treasure to give to the hero. */
    SpritePtr treasure_sprite;    /**< Sprite of the treasure. */
    ScopedLuaRef callback_ref;    /**< Lua ref of a function to call when the
                                   * treasure's dialog finishes. */

};

}

#endif

