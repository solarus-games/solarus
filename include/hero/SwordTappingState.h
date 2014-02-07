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
#ifndef SOLARUS_HERO_SWORD_TAPPING_STATE_H
#define SOLARUS_HERO_SWORD_TAPPING_STATE_H

#include "hero/State.h"

namespace solarus {

/**
 * \brief The state "sword tapping" of the hero.
 */
class Hero::SwordTappingState: public Hero::State {

  public:

    SwordTappingState(Hero& hero);
    ~SwordTappingState();

    void start(const State* previous_state);
    void stop(const State* next_state);
    void update();
    void set_suspended(bool suspended);
    bool can_sword_hit_crystal() const;
    bool can_pick_treasure(EquipmentItem& item) const;
    bool can_use_shield() const;
    bool is_cutting_with_sword(Detector& detector);
    bool is_teletransporter_obstacle(const Teletransporter& teletransporter) const;
    void notify_obstacle_reached();
    void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed);

  private:

    uint32_t next_sound_date;     /**< date when the sword tapping sound should be played */

};

}

#endif

