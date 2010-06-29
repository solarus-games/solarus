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
#include "menus/SelectionMenuEraseFile.h"
#include "menus/SelectionMenuSelectFile.h"
#include "menus/SelectionMenuConfirmErase.h"
#include "menus/SelectionMenu.h"
#include "Sprite.h"
#include "Savegame.h"
#include "Solarus.h"

/**
 * @brief Creates a selection menu phase where the player chooses a file to erase.
 * @param menu the selection menu this phase will belong to
 */
SelectionMenuEraseFile::SelectionMenuEraseFile(SelectionMenu *menu):
  SelectionMenuPhase(menu, "selection_menu.phase.erase_file") {

  menu->set_bottom_options("selection_menu.cancel", "");
  menu->get_cursor_sprite()->set_current_animation("red");
}

/**
 * @brief Destructor.
 */
SelectionMenuEraseFile::~SelectionMenuEraseFile(void) {

}

/**
 * @brief Handles an event in this phase.
 * @param event the event
 */
void SelectionMenuEraseFile::notify_event(InputEvent &event) {

  if (event.is_keyboard_key_pressed(validation_keys) || event.is_joypad_button_pressed()) {
    
    int cursor_position = menu->get_cursor_position();

    if (cursor_position == 5) {
      // the user chose "Quit"
      menu->set_exiting();
    }
    else if (cursor_position == 4) {
      // the user chose "Cancel"
      menu->play_ok_sound();
      menu->set_next_phase(new SelectionMenuSelectFile(menu));
    }
    else {

      int save_number_to_erase = cursor_position - 1;
      if (menu->get_savegame(save_number_to_erase)->is_empty()) {
	// the savegame doesn't exist: error sound
	menu->play_error_sound();
      }
      else {
	// the savegame exists: confirm deleting it
	menu->play_ok_sound();
	menu->set_next_phase(new SelectionMenuConfirmErase(menu, save_number_to_erase));
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

      default:
	break;
    }
  }
}

/**
 * @brief Displays this selection menu phase.
 * @param destination_surface the surface to draw
 */
void SelectionMenuEraseFile::display(Surface *destination_surface) {

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

