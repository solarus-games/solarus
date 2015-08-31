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
#ifndef SOLARUS_HERO_BASE_STATE_H
#define SOLARUS_HERO_BASE_STATE_H

#include "solarus/entities/EntityState.h"

namespace Solarus {

/**
 * \brief The hero base state.
 */
class Hero::BaseState: public Entity::State {

  public:

    virtual void start(const State* previous_state) override;

    virtual Hero& get_entity() override;
    virtual const Hero& get_entity() const override;

    void notify_attack_command_pressed();
    virtual void notify_item_command_pressed(int slot) override;

    bool is_jumper_obstacle(const Jumper& jumper, const Rectangle& candidate_position) const;

  protected:

    BaseState(Hero& hero, const std::string& state_name);

};

}

#endif

