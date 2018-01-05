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
#ifndef SOLARUS_HERO_BOOMERANG_STATE_H
#define SOLARUS_HERO_BOOMERANG_STATE_H

#include "solarus/hero/HeroState.h"
#include <string>

namespace Solarus {

/**
 * \brief The state "boomerang" of the hero.
 */
class Hero::BoomerangState: public HeroState {

  public:

    BoomerangState(Hero& hero, int max_distance, int speed,
        const std::string& hero_preparing_animation,
        const std::string& sprite_name);

    virtual void start(const State* previous_state) override;
    virtual void update() override;

    virtual bool can_avoid_stream(const Stream& stream) const override;

  private:

    int direction_pressed8;                      /**< direction pressed by the player */
    int max_distance;                            /**< maximum distance to traverse in pixel */
    int speed;                                   /**< speed of the movement in pixels per second */
    const std::string tunic_preparing_animation; /**< animation name of the hero's tunic
                                                  * when preparing the boomerang */
    const std::string sprite_name;               /**< animation set id that represents the boomerang */

};

}

#endif

