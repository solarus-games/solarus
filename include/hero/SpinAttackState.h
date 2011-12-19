/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_HERO_SPIN_ATTACK_STATE_H
#define SOLARUS_HERO_SPIN_ATTACK_STATE_H

#include "hero/State.h"

/**
 * @brief The state "spin attack" of the hero.
 */
class Hero::SpinAttackState: public Hero::State {

  private:

    bool being_pushed;           /**< indicates that the hero is being pushed after hitting an enemy */
    void play_spin_attack_sound();

  public:

    SpinAttackState(Hero& hero);
    ~SpinAttackState();

    void start(State* previous_state);
    void stop(State* next_state);
    void update();
    bool can_sword_hit_crystal();
    bool can_be_hurt(Enemy* attacker);
    bool is_cutting_with_sword(Detector& detector);
    int get_sword_damage_factor();
    bool is_deep_water_obstacle();
    bool is_hole_obstacle();
    bool is_lava_obstacle();
    bool is_prickle_obstacle();
    bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    void notify_obstacle_reached();
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
        EnemyReaction::Reaction& result, bool killed);
};

#endif

