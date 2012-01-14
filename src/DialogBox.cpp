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
#include "DialogBox.h"
#include "DialogResource.h"
#include "Sprite.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "lua/MapScript.h"
#include "entities/Hero.h"
#include "lowlevel/Color.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"

const uint32_t DialogBox::char_delays[] = {
  60, // slow
  45, // medium
  30  // fast (default)
};

/**
 * @brief Creates a new dialog box.
 * @param game the game this dialog box belongs to
 */
DialogBox::DialogBox(Game &game):
  game(game),
  issuer_script(NULL),
  dialog_surface(320, 240),
  box_img("hud/dialog_box.png"),
  icons_img("hud/dialog_icons.png"),
  end_lines_sprite("hud/dialog_box_message_end"),
  box_src_position(0, 0, 220, 60),
  box_dst_position(0, 0, 220, 60),
  question_src_position(96, 60, 8, 8) {

  dialog_surface.set_transparency_color(Color::get_black());
  set_style(STYLE_WITH_FRAME);

  for (int i = 0; i < nb_visible_lines; i++) {
    line_surfaces[i] = new TextSurface(0, 0, TextSurface::ALIGN_LEFT,
        TextSurface::ALIGN_TOP);
  }
}

/**
 * @brief Destructor.
 */
DialogBox::~DialogBox() {

  for (int i = 0; i < nb_visible_lines; i++) {
    delete line_surfaces[i];
  }
}

/**
 * @brief Returns the game where this dialog box is displayed.
 * @return the current game
 */
Game& DialogBox::get_game() {

  return game;
}

/**
 * @brief Returns whether the dialog box is currently active.
 * @return true if the dialog box is enabled
 */
bool DialogBox::is_enabled() {

  return dialog_id.size() > 0;
}

/**
 * @brief Sets the dialog box style for all subsequent dialogs.
 *
 * The default style is DialogBox::STYLE_WITH_FRAME.
 *
 * @param style the new style to set
 */
void DialogBox::set_style(Style style) {

  this->style = style;

  if (style != STYLE_WITHOUT_FRAME) {
    dialog_surface.set_opacity(216);
  }
}

/**
 * @brief Sets the vertical position of the dialog box.
 * @param vertical_position the vertical position
 */
void DialogBox::set_vertical_position(VerticalPosition vertical_position) {

  if (vertical_position == POSITION_AUTO) {
    // determine the position
    const Rectangle& camera_position = game.get_current_map().get_camera_position();
    vertical_position = POSITION_BOTTOM;

    if (game.get_hero().get_y() >= camera_position.get_y() + 130) {
      vertical_position = POSITION_TOP;
    }
  }

  // set the coordinates of graphic objects
  int x = 50;
  int y = (vertical_position == POSITION_TOP) ? 32 : 144;

  if (style == STYLE_WITHOUT_FRAME) {
    y += (vertical_position == POSITION_TOP) ? (-24) : 24;
  }

  box_dst_position.set_xy(x, y);
  question_dst_position.set_xy(x + 18, y + 27);
  icon_dst_position.set_xy(x + 18, y + 22);
}

/**
 * @brief Returns whether a sound should be played when displaying the letters.
 * @return true if a sound should be played when displaying the letters
 */
bool DialogBox::is_letter_sound_enabled() {

  return style != STYLE_WITHOUT_FRAME;
}

/**
 * @brief Specifies the value of a variable that will occur in a dialog.
 *
 * A value is expected in a dialog when the '$v' sequence is read.
 * You can specify only one variable at the same time per dialog.
 * If a variable was already specified for this dialog, it is replaced.
 *
 * @param dialog_id id of the dialog where this value will appear
 * @param value the value to add
 */
void DialogBox::set_variable(const std::string& dialog_id,
    const std::string& value) {

  variables[dialog_id] = value;
}

/**
 * @brief Same thing as set_variable(string, string) but with an integer
 * parameter.
 *
 * This function just converts the integer value to a string
 * add calls the other function.
 *
 * @param dialog id id of the dialog where this value will appear
 * @param value the value to set
 */
void DialogBox::set_variable(const std::string& dialog_id, int value) {

  std::ostringstream oss;
  oss << value;
  set_variable(dialog_id, oss.str());
}

/**
 * @brief Returns the variable specified by a previous
 * call to set_variable(), for the current dialog.
 * This function is called by
 * the current message when it reads the '$v' sequence.
 * @return the value of the variable
 */
