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
#ifndef SOLARUS_HERO_CARRYING_STATE_H
#define SOLARUS_HERO_CARRYING_STATE_H

#include "hero/PlayerMovementState.h"

namespace solarus {

/**
 * \brief The state "Carrying" of the hero.
 */
class Hero::CarryingState: public Hero::PlayerMovementState {

  public:

    CarryingState(Hero& hero, CarriedItem* carried_item);
    ~CarryingState();

    void start(const State* previous_state);
    void stop(const State* next_state);
    void set_map(Map& map);
    void notify_layer_changed();
    void set_suspended(bool suspended);
    void update();
    void notify_action_command_pressed();
    bool can_start_sword() const;
    bool can_use_shield() const;
    bool can_take_stairs() const;
    void set_animation_stopped();
    void set_animation_walking();
    CarriedItem* get_carried_item() const;
    CarriedItem::Behavior get_previous_carried_item_behavior() const;

  private:

    CarriedItem* carried_item;		/**< the item to carry */

    void throw_item();
    void destroy_carried_item();

};

}

#endif

