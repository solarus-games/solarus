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
#ifndef SOLARUS_HERO_STATE_FREE_H
#define SOLARUS_HERO_STATE_FREE_H

#include "hero/State.h"
#include "hero/StatePlayerMovement.h"

/**
 * @brief State of the hero where he can walk normally and interact with entities.
 */
class Hero::StateFree: public Hero::StatePlayerMovement {

  private:

    int pushing_direction4;			/**< direction where the hero is trying to push an obstacle (0 to 3) or -1*/ 
    uint32_t start_pushing_date;		/**< date when the state pushing starts */

  public:

    StateFree(Hero *hero);
    ~StateFree(void);

    void start(State *previous_state);
    void update(void);
    void set_suspended(bool suspended);
    void action_key_pressed(void);
    void notify_movement_tried(bool success);

    bool is_free(void);
    bool can_start_sword(void);
    void set_animation_stopped(void);
    void set_animation_walking(void);
};

#endif