const std::string& DialogBox::get_variable() {

  const std::string& value = variables[dialog_id];

  Debug::check_assertion(value.size() > 0, StringConcat()
      << "Missing variable in dialog '" << dialog_id << "'");

  return value;
}

/**
 * @brief Returns the answer selected by the player in the question of the
 * last dialog.
 * @return the answer selected: 0 for the first one, 1 for the second one,
 * -1 if the last dialog was not a question
 */
int DialogBox::get_last_answer() {

  return last_answer;
}

/**
 * @brief Starts a dialog.
 *
 * If there was already a dialog, it must be finished.
 *
 * @param dialog_id of the dialog
 * @param issuer_script the script that issued the request to start a dialog
 * (will be notified when the dialog finishes), or NULL
 * @param vertical_position vertical position where to display the dialog box (default: auto)
 */
void DialogBox::start_dialog(const std::string& dialog_id, Script* issuer_script,
    VerticalPosition vertical_position) {

  Debug::check_assertion(!is_enabled() || is_full(), StringConcat()
      << "Cannot start dialog '" << dialog_id
      << "': another dialog '" << this->dialog_id << "' is already started");

  bool first = !is_enabled();
  if (first) {
    // save the action and sword keys
    KeysEffect& keys_effect = game.get_keys_effect();
    action_key_effect_saved = keys_effect.get_action_key_effect();
    sword_key_effect_saved = keys_effect.get_sword_key_effect();
    this->icon_number = -1;
    this->skip_mode = Dialog::SKIP_NONE;
    this->char_delay = char_delays[SPEED_FAST];
    this->dialog_id = dialog_id;
  }

  // initialize the dialog data
  this->dialog = DialogResource::get_dialog(dialog_id);
  this->line_it = dialog.get_lines().begin();
  this->line_index = 0;
  this->char_index = 0;
  this->skipped = false;

  if (dialog.get_skip_mode() != Dialog::SKIP_UNCHANGED) {
    this->skip_mode = dialog.get_skip_mode();
  }

  if (dialog.get_icon() != -2) { // -2 means unchanged
    this->icon_number = dialog.get_icon();
  }
  if (dialog.is_question()) {
    this->last_answer = 0;
  }
  else {
    this->last_answer = -1;
  }
  question_dst_position.set_y(box_dst_position.get_y() + 27);

  if (first) {
    set_vertical_position(vertical_position);

    // notify the scripts
    game.get_map_script().event_dialog_started(dialog_id);
    this->issuer_script = issuer_script;
    if (issuer_script != NULL) {
      issuer_script->event_dialog_started(dialog_id);
    }
  }

  // start displaying text
  show_more_lines();
}

/**
 * @brief Shows a new group of 3 lines in the dialog box, if any.
 */
void DialogBox::show_more_lines() {

  if (!has_more_lines()) {
    show_next_dialog();
    return;
  }

  // hide the action icon and change the sword icon
  KeysEffect& keys_effect = game.get_keys_effect();
  keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (skip_mode != Dialog::SKIP_NONE) {
    keys_effect.set_sword_key_effect(KeysEffect::SWORD_KEY_SKIP);
    keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NEXT);
  }
  else {
    keys_effect.set_sword_key_effect(KeysEffect::SWORD_KEY_HIDDEN);
  }

  // hide the bottom arrow
  end_lines_sprite.stop_animation();

  // prepare the lines
  int text_x = box_dst_position.get_x() + ((icon_number == -1) ? 16 : 48);
  int text_y = box_dst_position.get_y() - 1;
  for (int i = 0; i < nb_visible_lines; i++) {
    text_y += 13;
    line_surfaces[i]->set_x(text_x);
    line_surfaces[i]->set_y(text_y);
    line_surfaces[i]->set_text("");

    if (has_more_lines()) {
      lines[i] = *line_it;
      line_it++;
    }
    else {
      lines[i] = "";
    }
  }

  if (dialog.is_question() && !has_more_lines()) {
    text_x += 24;
    line_surfaces[nb_visible_lines - 2]->set_x(text_x);
    line_surfaces[nb_visible_lines - 1]->set_x(text_x);
  }

  // initialize the lines
  this->line_index = 0;
  this->char_index = 0;
  this->next_char_date = this->next_sound_date = System::now();
}

/** 
 * @brief Shows the next dialog (if any).
 */
