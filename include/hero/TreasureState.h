/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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

#include "hero/State.h"
#include "Treasure.h"

/**
 * @brief The state "Treasure" of the hero.
 */
class Hero::TreasureState: public Hero::State {

  private:

    Treasure treasure;       /**< the treasure to give to the hero */
    int callback_ref;        /**< Lua ref of a function to call when the
                              * treasure's dialog finishes. */

  public:

    TreasureState(Hero& hero, const Treasure& treasure, int callback_ref);
    ~TreasureState();

    void start(State* previous_state);
    void stop(State* next_state);
    void update();
    void draw_on_map();
    CarriedItem::Behavior get_previous_carried_item_behavior(CarriedItem& carried_item);
};

#endif

