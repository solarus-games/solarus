/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/hero/PlayerMovementState.h"
#include <cstdint>

namespace Solarus {

/**
 * \brief The state "sword loading" of the hero.
 */
class Hero::SwordLoadingState: public Hero::PlayerMovementState {

  public:

    SwordLoadingState(Hero& hero);

    virtual void start(const State* previous_state) override;
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;
    virtual void notify_obstacle_reached() override;
    virtual void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed
    ) override;

    virtual bool is_direction_locked() const override;
    virtual bool can_take_stairs() const override;
    virtual bool can_pick_treasure(EquipmentItem& item) const override;
    virtual bool can_use_shield() const override;
    virtual void set_animation_stopped() override;
    virtual void set_animation_walking() override;

  private:

    void play_load_sound();

    uint32_t sword_loaded_date;            /**< date when the sword is loaded */
    bool sword_loaded;                     /**< becomes true when the spin attack is possible */

};

}

#endif

