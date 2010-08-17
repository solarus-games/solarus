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
#ifndef SOLARUS_HERO_CARRYING_STATE_H
#define SOLARUS_HERO_CARRYING_STATE_H

#include "hero/PlayerMovementState.h"

/**
 * @brief The state "Carrying" of the hero.
 */
class Hero::CarryingState: public Hero::PlayerMovementState {

  private:

    CarriedItem *carried_item;		/**< the item to carry */

    void throw_item(void);

  public:

    CarryingState(Hero *hero, CarriedItem *carried_item);
    ~CarryingState(void);

    void start(State *previous_state);
    void stop(State *next_state);
    void set_map(Map *map);
    void set_suspended(bool suspended);
    void update(void);
    void action_key_pressed(void);
    bool can_start_sword(void);
    bool can_take_jump_sensor(void);
    void set_animation_stopped(void);
    void set_animation_walking(void);

};

#endif

