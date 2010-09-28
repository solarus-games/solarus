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
#ifndef SOLARUS_SELECTION_MENU_SELECT_FILE_H
#define SOLARUS_SELECTION_MENU_SELECT_FILE_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"

/**
 * @brief Phase of the selection menu where the user can select a file.
 *
 * This is the first phase of the selection menu.
 */
class SelectionMenuSelectFile: public SelectionMenuPhase {

  public:

    // creation and destruction
    SelectionMenuSelectFile(SelectionMenu *selection_menu, int initial_cursor_position);
    ~SelectionMenuSelectFile();

    // update and display
    void display(Surface *destination_surface);
    void notify_event(InputEvent &event);
};

#endif

