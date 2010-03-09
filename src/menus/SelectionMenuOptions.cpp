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
#include "menus/SelectionMenuOptions.h"
#include "menus/SelectionMenuSelectFile.h"

/**
 * Creates a selection menu phase where the player sets the global options.
 * @param menu the selection menu this phase will belong to
 */
SelectionMenuOptions::SelectionMenuOptions(SelectionMenu *menu):
  SelectionMenuPhase(menu, "selection_menu.phase.options") {

}


/**
 * Destructor.
 */
SelectionMenuOptions::~SelectionMenuOptions(void) {

}

/**
 * Handles an input event in this phase.
 * @param event the event
 */
void SelectionMenuOptions::notify_event(InputEvent &event) {

}

/**
 * Displays this selection menu phase.
 * @param destination_surface the surface to draw
 */
void SelectionMenuOptions::display(Surface *destination_surface) {

}

