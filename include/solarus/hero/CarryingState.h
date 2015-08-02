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
#ifndef SOLARUS_HERO_CARRYING_STATE_H
#define SOLARUS_HERO_CARRYING_STATE_H

#include "solarus/hero/PlayerMovementState.h"
#include <memory>

namespace Solarus {

/**
 * \brief The state "Carrying" of the hero.
 */
class Hero::CarryingState: public Hero::PlayerMovementState {

  public:

    CarryingState(Hero& hero, const std::shared_ptr<CarriedItem>& carried_item);

    void start(const State* previous_state) override;
    void stop(const State* next_state) override;
    void set_map(Map& map) override;
    void notify_position_changed() override;
    void notify_layer_changed() override;
    void set_suspended(bool suspended) override;
    void update() override;
    void notify_action_command_pressed() override;
    bool can_start_sword() const override;
    bool can_use_shield() const override;
    bool can_take_stairs() const override;
    void set_animation_stopped() override;
    void set_animation_walking() override;
    std::shared_ptr<CarriedItem> get_carried_item() const override;
    CarriedItem::Behavior get_previous_carried_item_behavior() const override;

  private:

    void throw_item();

    std::shared_ptr<CarriedItem> carried_item;         /**< the item to carry */

};

}

#endif