void DialogBox::show_next_dialog() {

  const std::string& next_dialog_id =
      (dialog.is_question() && last_answer == 1) ?
      dialog.get_next2() : dialog.get_next();

  if (next_dialog_id.size() > 0 && next_dialog_id != "_unknown") {
    start_dialog(next_dialog_id);
  }
  else {
    close();
  }
}

/**
 * @brief Closes the dialog box.
 */
void DialogBox::close() {

  const std::string previous_dialog_id = dialog_id;
  Script* previous_issuer_script = issuer_script;
  dialog_id = "";

  // restore the action and sword keys
  KeysEffect& keys_effect = game.get_keys_effect();
  keys_effect.set_action_key_effect(action_key_effect_saved);
  keys_effect.set_sword_key_effect(sword_key_effect_saved);

  // notify the script if necessary
  if (!skipped && previous_dialog_id[0] != '_') { // FIXME: remove the '_' restriction
    // a dialog of the quest was just finished: notify the scripts
    Script& map_script = game.get_map_script();
    map_script.event_dialog_finished(previous_dialog_id, last_answer);

    if (previous_issuer_script != NULL
        && previous_issuer_script != &map_script) {
      // also notify the issuer script if different
      previous_issuer_script->event_dialog_finished(previous_dialog_id, last_answer);
    }
  }

}

/**
 * @brief This function is called by the game when a key is pressed
 * while displaying a dialog.
 * @param key the key pressed
 */
void DialogBox::key_pressed(GameControls::GameKey key) {

  switch (key) {

    // action key
    case GameControls::ACTION:
      action_key_pressed();
      break;

      // sword key
    case GameControls::SWORD:
      sword_key_pressed();
      break;

    case GameControls::UP:
    case GameControls::DOWN:
      up_or_down_key_pressed();

    default:
      break;
  }
}

/**
 * @brief This function is called when the user presses the action key.
 */
void DialogBox::action_key_pressed() {

  if (is_full()) { // the current group of 3 lines is over
    show_more_lines();
  }
  else if (skip_mode != Dialog::SKIP_NONE) {
    show_all_now();
  }
}

/**
 * @brief This function is called when the user presses the sword key.
 */
void DialogBox::sword_key_pressed() {

  if (skip_mode == Dialog::SKIP_ALL) {
    skipped = true;
    close();
  }
  else if (is_full()) {
    show_more_lines();
  }
  else if (skip_mode == Dialog::SKIP_CURRENT) {
    show_all_now();
  }
}

/**
 * @brief This function is called when the user pressed the up or down key.
 */
void DialogBox::up_or_down_key_pressed() {

  if (dialog.is_question()
      && !has_more_lines()
      && is_full()) {

    // switch the answer
    last_answer = 1 - last_answer;
    question_dst_position.set_y(
        box_dst_position.get_y() + ((last_answer == 0) ? 27 : 40));
    Sound::play("cursor");
  }
}

/**
 * @brief Stops displaying gradually the current 3 lines, shows them
 * immediately.
 * If the 3 lines were already finished, the next group of 3 lines starts
 * (if any).
 */
void DialogBox::show_all_now() {

  if (is_full()) {
    show_more_lines();
  }
  else {

    // check the end of the current line
    while (!is_full()) {

      while (!is_full() && char_index >= lines[line_index].size()) {
        char_index = 0;
        line_index++;
      }
      if (!is_full()) {
        add_character();
      }
    }
  }
}

/**
 * @brief Returns the id of the current dialog.
 * @return the id of the dialog currently shown
 */
const std::string& DialogBox::get_dialog_id() {
  return dialog_id;
}

/**
 * @brief Returns whether all 3 current lines of the dialog box are entirely
 * displayed.
 * @return true if the dialog box is full
 */
bool DialogBox::is_full() {

  return line_index >= nb_visible_lines - 1
      && char_index >= lines[nb_visible_lines - 1].size();
}

/**
 * @brief Returns whether there are more lines remaining to display after the
 * current 3 lines.
 * @return true if there are more lines
 */
bool DialogBox::has_more_lines() {

  return line_it != dialog.get_lines().end();
}

/**
 * @brief Updates the dialog box.
 *
 * This function is called repeatedly by the game
 * while the dialog box exists.
 */
