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
#ifndef SOLARUS_HERO_CARRYING_STATE_H
#define SOLARUS_HERO_CARRYING_STATE_H

#include "hero/PlayerMovementState.h"

namespace Solarus {

/**
 * \brief The state "Carrying" of the hero.
 */
class Hero::CarryingState: public Hero::PlayerMovementState {

  public:

    CarryingState(Hero& hero, const std::shared_ptr<CarriedItem>& carried_item);

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;
    virtual void set_map(Map& map) override;
    virtual void notify_layer_changed() override;
    virtual void set_suspended(bool suspended) override;
    virtual void update() override;
    virtual void notify_action_command_pressed() override;
    virtual bool can_start_sword() const override;
    virtual bool can_use_shield() const override;
    virtual bool can_take_stairs() const override;
    virtual void set_animation_stopped() override;
    virtual void set_animation_walking() override;
    virtual std::shared_ptr<CarriedItem> get_carried_item() const override;
    virtual CarriedItem::Behavior get_previous_carried_item_behavior() const override;

  private:

    void throw_item();

    std::shared_ptr<CarriedItem> carried_item;         /**< the item to carry */

};

}

#endif

