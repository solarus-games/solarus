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
#ifndef SOLARUS_HERO_STATE_GRABBING_H
#define SOLARUS_HERO_STATE_GRABBING_H

#include "hero/State.h"

/**
 * @brief The state "Grabbing" of the hero.
 */
class Hero::StateGrabbing: public Hero::State {

  public:

    StateGrabbing(Hero *hero);
    ~StateGrabbing(void);
};

#endif

