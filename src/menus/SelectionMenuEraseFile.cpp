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
#include "menus/SelectionMenuEraseFile.h"
#include "menus/SelectionMenuSelectFile.h"
#include "menus/SelectionMenuConfirmErase.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Savegame.h"

/**
 * Creates a selection menu with the phase where the
 * player chooses a file to erase.
 * @param previous the previous phase
 */
SelectionMenuEraseFile::SelectionMenuEraseFile(SelectionMenuPhase *previous):
  SelectionMenuPhase(previous,
		     "Quel fichier voulez-vous effacer ?") {

  get_cursor()->set_current_animation("red");
}

/**
 * Destructor.
 */
SelectionMenuEraseFile::~SelectionMenuEraseFile(void) {

}

/**
 * Handles an SDL event in this phase.
 * @param event the event
 */
void SelectionMenuEraseFile::handle_event(const SDL_Event &event) {

  if (event.type == SDL_KEYDOWN) {
    
    int cursor_position = get_cursor_position();
    switch (event.key.keysym.sym) {

    case SDLK_SPACE:
    case SDLK_RETURN:
      if (cursor_position == 5) {
	// the user chose "Quit"
	zsdx->set_exiting();
      }
      else if (cursor_position == 4) {
	// the user chose "Cancel"
	play_ok_sound();
	set_next_screen(new SelectionMenuSelectFile(this));
      }
      else {
	
	int save_number_to_erase = cursor_position - 1;
	if (get_savegame(save_number_to_erase)->is_empty()) {
	  // the savegame doesn't exist: error sound
	  play_error_sound();
	}
	else {
	  // the savegame exists: confirm deleting it
	  play_ok_sound();
	  set_next_screen(new SelectionMenuConfirmErase(this, save_number_to_erase));
	}
      }
      break;
      
    case SDLK_DOWN:
      move_cursor_down();
      break;
      
    case SDLK_UP:
      move_cursor_up();
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
void SelectionMenuEraseFile::display(SDL_Surface *screen_surface) {

  start_display(screen_surface);

  // savegames
  for (int i = 0; i < 3; i++) {
    display_savegame(i);
  }

  // options
  display_options("Annuler", "Quitter");

  // cursor
  display_normal_cursor();
 
  // save numbers
  for (int i = 0; i < 3; i++) {
    display_savegame_number(i);
  }

  finish_display(screen_surface);
}
