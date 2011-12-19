/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "menus/SelectionMenuSelectFile.h"
#include "menus/SelectionMenuEraseFile.h"
#include "menus/SelectionMenuChooseName.h"
#include "menus/SelectionMenuChooseMode.h"
#include "menus/SelectionMenuOptions.h"
#include "Transition.h"
#include "Savegame.h"
#include "Sprite.h"
#include "Solarus.h"

/**
 * @brief Creates a selection menu phase where the player selects a file.
 * @param menu the selection menu this phase will belong to
 * @param initial_cursor_position position of the cursor to set (1 to 5)
 */
SelectionMenuSelectFile::SelectionMenuSelectFile(SelectionMenu *menu, int initial_cursor_position):
  SelectionMenuPhase(menu, "selection_menu.phase.select_file") {

  menu->set_bottom_options("selection_menu.erase", "selection_menu.options");

  // initialize the cursor
  menu->get_cursor_sprite()->set_current_animation("blue");
  menu->set_cursor_position(initial_cursor_position);
}


/**
 * @brief Destructor.
 */
SelectionMenuSelectFile::~SelectionMenuSelectFile() {

}

/**
 * @brief Handles an input event in this phase.
 * @param event the event
 */
void SelectionMenuSelectFile::notify_event(InputEvent &event) {

  if (!menu->has_transition()) {
    
    if (event.is_keyboard_key_pressed(validation_keys) || event.is_joypad_button_pressed()) {

      menu->play_ok_sound();
      int cursor_position = menu->get_cursor_position();
    
      if (cursor_position == 5) {
	// the user chose "Options"
	menu->set_next_phase(new SelectionMenuOptions(menu));
      }
      else if (cursor_position == 4) {
	// the user chose "Erase"
	menu->set_next_phase(new SelectionMenuEraseFile(menu));
      }
      else {
	// the user chose a save

	Savegame **savegames = menu->get_savegames();
	if (savegames[cursor_position - 1]->is_empty()) {
	  // the savegame doesn't exist: ask the name
	  menu->set_next_phase(new SelectionMenuChooseName(menu));
	}
	else {
	  // the savegame exists: choose the mode and then start the game
	  menu->set_next_phase(new SelectionMenuChooseMode(menu));
	}
      }
    }

    if (event.is_direction_pressed()) {

      switch (event.get_direction()) {

	case 6: // down
	  menu->move_cursor_down();
	  break;

	case 2: // up
	  menu->move_cursor_up();
	  break;

	case 0: // right
	case 4: // left
	  menu->move_cursor_left_or_right();
	  break;

	default:
	  break;
      }
    }
  }
}

/**
 * @brief Displays this selection menu phase.
 * @param destination_surface the surface to draw
 */
void SelectionMenuSelectFile::display(Surface *destination_surface) {

  // savegames
  for (int i = 0; i < 3; i++) {
    menu->display_savegame(destination_surface, i);
  }

  // options
  menu->display_bottom_options(destination_surface);

  // cursor
  menu->display_savegame_cursor(destination_surface);

  // save numbers
  for (int i = 0; i < 3; i++) {
    menu->display_savegame_number(destination_surface, i);
  }
}

