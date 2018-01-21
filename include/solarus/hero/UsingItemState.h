/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_HERO_INVENTORY_ITEM_STATE_H
#define SOLARUS_HERO_INVENTORY_ITEM_STATE_H

#include "solarus/core/Common.h"
#include "solarus/core/EquipmentItemUsage.h"
#include "solarus/hero/HeroState.h"

namespace Solarus {

/**
 * \brief The state "using equipment item" of the hero.
 */
class Hero::UsingItemState: public HeroState {

  public:

    UsingItemState(Hero& hero, EquipmentItem& item);

    virtual void start(const State* previous_state) override;
    virtual void update() override;

    virtual bool is_using_item() const override;
    virtual EquipmentItemUsage& get_item_being_used() override;
    virtual bool can_avoid_stream(const Stream& stream) const override;

  private:

    EquipmentItemUsage item_usage;     /**< Info about using this equipment item. */

};

}

#endif

