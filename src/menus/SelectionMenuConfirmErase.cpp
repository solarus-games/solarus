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
#include "ZSDX.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Savegame.h"
#include "FileTools.h"

/**
 * Creates a selection menu with the phase where the
 * player has to confirm to erase a file.
 * @param previous the previous phase
 * @param save_number_to_erase number of the savegame to delete (0 to 2)
 */
SelectionMenuConfirmErase::SelectionMenuConfirmErase(SelectionMenuPhase *previous, int save_number_to_erase):
  SelectionMenuPhase(previous, "selection_menu.confirm_erase"),
  save_number_to_erase(save_number_to_erase) {

  set_bottom_options("selection_menu.big_no", "selection_menu.big_yes");
  set_cursor_position(4); // select "no" by default
}

/**
 * Destructor.
 */
SelectionMenuConfirmErase::~SelectionMenuConfirmErase(void) {

}

/**
 * Handles an SDL event in this phase.
 * @param event the event
 */
void SelectionMenuConfirmErase::handle_event(const SDL_Event &event) {

  if (event.type == SDL_KEYDOWN) {
    
    int cursor_position = get_cursor_position();

    switch (event.key.keysym.sym) {

    case SDLK_SPACE:
    case SDLK_RETURN:
      if (cursor_position == 5) {
	// the user chose "Yes"
	ResourceManager::get_sound("boss_killed")->play();
	delete_save_file(save_number_to_erase);
	set_cursor_position(save_number_to_erase + 1);
	set_next_screen(new SelectionMenuSelectFile(this));
      }
      else if (cursor_position == 4) {
	// the user chose "No"
	play_ok_sound();
	set_next_screen(new SelectionMenuSelectFile(this));
      }
      break;

    case SDLK_RIGHT:
    case SDLK_LEFT:
      move_cursor_left_or_right();
      break;

    default:
      break;
    }
  }
}

/**
 * Displays the selection menu in this phase.
 */
void SelectionMenuConfirmErase::display(SDL_Surface *screen_surface) {

  start_display(screen_surface);

  // savegame
  display_savegame(save_number_to_erase);
  display_savegame_number(save_number_to_erase);

  // options
  display_bottom_options();

  // cursor
  display_normal_cursor();
 
  finish_display(screen_surface);
}

/**
 * Deletes a save file.
 * @param save_number number of the savegame to delete (0 to 2)
 */
void SelectionMenuConfirmErase::delete_save_file(int save_number) {

  Savegame *savegame = get_savegame(save_number);
  FileTools::data_file_delete(savegame->get_file_name().c_str());
  reload_savegames();
}

