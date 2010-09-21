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
#include "menus/SelectionMenuConfirmErase.h"
#include "menus/SelectionMenuSelectFile.h"
#include "menus/SelectionMenu.h"
#include "Savegame.h"
#include "lowlevel/FileTools.h"

/**
 * @brief Creates a selection menu phase where the player has to confirm to erase a file.
 * @param menu the selection menu this phase will belong to
 * @param save_number_to_erase number of the savegame to delete (0 to 2)
 */
SelectionMenuConfirmErase::SelectionMenuConfirmErase(SelectionMenu *menu, int save_number_to_erase):
  SelectionMenuPhase(menu, "selection_menu.phase.confirm_erase"),
  save_number_to_erase(save_number_to_erase) {

  menu->set_bottom_options("selection_menu.big_no", "selection_menu.big_yes");
  menu->set_cursor_position(4); // select "no" by default
}

/**
 * @brief Destructor.
 */
SelectionMenuConfirmErase::~SelectionMenuConfirmErase() {

}

/**
 * @brief Handles an input event in this phase.
 * @param event the event
 */
void SelectionMenuConfirmErase::notify_event(InputEvent &event) {

  if (event.is_keyboard_key_pressed(validation_keys)) { // to erase a savegame file we don't allow joypad buttons

    int cursor_position = menu->get_cursor_position();

    if (cursor_position == 5) {
      // the user chose "Yes"
      menu->play_erase_sound();
      delete_save_file(save_number_to_erase);
      menu->set_cursor_position(save_number_to_erase + 1);
      menu->set_next_phase(new SelectionMenuSelectFile(menu));
    }
    else if (cursor_position == 4) {
      // the user chose "No"
      menu->play_ok_sound();
      menu->set_next_phase(new SelectionMenuSelectFile(menu));
    }
  }

  if (event.is_direction_pressed()
      && event.get_direction() % 4 == 0) { // right or left

    menu->move_cursor_left_or_right();
  }
}

/**
 * @brief Displays this selection menu phase.
 * @param destination_surface the surface to draw
 */
void SelectionMenuConfirmErase::display(Surface *destination_surface) {

  // savegame
  menu->display_savegame(destination_surface, save_number_to_erase);
  menu->display_savegame_number(destination_surface, save_number_to_erase);

  // options
  menu->display_bottom_options(destination_surface);

  // cursor
  menu->display_savegame_cursor(destination_surface);
}

/**
 * @brief Deletes a save file.
 * @param save_number number of the savegame to delete (0 to 2)
 */
void SelectionMenuConfirmErase::delete_save_file(int save_number) {

  Savegame *savegame = menu->get_savegame(save_number);
  FileTools::data_file_delete(savegame->get_file_name().c_str());
  menu->reload_savegames();
}

