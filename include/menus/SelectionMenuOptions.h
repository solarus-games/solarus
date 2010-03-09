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
#ifndef SOLARUS_SELECTION_MENU_OPTIONS_H
#define SOLARUS_SELECTION_MENU_OPTIONS_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"

/**
 * A phase of the selection menu where the user change set global options
 * such as the language, the video mode and the sound volume.
 * Those options are global to all savegames even though the video mode
 * can still be changed during the game.
 * They are saved in the file config.ini.
 */
class SelectionMenuOptions: public SelectionMenuPhase {

  public:

    // creation and destruction
    SelectionMenuOptions(SelectionMenu *selection_menu);
    ~SelectionMenuOptions(void);

    // update and display
    void display(Surface *destination_surface);
    void notify_event(InputEvent &event);
};

#endif

