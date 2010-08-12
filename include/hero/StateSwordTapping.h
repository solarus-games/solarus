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
#ifndef SOLARUS_HERO_STATE_SWORD_TAPPING_H
#define SOLARUS_HERO_STATE_SWORD_TAPPING_H

#include "hero/State.h"

/**
 * @brief The state "sword tapping" of the hero.
 */
class Hero::StateSwordTapping: public Hero::State {

  private:

    uint32_t next_sound_date;		/**< date when the sword tapping sound should be played */

  public:

    StateSwordTapping(Hero *hero);
    ~StateSwordTapping(void);

    void start(State *previous_state);
    void update(void);
    void set_suspended(bool suspended);
};

#endif

