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
#include "menus/SelectionMenuOptions.h"
#include "menus/SelectionMenuSelectFile.h"
#include "menus/SelectionMenu.h"
#include "StringResource.h"
#include "Configuration.h"
#include "Sprite.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Color.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

const std::string SelectionMenuOptions::label_keys[nb_options] = {
  "selection_menu.options.language",
  "selection_menu.options.video_mode",
  "selection_menu.options.music_volume",
  "selection_menu.options.sound_volume",
};

/**
 * @brief Creates a selection menu phase where the player sets the global options.
 * @param menu the selection menu this phase will belong to
 */
SelectionMenuOptions::SelectionMenuOptions(SelectionMenu *menu):
  SelectionMenuPhase(menu, "selection_menu.phase.options"),
  cursor_position(0), modifying(false) {

  // option texts and values
  for (int i = 0; i < nb_options; i++) {

    // labels
    this->label_texts[i] = new TextSurface(64, 86 + i * 16, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
    this->label_texts[i]->set_font("fixed");
    this->label_texts[i]->set_text(StringResource::get_string(label_keys[i]));

    // values
    this->value_texts[i] = new TextSurface(266, 86 + i * 16, TextSurface::ALIGN_RIGHT, TextSurface::ALIGN_MIDDLE);
    this->value_texts[i]->set_font("fixed");

    this->current_indices[i] = -1;
  }
  load_configuration();

  this->left_arrow_sprite = new Sprite("menus/arrow");
  this->left_arrow_sprite->set_current_animation("blink");
  this->left_arrow_sprite->set_current_direction(2);

  this->right_arrow_sprite = new Sprite("menus/arrow");
  this->right_arrow_sprite->set_current_animation("blink");
  this->right_arrow_sprite->set_current_direction(0);

  menu->set_bottom_options("selection_menu.back", "");
  set_cursor_position(0);
}

/**
 * @brief Destructor.
 */
SelectionMenuOptions::~SelectionMenuOptions() {

  for (int i = 0; i < nb_options; i++) {
    delete label_texts[i];
    delete value_texts[i];
    delete[] all_values[i];
  }
  delete[] language_codes;
  delete left_arrow_sprite;
  delete right_arrow_sprite;
}

/**
 * @brief Sets the current position of the options cursor.
 * @param cursor_position the new position (0 to nb_options + 1)
 */
void SelectionMenuOptions::set_cursor_position(int cursor_position) {

  Debug::check_assertion(cursor_position >= 0 && cursor_position <= nb_options,
      StringConcat() << "Illegal cursor position: " << cursor_position);

  if (this->cursor_position < nb_options) {
    // a option line was selected
    label_texts[this->cursor_position]->set_text_color(Color::get_white());
//    value_texts[this->cursor_position]->set_text_color(Color::get_white());
  }

  this->cursor_position = cursor_position;
  if (cursor_position == nb_options) {
    menu->set_cursor_position(4);
  }

  if (cursor_position < nb_options) {
    // a option line is now selected
    label_texts[cursor_position]->set_text_color(Color::get_yellow());
//    value_texts[cursor_position]->set_text_color(Color::get_yellow());
  }
}

/**
 * @brief Moves the options cursor upwards.
 */
void SelectionMenuOptions::move_cursor_up() {

  menu->play_cursor_sound();
  left_arrow_sprite->restart_animation();

  int cursor_position = this->cursor_position - 1;

  if (cursor_position == -1) {
    cursor_position = nb_options;
  }
  set_cursor_position(cursor_position);
}

/**
 * @brief Moves the options cursor downwards.
 */
void SelectionMenuOptions::move_cursor_down() {

  menu->play_cursor_sound();
  left_arrow_sprite->restart_animation();

  int cursor_position = this->cursor_position + 1;

  if (cursor_position > nb_options) {
    cursor_position = 0;
  }
  set_cursor_position(cursor_position);
}

/**
 * @brief For the selected option, selects the next possible value in the list.
 */
void SelectionMenuOptions::set_option_next_value() {

  int index = current_indices[cursor_position];
  index = (index + 1) % nb_values[cursor_position];
  set_option_value(index);

  menu->play_cursor_sound();
  left_arrow_sprite->restart_animation();
  right_arrow_sprite->restart_animation();
}

/**
 * @brief For the selected option, selects the previous possible value in the list.
 */
void SelectionMenuOptions::set_option_previous_value() {

  int index = current_indices[cursor_position];
  index = (index + nb_values[cursor_position] - 1) % nb_values[cursor_position];
  set_option_value(index);
  menu->play_cursor_sound();

  left_arrow_sprite->restart_animation();
  right_arrow_sprite->restart_animation();
}

/**
 * @brief For the selected option, sets the value at the specified index.
 * @param index index of the value to set for the current option
 */
void SelectionMenuOptions::set_option_value(int index) {
  set_option_value(cursor_position, index);
}

/**
 * @brief For the specified option, sets the value at the specified index.
 * @param option the option to set
 * @param index index of the value to set for the specified option
 */
void SelectionMenuOptions::set_option_value(int option, int index) {

  value_texts[option]->set_text(all_values[option][index]);
  if (current_indices[option] != index) {
    current_indices[option] = index;

    switch (option) {

    case 0: // language
      FileTools::set_language(language_codes[index]);
      reload_strings();
      break;

    case 1: // video mode
      VideoManager::get_instance()->set_video_mode(VideoManager::VideoMode(index));
      break;

    case 2: // music volume
      Music::set_volume(index * 10);
      break;

    case 3: // sound volume
      Sound::set_volume(index * 10);
      break;
    }
  }
}

/**
 * @brief Reloads all strings displayed on the menu.
 *
 * This function is called when the language has just been changed.
 */
void SelectionMenuOptions::reload_strings() {

  // the value of each option is language dependent only for the video mode option
  for (int i = 0; i < VideoManager::NB_MODES; i++) {
    std::ostringstream oss;
    oss << "options.video_mode_" << i;
    all_values[1][i] = StringResource::get_string(oss.str());
  }
  int index = current_indices[1];
  value_texts[1]->set_text(all_values[1][index]);

  // reload the label of each option
  for (int i = 0; i < nb_options; i++) {
    label_texts[i]->set_text(StringResource::get_string(label_keys[i]));
  }

  // other menu elements
  menu->set_title_text("selection_menu.phase.options");
  menu->set_bottom_options("selection_menu.back", "");
}

/**
 * @brief Loads the values from the configuration file into the menu.
 */
void SelectionMenuOptions::load_configuration() {
  
  // compute the list of languages and set the current one
  std::map<std::string, std::string> language_names = FileTools::get_languages();
  const std::string &current_language_code = Configuration::get_value("language", FileTools::get_default_language());
  nb_values[0] = language_names.size();
  all_values[0] = new std::string[nb_values[0]];
  language_codes = new std::string[nb_values[0]];
  std::map<std::string, std::string>::iterator it;
  int i = 0;
  for (it = language_names.begin(); it != language_names.end(); it++) {
    language_codes[i] = it->first;
    all_values[0][i] = it->second;

    if (language_codes[i] == current_language_code) {
      current_indices[0] = i;
      set_option_value(0, i);
    }
    i++;
  }

  // video mode
  nb_values[1] = VideoManager::NB_MODES;
  all_values[1] = new std::string[nb_values[1]];
  for (int i = 0; i < nb_values[1]; i++) {
    std::ostringstream oss;
    oss << "options.video_mode_" << i;
    all_values[1][i] = StringResource::get_string(oss.str());

    if (i == VideoManager::get_instance()->get_video_mode()) {
      current_indices[1] = i;
      set_option_value(1, i);
    }
  }

  // music volume
  int volume = Music::get_volume();
  if (volume % 10 != 0) {
    // make sure the volume is a multiple of 10
    volume = (volume + 5) / 10 * 10;
    Music::set_volume(volume);
  }
  nb_values[2] = 11;
  all_values[2] = new std::string[nb_values[2]];
  for (int i = 0; i < nb_values[2]; i++) {
    volume = i * 10;
    std::ostringstream oss;
    oss << volume << " %";
    all_values[2][i] = oss.str();

    if (volume == Music::get_volume()) {
      current_indices[2] = i;
      set_option_value(2, i);
    }
  }

  // sound volume
  volume = Sound::get_volume();
  if (volume % 10 != 0) {
    // make sure the volume is a multiple of 10
    volume = (volume + 5) / 10 * 10;
    Sound::set_volume(volume);
  }
  nb_values[3] = 11;
  all_values[3] = new std::string[nb_values[3]];
  for (int i = 0; i < nb_values[3]; i++) {
    volume = i * 10;
    std::ostringstream oss;
    oss << volume << " %";
    all_values[3][i] = oss.str();

    if (volume == Sound::get_volume()) {
      current_indices[3] = i;
      set_option_value(3, i);
    }
  }
}

/**
 * @brief Updates this phase.
 */
void SelectionMenuOptions::update() {

  left_arrow_sprite->update();
  right_arrow_sprite->update();
}

/**
 * @brief Handles an input event in this phase.
 * @param event the event
 */
void SelectionMenuOptions::notify_event(InputEvent &event) {

  if (event.is_direction_pressed()) {

    int direction = event.get_direction();
    if (!modifying) {

      if (direction == 2) { // up
        move_cursor_up();
      }
      else if (direction == 6) { // down
        move_cursor_down();
      }
    }
    else {

      if (direction == 0) { // right
        set_option_next_value();
      }
      else if (direction == 4) { // left
        set_option_previous_value();
      }
    }
  }

  else if (event.is_keyboard_key_pressed(validation_keys) || event.is_joypad_button_pressed()) {

    if (cursor_position < nb_options) { // set an option

      if (!modifying) {
        menu->play_ok_sound();
        left_arrow_sprite->restart_animation();
        right_arrow_sprite->restart_animation();
        label_texts[cursor_position]->set_text_color(Color::get_white());
        value_texts[cursor_position]->set_text_color(Color::get_yellow());
        menu->set_title_text("selection_menu.phase.options.changing");
        modifying = true;
      }
      else {
        menu->play_letter_sound();
        label_texts[cursor_position]->set_text_color(Color::get_yellow());
        value_texts[cursor_position]->set_text_color(Color::get_white());
        menu->set_title_text("selection_menu.phase.options");
        modifying = false;
      }
    }
    else if (cursor_position == nb_options) { // back
      menu->play_ok_sound();
      menu->set_next_phase(new SelectionMenuSelectFile(menu, 5));
    }
  }
}

/**
 * @brief Displays this selection menu phase.
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
  if (this->cursor_position == nb_options) {
    menu->display_savegame_cursor(destination_surface);
  }
  else {

    int y = 80 + 16 * cursor_position;
    if (modifying) {
      right_arrow_sprite->display(destination_surface, 268, y);
      left_arrow_sprite->display(destination_surface, 256 - value_texts[cursor_position]->get_width(), y);
    }
    else {
      right_arrow_sprite->display(destination_surface, 54, y);
    }
  }
}

