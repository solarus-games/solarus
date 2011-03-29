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
#ifndef SOLARUS_HERO_SWORD_LOADING_STATE_H
#define SOLARUS_HERO_SWORD_LOADING_STATE_H

#include "hero/PlayerMovementState.h"

/**
 * @brief The state "sword loading" of the hero.
 */
class Hero::SwordLoadingState: public Hero::PlayerMovementState {

  private:

    uint32_t sword_loaded_date;            /**< date when the sword is loaded */
    bool sword_loaded;                     /**< becomes true when the spin attack is possible */

    void play_load_sound();

  public:

    SwordLoadingState(Hero &hero);
    ~SwordLoadingState();

    void start(State *previous_state);
    void update();
    void set_suspended(bool suspended);
    void notify_movement_tried(bool success);
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim, EnemyReaction::Reaction& result, bool killed);

    bool is_direction_locked();
    bool can_take_jump_sensor();
    bool can_take_stairs();
    void set_animation_stopped();
    void set_animation_walking();
};

#endif

