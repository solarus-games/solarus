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
#ifndef SOLARUS_HERO_SWORD_SWINGING_STATE_H
#define SOLARUS_HERO_SWORD_SWINGING_STATE_H

#include "hero/State.h"

/**
 * @brief The state "sword swinging" of the hero.
 */
class Hero::SwordSwingingState: public Hero::State {

  private:

    bool attacked;               /* indicates that at least one enemy was attacked */
    bool sword_finished;         /* indicates that the sword animation is finished */

  public:

    SwordSwingingState(Hero& hero);
    ~SwordSwingingState();

    void start(State* previous_state);
    void stop(State* next_state);
    void update();
    bool can_start_sword();
    bool can_be_hurt(Enemy* attacker);
    bool can_sword_hit_crystal();
    bool is_cutting_with_sword(Detector& detector);
    bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    void notify_obstacle_reached();
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
        EnemyReaction::Reaction& result, bool killed);
};

#endif

