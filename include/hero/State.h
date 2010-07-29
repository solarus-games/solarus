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
#ifndef ZSDX_HERO_STATE_H
#define ZSDX_HERO_STATE_H

#include "Common.h"
#include "entities/Hero.h"

/**
 * @brief Abstract base class for a state of the hero.
 */
class Hero::State {

  protected:

    Hero *hero;				/**< the hero this state is managing */

    State(Hero *hero);

  public:

    virtual ~State(void);

    virtual void start(void);
    virtual void stop(void);

    virtual void update(void);
    virtual void display_on_map(void);
    virtual void set_suspended(bool suspended);
};

#endif