void DialogBox::update() {

  if (!is_enabled()) {
    return; // nothing to update
  }

  // update the text displaying
  update_lines();

  // handle the end of the visible lines
  if (is_full()) {

    // update the message end arrow
    end_lines_sprite.update();

    // show the appropriate action icon
    KeysEffect& keys_effect = game.get_keys_effect();
    if (!end_lines_sprite.is_animation_started()) {

      if (has_more_lines()
          || dialog.has_next()
          || dialog.is_question()) {
        end_lines_sprite.set_current_animation("next");
        keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NEXT);
      }
      else {
        keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_RETURN);
        end_lines_sprite.set_current_animation("last");
      }

      keys_effect.set_sword_key_effect(KeysEffect::SWORD_KEY_HIDDEN);
      Sound::play("message_end");
    }
  }
}

/**
 * @brief Displays the dialog box on a surface.
 * @param destination_surface the surface
 */
void DialogBox::display(Surface* destination_surface) {

  int x = box_dst_position.get_x();
  int y = box_dst_position.get_y();

  dialog_surface.fill_with_color(Color::get_black());

  if (style == STYLE_WITHOUT_FRAME) {
    // display a dark rectangle
    destination_surface->fill_with_color(Color::get_black(),
        box_dst_position);
  }
  else {
    // display the dialog box
    box_img.blit(box_src_position, &dialog_surface, box_dst_position);
  }

  // display the text
  for (int i = 0; i < nb_visible_lines; i++) {
    line_surfaces[i]->display(&dialog_surface);
  }

  // display the icon
  if (icon_number != -1) {
    Rectangle src_position(0, 0, 16, 16);
    src_position.set_xy(16 * (icon_number % 10), 16 * (icon_number / 10));
    icons_img.blit(src_position, &dialog_surface, icon_dst_position);

    question_dst_position.set_x(x + 50);
  }
  else {
    question_dst_position.set_x(x + 18);
  }

  // display the question arrow
  if (dialog.is_question()
      && is_full()
      && !has_more_lines()) {
    box_img.blit(question_src_position, &dialog_surface, question_dst_position);
  }

  // display the end message arrow
  if (is_full()) {
    end_lines_sprite.display(&dialog_surface, x + 103, y + 56);
  }

  // final blit
  dialog_surface.blit(destination_surface);
}

/**
 * @brief Updates displaying the characters in the 3 lines.
 */
void DialogBox::update_lines() {

  uint32_t now = System::now();
  while (!is_full() && now >= next_char_date) {

     // check the end of the current line
     while (!is_full() && char_index >= lines[line_index].size()) {
       char_index = 0;
       line_index++;
     }

     if (!is_full()) {
       add_character();
     }
   }
}

/**
 * @brief Adds the next character to the message.
 *
 * If this is a special character (like $0, $v, etc.),
 * the corresponding action is performed.
 */
void DialogBox::add_character() {

  unsigned char current_char = lines[line_index][char_index++];

  /*
   * Special characters:
   * - $1, $2 and $3: slow, medium and fast
   * - $0: pause
   * - $v: variable
   * - space: don't add the delay
   * - 110xxxx: multibyte character
   */

  bool special = false;
  if (current_char == '$') {
    // special character

    special = true;
    current_char = lines[line_index][char_index++];

    switch (current_char) {

    case '0':
      // pause
      next_char_date += 1000;
      break;

    case '1':
      // slow
      char_delay = char_delays[SPEED_SLOW];
      break;

    case '2':
      // medium
      char_delay = char_delays[SPEED_MEDIUM];
      break;

    case '3':
      // fast
      char_delay = char_delays[SPEED_FAST];
      break;

    case 'v':
    {
      char_index -= 2;
      lines[line_index] = lines[line_index].replace(char_index, 2, get_variable());
      break;
    }

    default:
      // not a special char, actually
      line_surfaces[line_index]->add_char('$');
      special = false;
    }
  }

  if (!special) {
    // normal character
    line_surfaces[line_index]->add_char(current_char);

    // if this is a multibyte character, also add the next byte
    if ((current_char & 0xE0) == 0xC0) {
      // the first byte is 110xxxxx: this means the character is stored with two bytes (utf-8)

      current_char = lines[line_index][char_index++];
      line_surfaces[line_index]->add_char(current_char);
    }

    if (current_char != ' ') {
      next_char_date += char_delay;
    }

    uint32_t now = System::now();
    if (now >= next_sound_date && is_letter_sound_enabled()) {
      Sound::play("message_letter");
      next_sound_date = now + 100;
    }
  }
}
