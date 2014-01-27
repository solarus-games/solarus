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
#ifndef SOLARUS_HERO_SPIN_ATTACK_STATE_H
#define SOLARUS_HERO_SPIN_ATTACK_STATE_H

#include "hero/State.h"

namespace solarus {

/**
 * \brief The state "spin attack" of the hero.
 */
class Hero::SpinAttackState: public Hero::State {

  public:

    SpinAttackState(Hero& hero);
    ~SpinAttackState();

    void start(const State* previous_state);
    void stop(const State* next_state);
    void update();
    bool can_sword_hit_crystal() const;
    bool can_be_hurt(Enemy* attacker) const;
    bool can_pick_treasure(EquipmentItem& item) const;
    bool is_cutting_with_sword(Detector& detector) const;
    int get_sword_damage_factor() const;
    bool is_deep_water_obstacle() const;
    bool is_hole_obstacle() const;
    bool is_lava_obstacle() const;
    bool is_prickle_obstacle() const;
    bool is_teletransporter_obstacle(const Teletransporter& teletransporter) const;
    bool is_separator_obstacle(const Separator& separator) const;
    void notify_obstacle_reached();
    void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed);

  private:

    void play_spin_attack_sound();

    bool being_pushed;           /**< indicates that the hero is being pushed after hitting an enemy */

};

}

#endif

