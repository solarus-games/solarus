/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_HERO_FREE_STATE_H
#define SOLARUS_HERO_FREE_STATE_H

#include "hero/State.h"
#include "hero/PlayerMovementState.h"

/**
 * @brief State of the hero where he can walk normally and interact with entities.
 */
class Hero::FreeState: public Hero::PlayerMovementState {

  private:

    int pushing_direction4;			/**< direction where the hero is trying to push an obstacle (0 to 3) or -1*/ 
    uint32_t start_pushing_date;		/**< date when the state pushing starts */

  public:

    FreeState(Hero &hero);
    ~FreeState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    void set_suspended(bool suspended);
    void action_key_pressed();
    void notify_movement_tried(bool success);

    bool is_free();
    bool can_start_sword();
    bool can_start_inventory_item();
    bool can_take_stairs();
    bool can_take_jump_sensor();
    CarriedItem::Behavior get_previous_carried_item_behavior(CarriedItem& carried_item);
    void set_animation_stopped();
    void set_animation_walking();
};

#endif

