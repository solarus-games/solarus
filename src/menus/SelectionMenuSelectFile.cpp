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
#include "menus/SelectionMenuSelectFile.h"
#include "menus/SelectionMenuEraseFile.h"
#include "menus/SelectionMenuChooseName.h"
#include "menus/SelectionMenuChooseMode.h"
#include "Transition.h"
#include "Savegame.h"
#include "Sprite.h"
#include "ZSDX.h"

/**
 * Creates a selection menu with the phase where the
 * player selects a file.
 * @param previous the previous phase (if we were already in
 * the selection menu), or NULL
 */
SelectionMenuSelectFile::SelectionMenuSelectFile(SelectionMenuPhase *previous):
  SelectionMenuPhase(previous, "selection_menu.select_file") {

  set_bottom_options("selection_menu.erase", "selection_menu.exit");

  // initialize the cursor
  get_cursor()->set_current_animation("blue");

  if (previous == NULL) {
    // we don't come from another phase or the selection menu itself: make a transition
    transition = Transition::create(Transition::FADE, Transition::IN);
    transition->start();
    set_cursor_position(1);
  }
}

/**
 * Destructor.
 */
SelectionMenuSelectFile::~SelectionMenuSelectFile(void) {

}

/**
 * Handles an SDL event in this phase.
 * @param event the event
 */
void SelectionMenuSelectFile::handle_event(const SDL_Event &event) {

  if (transition == NULL && event.type == SDL_KEYDOWN) {

    int cursor_position = get_cursor_position();
    
    switch (event.key.keysym.sym) {

    case SDLK_SPACE:
    case SDLK_RETURN:
      if (cursor_position == 5) {
	// the user chose "Exit"
	zsdx->set_exiting();
      }
      else if (cursor_position == 4) {
	// the user chose "Erase"
	play_ok_sound();
	set_next_screen(new SelectionMenuEraseFile(this));
      }
      else {
	// the user chose a save
	play_ok_sound();
	
	Savegame **savegames = get_savegames();
	if (savegames[cursor_position - 1]->is_empty()) {
	  // the savegame doesn't exist: ask the name
	  set_next_screen(new SelectionMenuChooseName(this));
	}
	else {
	  // the savegame exists: choose the mode and then start the game
	  set_next_screen(new SelectionMenuChooseMode(this));
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
void SelectionMenuSelectFile::display(SDL_Surface *screen_surface) {

  start_display(screen_surface);

  // savegames
  for (int i = 0; i < 3; i++) {
    display_savegame(i);
  }

  // options
  display_bottom_options();

  // cursor
  display_normal_cursor();
  
  // save numbers
  for (int i = 0; i < 3; i++) {
    display_savegame_number(i);
  }

  finish_display(screen_surface);
}

