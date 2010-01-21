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
#include "menus/SelectionMenuChooseName.h"
#include "menus/SelectionMenuSelectFile.h"
#include "ResourceManager.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "lowlevel/TextSurface.h"
#include "Savegame.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"
#include "lowlevel/FileTools.h"

/**
 * Creates a selection menu with the phase where the
 * player has to choose his name.
 * @param previous the previous phase
 */
SelectionMenuChooseName::SelectionMenuChooseName(SelectionMenuPhase *previous):
  SelectionMenuPhase(previous, "selection_menu.choose_name"),
  next_key_date(System::now()) {

  SDL_EnableKeyRepeat(300, 50);

  get_cursor()->set_current_animation("letters");

  player_name[0] = '\0';

  text_new_player_name = new TextSurface(67, 85, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  text_new_player_name->set_text(player_name);

  x_letter_cursor = 0;
  y_letter_cursor = 0;

  img_arrow = ResourceManager::load_image("menus/selection_menu_arrow.png");
  img_letters = ResourceManager::load_image("menus/selection_menu_letters.png");
}

/**
 * Destructor.
 */
SelectionMenuChooseName::~SelectionMenuChooseName(void) {

  SDL_EnableKeyRepeat(0, 0);

  delete text_new_player_name;
  delete img_arrow;
  delete img_letters;
}

/**
 * Handles an SDL event in this phase.
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
	text_new_player_name->set_text(player_name);
	next_key_date = now + 500;
      }
      break;

    case SDLK_RIGHT:
      play_cursor_sound();
      x_letter_cursor = (x_letter_cursor + 14) % 13;
      break;
	  
    case SDLK_UP:
      play_cursor_sound();
      y_letter_cursor = (y_letter_cursor + 4) % 5;
      break;
	  
    case SDLK_LEFT:
      play_cursor_sound();
      x_letter_cursor = (x_letter_cursor + 12) % 13;
      break;
	  
    case SDLK_DOWN:
      play_cursor_sound();
      y_letter_cursor = (y_letter_cursor + 6) % 5;
      break;
  
    default:
      break;
    }

    if (finished) {
      set_next_screen(new SelectionMenuSelectFile(this));
    }
  }
}

/**
 * Displays the selection menu in this phase.
 */
void SelectionMenuChooseName::display(Surface *screen_surface) {

  start_display(screen_surface);

  // cursor
  get_cursor()->display(destination_surface,
      51 + 16 * x_letter_cursor,
      93 + 18 * y_letter_cursor);
 
  // current name
  Rectangle position(57, 76, 0, 0);
  img_arrow->blit(destination_surface, position);
  text_new_player_name->display(destination_surface);

  // letters
  position.set_y(98);
  img_letters->blit(destination_surface, position);

  finish_display(screen_surface);
}

/**
 * This function is called when the player chooses a letter
 * when typing his name.
 * @return true if he finished typing the name (because he validated
 * or cancelled), false otherwise
 */
bool SelectionMenuChooseName::select_letter(void) {

  size_t length = strlen(player_name);
  char letter_to_add = '\0';
  bool finished = false;
  Sound *letter_sound = ResourceManager::get_sound("danger");

  switch (y_letter_cursor) {
    
  case 0:
    // upper case letter from A to M
    letter_to_add = 'A' + x_letter_cursor;
    break;
    
  case 1:
    // upper case letter from N to Z
    letter_to_add = 'N' + x_letter_cursor;
    break;

  case 2:
    // lower case letter from a to m
    letter_to_add = 'a' + x_letter_cursor;
    break;
    
  case 3:
    // lower case letter from n to z
    letter_to_add = 'n' + x_letter_cursor;
    break;

  case 4:
    // digit or special command

    if (x_letter_cursor <= 9) {
      // digit
      letter_to_add = '0' + x_letter_cursor;
    }
    else {
      // special command
      switch (x_letter_cursor) {

      case 10:
	// remove the last letter
	if (length > 0) {
	  player_name[length - 1] = '\0';
	  letter_sound->play();
	}
	else {
	  play_error_sound();
	}
	break;

      case 11:
	// validate the choice
	finished = validate_player_name();
	break;

      case 12:
	// cancel
	letter_sound->play();
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
      letter_sound->play();
    }
    else {
      play_error_sound();
    }
  }  

  return finished;
}

/**
 * This function is called when the player wants to finish
 * typing his name.
 * @return true if the new name is valid, false otherwise
 */
bool SelectionMenuChooseName::validate_player_name(void) {

  if (strlen(player_name) == 0) {
    play_error_sound();
    return false;
  }

  play_ok_sound();

  // create the savegame
  Savegame *savegame = get_savegame(get_cursor_position() - 1);
  savegame->set_string(Savegame::PLAYER_NAME, player_name);
  savegame->save();
  reload_savegames();

  return true;
}

