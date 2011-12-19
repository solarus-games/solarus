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
#include "menus/SelectionMenuChooseName.h"
#include "menus/SelectionMenuSelectFile.h"
#include "Sprite.h"
#include "Savegame.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"

/**
 * @brief Creates a selection menu with the phase where the
 * player has to choose his name.
 * @param menu the selection menu this phase will belong to
 */
SelectionMenuChooseName::SelectionMenuChooseName(SelectionMenu *menu):
  SelectionMenuPhase(menu, "selection_menu.phase.choose_name"), next_key_date(System::now()) {

  InputEvent::set_key_repeat(300, 50);

  menu->get_cursor_sprite()->set_current_animation("letters");

  player_name[0] = '\0';

  player_name_text = new TextSurface(67, 85, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  player_name_text->set_text(player_name);

  letter_cursor_x = 0;
  letter_cursor_y = 0;

  letters_img = new Surface("menus/selection_menu_letters.png");
  arrow_sprite = new Sprite("menus/arrow");
  arrow_sprite->set_current_direction(0);
}

/**
 * @brief Destructor.
 */
SelectionMenuChooseName::~SelectionMenuChooseName() {

  InputEvent::set_key_repeat(0, 0);

  delete player_name_text;
  delete arrow_sprite;
  delete letters_img;
}

/**
 * @brief Updates this phase.
 */
void SelectionMenuChooseName::update() {
  arrow_sprite->update();
}

/**
 * @brief Handles an event in this phase.
 * @param event the event
 */
void SelectionMenuChooseName::notify_event(InputEvent &event) {

  static const InputEvent::KeyboardKey letter_keys[] = { InputEvent::KEY_c, InputEvent::KEY_SPACE, InputEvent::KEY_NONE };

  if (event.is_pressed()) {

    uint32_t now = System::now();
    bool finished = false;

    if (event.is_keyboard_key_pressed(InputEvent::KEY_RETURN)) {

      // directly validate the name
      finished = validate_player_name();
    }

    else if (event.is_keyboard_key_pressed(letter_keys) || event.is_joypad_button_pressed()) {

      if (now >= next_key_date) {
	// choose a letter
	finished = select_letter();
	player_name_text->set_text(player_name);
	next_key_date = now + 500;
      }
    }

    else if (event.is_direction_pressed()) {

      switch (event.get_direction()) {

	case 0: // right
	  menu->play_cursor_sound();
	  letter_cursor_x = (letter_cursor_x + 14) % 13;
	  break;

	case 2: // up
	  menu->play_cursor_sound();
	  letter_cursor_y = (letter_cursor_y + 4) % 5;
	  break;

	case 4: // left
	  menu->play_cursor_sound();
	  letter_cursor_x = (letter_cursor_x + 12) % 13;
	  break;

	case 6: // down
	  menu->play_cursor_sound();
	  letter_cursor_y = (letter_cursor_y + 6) % 5;
	  break;

	default:
	  break;
      }
    }

    if (finished) {
      menu->set_next_phase(new SelectionMenuSelectFile(menu, menu->get_cursor_position()));
    }
  }
}

/**
 * @brief Displays the selection menu phase.
 * @param destination_surface the surface to draw
 */
void SelectionMenuChooseName::display(Surface *destination_surface) {

  // cursor
  menu->get_cursor_sprite()->display(destination_surface,
      51 + 16 * letter_cursor_x, 93 + 18 * letter_cursor_y);
 
  // current name
  arrow_sprite->display(destination_surface, 57, 76);
  player_name_text->display(destination_surface);

  // letters
  Rectangle position(57, 98);
  letters_img->blit(destination_surface, position);
}

/**
 * @brief This function is called when the player chooses a letter when typing his name.
 * @return true if he finished typing the name (because he validated or cancelled)
 */
bool SelectionMenuChooseName::select_letter() {

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
 * @brief This function is called when the player wants to finish typing his name.
 * @return true if the new name is valid, false otherwise
 */
bool SelectionMenuChooseName::validate_player_name() {

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

