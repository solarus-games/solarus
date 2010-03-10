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
#include "menus/SelectionMenuOptions.h"
#include "menus/SelectionMenuSelectFile.h"
#include "menus/SelectionMenu.h"
#include "StringResource.h"
#include "Configuration.h"
#include "Sprite.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Color.h"
#include "lowlevel/System.h"
#include "lowlevel/FileTools.h"

const std::string SelectionMenuOptions::label_keys[nb_options] = {
  "selection_menu.options.language",
  "selection_menu.options.video_mode",
  "selection_menu.options.music_volume",
  "selection_menu.options.sound_volume",
};

/**
 * Creates a selection menu phase where the player sets the global options.
 * @param menu the selection menu this phase will belong to
 */
SelectionMenuOptions::SelectionMenuOptions(SelectionMenu *menu):
  SelectionMenuPhase(menu, "selection_menu.phase.options"),
  cursor_position(0), modifying(false), blinking_yellow(false) {

  // option texts and values
  for (int i = 0; i < nb_options; i++) {

    // labels
    this->label_texts[i] = new TextSurface(64, 86 + i * 16, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
    this->label_texts[i]->set_font(TextSurface::FONT_STANDARD);
    this->label_texts[i]->set_text(StringResource::get_string(label_keys[i]));

    // values
    this->value_texts[i] = new TextSurface(256, 86 + i * 16, TextSurface::ALIGN_RIGHT, TextSurface::ALIGN_MIDDLE);
    this->value_texts[i]->set_font(TextSurface::FONT_STANDARD);
  }
  load_configuration();

  this->left_arrow_sprite = new Sprite("menus/arrow");
  this->left_arrow_sprite->set_current_animation("blink");
  this->left_arrow_sprite->set_current_direction(2);

  this->right_arrow_sprite = new Sprite("menus/arrow");
  this->right_arrow_sprite->set_current_animation("blink");
  this->right_arrow_sprite->set_current_direction(0);

  menu->set_bottom_options("selection_menu.validate", "selection_menu.back");
}

/**
 * Destructor.
 */
SelectionMenuOptions::~SelectionMenuOptions(void) {

  for (int i = 0; i < nb_options; i++) {
    delete label_texts[i];
    delete value_texts[i];
    delete[] all_values[i];
  }
}

/**
 * Moves the options cursor upwards.
 */
void SelectionMenuOptions::move_cursor_up(void) {

  menu->play_cursor_sound();
  left_arrow_sprite->restart_animation();

  cursor_position--;

  if (cursor_position == -1) {
    cursor_position = nb_options;
    menu->set_cursor_position(4);
  }
  else if (cursor_position == nb_options) {
    cursor_position = 3;
  }
}

/**
 * Moves the options cursor downwards.
 */
void SelectionMenuOptions::move_cursor_down(void) {

  menu->play_cursor_sound();
  left_arrow_sprite->restart_animation();

  cursor_position++;

  if (cursor_position > nb_options) {
    cursor_position = 0;
  }
  else if (cursor_position == nb_options) {
    menu->set_cursor_position(4);
  }
}

/**
 * Tries to move the options cursor to the left or to the right.
 */
void SelectionMenuOptions::move_cursor_left_or_right(void) {

  if (cursor_position >= nb_options) {

    if (cursor_position == nb_options) {
      menu->set_cursor_position(4);
      this->cursor_position = nb_options + 1;
    }
    else {
      menu->set_cursor_position(5);
      this->cursor_position = nb_options;
    }
    menu->move_cursor_left_or_right();
  }
}

/**
 * For the selected option, selects the next possible value in the list.
 */
void SelectionMenuOptions::set_option_next_value(void) {

  int index = current_indices[cursor_position];
  index = (index + 1) % nb_values[cursor_position];
  set_option_value(index);

  left_arrow_sprite->restart_animation();
  right_arrow_sprite->restart_animation();
}

/**
 * For the selected option, selects the previous possible value in the list.
 */
void SelectionMenuOptions::set_option_previous_value(void) {

  int index = current_indices[cursor_position];
  index = (index + nb_values[cursor_position] - 1) % nb_values[cursor_position];
  set_option_value(index);

  left_arrow_sprite->restart_animation();
  right_arrow_sprite->restart_animation();
}

/**
 * For the selected option, sets the value at the specified index.
 * @param index index of the value to set for the current option
 */
void SelectionMenuOptions::set_option_value(int index) {
  set_option_value(cursor_position, index);
}

/**
 * For the specified option, sets the value at the specified index.
 * @param option the option to set
 * @param index index of the value to set for the specified option
 */
void SelectionMenuOptions::set_option_value(int option, int index) {
  current_indices[option] = index;
  value_texts[option]->set_text(all_values[option][index]);
}

/**
 * Loads the values from the configuration file into the menu.
 */
void SelectionMenuOptions::load_configuration(void) {
  
  // compute the list of languages and set the current one
  std::map<std::string, std::string> language_names = FileTools::get_languages();
  const std::string &current_language_code = Configuration::get_value("language", FileTools::get_default_language());
  nb_values[0] = language_names.size();
  all_values[0] = new std::string[nb_values[0]];
  std::map<std::string, std::string>::iterator it;
  int k = 0;
  for (it = language_names.begin(); it != language_names.end(); it++) {
    all_values[0][k] = it->second;
    if (it->first == current_language_code) {
      set_option_value(0, k);
    }
    k++;
  }

  // video mode
  nb_values[1] = 0;
  all_values[1] = NULL;
  nb_values[2] = 0;
  all_values[2] = NULL;
  nb_values[3] = 0;
  all_values[3] = NULL;
}

/**
 * Saves the values of the menu into the configuration file.
 */
void SelectionMenuOptions::save_configuration(void) {

}

/**
 * Updates this phase.
 */
void SelectionMenuOptions::update(void) {

  uint32_t now = System::now();
  if (modifying && now > next_blink_date) {
    
    if (blinking_yellow) {
      /*
      label_texts[cursor_position]->set_text_color(Color::get_white());
      value_texts[cursor_position]->set_text_color(Color::get_white());
      */
    }
    else {
      label_texts[cursor_position]->set_text_color(Color::get_yellow());
      value_texts[cursor_position]->set_text_color(Color::get_yellow());
    }
    blinking_yellow = !blinking_yellow;
    next_blink_date = now + 100;
  }

  left_arrow_sprite->update();
  right_arrow_sprite->update();
}

/**
 * Handles an input event in this phase.
 * @param event the event
 */
void SelectionMenuOptions::notify_event(InputEvent &event) {

  if (event.is_direction_pressed()) {

    int direction = event.get_direction();
    if (!modifying) {
      switch (direction) {

	case 2: // up
	  move_cursor_up();
	  break;

	case 6: // down
	  move_cursor_down();
	  break;

	case 0: // right
	  move_cursor_left_or_right();
	  break;

	case 4: // left
	  move_cursor_left_or_right();
	  break;

	default:
	  break;
      }
    }

    else {
      if (direction == 0) {
        set_option_next_value();
      }
      else if (direction == 4) {
        set_option_previous_value();
      }
    }
  }

  else if (event.is_keyboard_key_pressed(validation_keys) || event.is_joypad_button_pressed()) {

    if (cursor_position < nb_options) { // set an option
      
      if (!modifying) {
	menu->play_ok_sound();
	next_blink_date = System::now();
        modifying = true;
      }
      else {
	label_texts[cursor_position]->set_text_color(Color::get_white());
	value_texts[cursor_position]->set_text_color(Color::get_white());
	modifying = false;
      }
    }
    else if (cursor_position == nb_options) { // validate
      save_configuration();
      menu->set_next_phase(new SelectionMenuSelectFile(menu));
      menu->set_cursor_position(5);
    }
    else { // back
      menu->play_ok_sound();
      menu->set_next_phase(new SelectionMenuSelectFile(menu));
      menu->set_cursor_position(5);
    }
  }
}

/**
 * Displays this selection menu phase.
 * @param destination_surface the surface to draw
 */
void SelectionMenuOptions::display(Surface *destination_surface) {

  // each option
  for (int i = 0; i < nb_options; i++) {
    label_texts[i]->display(destination_surface);
    value_texts[i]->display(destination_surface);
  }

  // bottom buttons
  menu->display_bottom_options(destination_surface);

  // cursor
  if (this->cursor_position >= nb_options) {
    menu->display_savegame_cursor(destination_surface);
  }
  else {

    int y = 80 + 16 * cursor_position;
    if (modifying) {
      right_arrow_sprite->display(destination_surface, 258, y);
      left_arrow_sprite->display(destination_surface, 246 - value_texts[cursor_position]->get_width(), y);
    }
    else {
      right_arrow_sprite->display(destination_surface, 54, y);
    }
  }
}

