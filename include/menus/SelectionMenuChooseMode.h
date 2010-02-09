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
#ifndef SOLARUS_SELECTION_MENU_CHOOSE_MODE_H
#define SOLARUS_SELECTION_MENU_CHOOSE_MODE_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"

/**
 * Phase of the selection menu where the user chooses the game mode.
 * Note that currently, this phase is automatically closed because there is only one game mode for now.
 * This class may allow to create an alternative game mode later.
 */
class SelectionMenuChooseMode: public SelectionMenuPhase {

  private:

    bool adventure_mode;         /**< the currently selected mode */
    Surface *mode_img;           /**< image representing the mode choice */
    Surface *savegame_surface;   /**< an intermediate surface used to draw the selected savegame */

  public:

    // creation and destruction
    SelectionMenuChooseMode(SelectionMenu *menu);
    ~SelectionMenuChooseMode(void);

    // update and display
    void notify_event(InputEvent &event);
    void update(void);
    void display(Surface *screen_surface);
};

#endif

