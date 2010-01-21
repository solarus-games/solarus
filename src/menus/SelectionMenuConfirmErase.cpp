/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
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
 * Creates a selection menu phase where the player has to confirm to erase a file.
 * @param menu the selection menu this phase will belong to
 * @param save_number_to_erase number of the savegame to delete (0 to 2)
 */
SelectionMenuConfirmErase::SelectionMenuConfirmErase(SelectionMenu *menu, int save_number_to_erase):
  SelectionMenuPhase(menu, "selection_menu.confirm_erase"),
  save_number_to_erase(save_number_to_erase) {

  menu->set_bottom_options("selection_menu.big_no", "selection_menu.big_yes");
  menu->set_cursor_position(4); // select "no" by default
}

/**
 * Destructor.
 */
SelectionMenuConfirmErase::~SelectionMenuConfirmErase(void) {

}

/**
 * Handles an event in this phase.
 * @param event the event
 */
void SelectionMenuConfirmErase::handle_event(const SDL_Event &event) {

  if (event.type == SDL_KEYDOWN) {
    
    int cursor_position = menu->get_cursor_position();

    switch (event.key.keysym.sym) {

    case SDLK_SPACE:
    case SDLK_RETURN:
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
      break;

    case SDLK_RIGHT:
    case SDLK_LEFT:
      menu->move_cursor_left_or_right();
      break;

    default:
      break;
    }
  }
}

/**
 * Displays this selection menu phase.
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
 * Deletes a save file.
 * @param save_number number of the savegame to delete (0 to 2)
 */
void SelectionMenuConfirmErase::delete_save_file(int save_number) {

  Savegame *savegame = menu->get_savegame(save_number);
  FileTools::data_file_delete(savegame->get_file_name().c_str());
  menu->reload_savegames();
}

