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
#ifndef SOLARUS_TIMER_H
#define SOLARUS_TIMER_H

#include "Common.h"

/**
 * @brief Represents a timer that any class can start.
 *
 * Timers are mostly used by the scripts.
 */
class Timer {

  private:

    // timer
    const std::string name;          /**< name of the timer (permits to identify it) */

    uint32_t expiration_date;        /**< date when the timer is finished */
    bool finished;                   /**< indicates that the timer is finished */

    bool suspended;                  /**< indicates whether the timer is suspended */
    uint32_t when_suspended;         /**< date when the timer was suspended */

    // sound
    uint32_t next_sound_date;        /**< date when the next countdown sound effect is played */
    Sound *countdown_sound;          /**< sound played every second while the timer is running */
    Sound *countdown_hurry_sound;    /**< sound played when the timer is close to finish */

  public:

    Timer(Game *game, uint32_t duration, const std::string &name, bool with_sound);
    ~Timer();

    const std::string& get_name();
    bool is_finished();

    void update();
    void set_suspended(bool suspended);
};

#endif

