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
#ifndef SOLARUS_SCRIPTS_H
#define SOLARUS_SCRIPTS_H

#include "Common.h"
#include <list>

/**
 * @brief Represents a list of scripts currently running.
 *
 * When an event is triggered (i.e. calling Lua from C++),
 * you can use this class to notify all current scripts.
 */
class Scripts {

  private:

    std::list<Script*> scripts;			/**< the scripts currently running */

  public:

    Scripts();
    ~Scripts();

    void add_script(Script *script);
    void remove_script(Script *script);

    void send_event();
};

#endif

