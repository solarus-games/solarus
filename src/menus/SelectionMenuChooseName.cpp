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
#include "menus/SelectionMenuChooseName.h"
#include "menus/SelectionMenuSelectFile.h"
#include "Sprite.h"
#include "Savegame.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"

/**
 * Creates a selection menu with the phase where the
 * player has to choose his name.
 * @param previous the previous phase
 */
SelectionMenuChooseName::SelectionMenuChooseName(SelectionMenu *menu):
  SelectionMenuPhase(menu, "selection_menu.choose_name"), next_key_date(System::now()) {

  SDL_EnableKeyRepeat(300, 50);

  menu->get_cursor_sprite()->set_current_animation("letters");

  player_name[0] = '\0';

  player_name_text = new TextSurface(67, 85, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  player_name_text->set_text(player_name);

  letter_cursor_x = 0;
  letter_cursor_y = 0;

  arrow_img = new Surface("menus/selection_menu_arrow.png");
  letters_img = new Surface("menus/selection_menu_letters.png");
}

/**
 * Destructor.
 */
SelectionMenuChooseName::~SelectionMenuChooseName(void) {

  SDL_EnableKeyRepeat(0, 0);

  delete player_name_text;
  delete arrow_img;
  delete letters_img;
}

/**
 * Handles an event in this phase.
 * @param event the event
 */
void SelectionMenuChooseName::handle_event(const SDL_Event &event) {

  if (event.type == SDL_KEYDOWN) {

    uint32_t now = System::now();

    bool finished = false;
    switch (event.key.keysym.sym) {

    case SDLK_RETURN:
      // directly validate the name
      finished = validate_player_name();
      break;
  
    case SDLK_c:
    case SDLK_SPACE:
      if (now >= next_key_date) {
	// choose a letter
	finished = select_letter();
	player_name_text->set_text(player_name);
	next_key_date = now + 500;
      }
      break;

    case SDLK_RIGHT:
      menu->play_cursor_sound();
      letter_cursor_x = (letter_cursor_x + 14) % 13;
      break;
	  
    case SDLK_UP:
      menu->play_cursor_sound();
      letter_cursor_y = (letter_cursor_y + 4) % 5;
      break;
	  
    case SDLK_LEFT:
      menu->play_cursor_sound();
      letter_cursor_x = (letter_cursor_x + 12) % 13;
      break;
	  
    case SDLK_DOWN:
      menu->play_cursor_sound();
      letter_cursor_y = (letter_cursor_y + 6) % 5;
      break;
  
    default:
      break;
    }

    if (finished) {
      menu->set_next_phase(new SelectionMenuSelectFile(menu));
    }
  }
}

/**
 * Displays the selection menu phase.
 * @param destination_surface the surface to draw
 */
void SelectionMenuChooseName::display(Surface *destination_surface) {

  // cursor
  menu->get_cursor_sprite()->display(destination_surface,
      51 + 16 * letter_cursor_x, 93 + 18 * letter_cursor_y);
 
  // current name
  Rectangle position(57, 76, 0, 0);
  arrow_img->blit(destination_surface, position);
  player_name_text->display(destination_surface);

  // letters
  position.set_y(98);
  letters_img->blit(destination_surface, position);
}

/**
 * This function is called when the player chooses a letter when typing his name.
 * @return true if he finished typing the name (because he validated or cancelled)
 */
bool SelectionMenuChooseName::select_letter(void) {

  size_t length = strlen(player_name);
  char letter_to_add = '\0';
  bool finished = false;

  switch (letter_cursor_y) {
    
  case 0:
    // upper case letter from A to M
    letter_to_add = 'A' + letter_cursor_x;
    break;
    
  case 1:
    // upper case letter from N to Z
    letter_to_add = 'N' + letter_cursor_x;
    break;

  case 2:
    // lower case letter from a to m
    letter_to_add = 'a' + letter_cursor_x;
    break;
    
  case 3:
    // lower case letter from n to z
    letter_to_add = 'n' + letter_cursor_x;
    break;

  case 4:
    // digit or special command

    if (letter_cursor_x <= 9) {
      // digit
      letter_to_add = '0' + letter_cursor_x;
    }
    else {
      // special command
      switch (letter_cursor_x) {

      case 10:
	// remove the last letter
	if (length > 0) {
	  player_name[length - 1] = '\0';
	  menu->play_letter_sound();
	}
	else {
	  menu->play_error_sound();
	}
	break;

      case 11:
	// validate the choice
	finished = validate_player_name();
	break;

      case 12:
	// cancel
	menu->play_letter_sound();
	finished = true;
	break;
      }
    }
    break;
  }

  if (letter_to_add != '\0') {
    // a letter was selected
    if (length < 6) {
      player_name[length] = letter_to_add;
      player_name[length + 1] = '\0';
      menu->play_letter_sound();
    }
    else {
      menu->play_error_sound();
    }
  }  

  return finished;
}

/**
 * This function is called when the player wants to finish typing his name.
 * @return true if the new name is valid, false otherwise
 */
bool SelectionMenuChooseName::validate_player_name(void) {

  if (strlen(player_name) == 0) {
    menu->play_error_sound();
    return false;
  }

  menu->play_ok_sound();

  // create the savegame
  Savegame *savegame = menu->get_savegame(menu->get_cursor_position() - 1);
  savegame->set_string(Savegame::PLAYER_NAME, player_name);
  savegame->save();
  menu->reload_savegames();

  return true;
}

