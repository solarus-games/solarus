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
#ifndef SOLARUS_SELECTION_PHASE_H
#define SOLARUS_SELECTION_PHASE_H

#include "Common.h"
#include "Screen.h"

/**
 * Abstract class for a phase of the selection menu,
 * i.e. the menu to select the savegame file.
 */
class SelectionMenuPhase {

  protected:

    SelectionMenu *menu;    /**< the selection menu this phase belongs to */

  protected:

    // creation and destruction
    SelectionMenuPhase(SelectionMenu *menu, const std::string &title_string_key);

  public:

    virtual ~SelectionMenuPhase(void);

    // update and display
    virtual void update(void);
    virtual void display(Surface *destination_surface);
    virtual void handle_event(const SDL_Event &event);

};

#endif

