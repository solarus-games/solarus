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
#include "DialogBox.h"
#include "Message.h"
#include "Sprite.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "MapScript.h"
#include "entities/Hero.h"
#include "lowlevel/Color.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"

/**
 * Creates a new dialog box.
 * @param game the game this dialog box belongs to
 */
DialogBox::DialogBox(Game *game):
  game(game), current_message(NULL) {

  // initialize the surface
  dialog_surface = new Surface(320, 240);
  dialog_surface->set_transparency_color(Color::get_black());

  // load the images
  box_img = new Surface("hud/dialog_box.png");
  icons_img = new Surface("hud/message_and_treasure_icons.png");
  box_src_position = Rectangle(0, 0, 220, 60);
  box_dst_position.set_size(box_src_position);
  question_src_position = Rectangle(96, 60, 8, 8);

  // create the sprites
  end_message_sprite = new Sprite("hud/dialog_box_message_end");

  set_style(STYLE_WITH_FRAME);
}

/**
 * Destructor.
 */
DialogBox::~DialogBox(void) {

  // free the memory
  delete dialog_surface;
  delete box_img;
  delete icons_img;
  delete end_message_sprite;
  delete current_message;
}

/**
 * Returns the game where this dialog box is displayed.
 * @return the current game
 */
Game * DialogBox::get_game(void) {
  return game;
}

/**
 * Returns whether the dialog box is currently active.
 * @return true if the dialog box is enabled
 */
bool DialogBox::is_enabled(void) {
  return current_message != NULL;
}

/**
 * Sets the dialog box style for all subsequent dialogs.
 * The default style is DialogBox::STYLE_WITH_FRAME.
 * @param style the new style to set
 */
void DialogBox::set_style(Style style) {

  this->style = style;

  if (style != STYLE_WITHOUT_FRAME) {
    dialog_surface->set_opacity(216);
  }
}

/**
 * Sets the vertical position of the dialog box.
 * @param vertical_position the vertical position
 */
void DialogBox::set_vertical_position(VerticalPosition vertical_position) {

  if (vertical_position == POSITION_AUTO) {
    // determine the position
    const Rectangle &camera_position = game->get_current_map()->get_camera_position();
    vertical_position = POSITION_BOTTOM;

    if (game->get_hero()->get_y() >= camera_position.get_y() + 130) {
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
 * Returns the speed of the text.
 * @return the speed
 */
DialogBox::Speed DialogBox::get_speed(void) {
  return speed;
}

/**
 * Sets the speed of the text.
 * @param speed the new speed
 */
void DialogBox::set_speed(Speed speed) {
  this->speed = speed;
}

/**
 * Returns the current skip mode of the dialog box.
 * @return the skip mode
 */
DialogBox::SkipMode DialogBox::get_skip_mode(void) {
  return skip_mode;
}

/**
 * Sets the skip mode of the dialog box.
 * @param skip_mode the new skip mode
 */
void DialogBox::set_skip_mode(SkipMode skip_mode) {
  this->skip_mode = skip_mode;
}

/**
 * Returns the index of the icon displayed in this dialog box,
 * or -1 if there is no icon.
 * @return the icon number
 */
int DialogBox::get_icon_number(void) {
  return icon_number;
}

/**
 * Sets the index of the icon displayed in this dialog box,
 * @param icon_number the icon number, or -1 to remove a previous icon
 */
void DialogBox::set_icon_number(int icon_number) {
  this->icon_number = icon_number;
}

/**
 * Returns whether a sound should be played when displaying the letters.
 * @return true if a sound should be played when displaying the letters
 */
bool DialogBox::is_letter_sound_enabled(void) {
  return style != STYLE_WITHOUT_FRAME;
}

/**
 * Specifies the value of a variable that will occur
 * in the specified sequence of messages (with the '$v' sequence).
 * You can specify only one variable at the same time per message sequence.
 * If a variable was already specified for this sequence of messages, it is replaced.
 * @param first_message_id id of the first message of the sequence where this value will appear
 * @param value the value to add
 */
void DialogBox::set_variable(const MessageId &first_message_id, const std::string &value) {
  variables[first_message_id] = value;
}

/**
 * Same thing as set_variable(MessageId, string) but with an integer parameter.
 * This function just converts the integer value to a string
 * add calls the other function.
 * @param first_message_id id of the first message of the sequence where this value will appear
 * @param value the value to set
 */
void DialogBox::set_variable(const MessageId &first_message_id, int value) {
  std::ostringstream oss;
  oss << value;
  set_variable(first_message_id, oss.str());
}

/**
 * Returns the variable specified by a previous
 * call to set_variable(), for the current sequence of messages.
 * This function is called by
 * the current message when it reads the '$v' sequence.
 */
const std::string& DialogBox::get_variable(void) {

  const std::string &value = variables[first_message_id];

  if (value == "") {
    DIE("Missing variable in message '" << current_message_id << "'");
  }

  return value;
}

/**
 * Returns the answer selected by the player in the last message with a question.
 * @return the answer selected: 0 for the first one, 1 for the second one,
 * -1 if the last dialog was not a question
 */
int DialogBox::get_last_answer(void) {
  return last_answer;
}

/**
 * Remembers the answer selected by the player in the last dialog with a question.
 * @param answer the answer selected: 0 for the first one, 1 for the second one,
 * -1 if the last dialog was not a question
 */
void DialogBox::set_last_answer(int answer) {

  if (answer < -1 || answer > 1) {
    DIE("Invalid value of answer: " << answer);
  }
  this->last_answer = answer;
}

/**
 * Starts a sequence of messages.
 * The dialog box should not be enabled already when you call this function.
 * @param first_message_id id of the first message of the sequence
 * @param vertical_position vertical position where to display the dialog box (default: auto)
 */
void DialogBox::start_message_sequence(const MessageId &first_message_id, VerticalPosition vertical_position) {

  if (is_enabled()) {
    DIE("Cannot start message sequence '" << first_message_id << ": the dialog box is already enabled");
  }

  // save the action and sword keys
  KeysEffect *keys_effect = game->get_keys_effect();
  action_key_effect_saved = keys_effect->get_action_key_effect();
  sword_key_effect_saved = keys_effect->get_sword_key_effect();

  // initialize the dialog box with the default parameters
  set_vertical_position(vertical_position);
  set_speed(SPEED_FAST);
  set_skip_mode(SKIP_NONE);
  set_icon_number(-1);
  this->skipped = false;
  this->first_message_id = first_message_id;
  show_message(first_message_id);
}

/**
 * Shows a new message in the dialog box.
 * @param message_id id of the message to create (must be a valid id)
 */
void DialogBox::show_message(const MessageId &message_id) {

  // create the message
  delete current_message;
  current_message = new Message(this, message_id, box_dst_position.get_x(), box_dst_position.get_y());
  current_message_id = message_id;

  if (current_message->is_question()) {
    set_last_answer(0);
  }
  else {
    set_last_answer(-1);
  }
  question_dst_position.set_y(box_dst_position.get_y() + 27);
  
  // hide the action icon
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (get_skip_mode() != SKIP_NONE) {
    keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SKIP);
  }
  else {
    keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_HIDDEN);
  }

  // notify the script
  game->get_current_script()->event_message_started(message_id);
}

/** 
 * Shows the next message (if any).
 */
void DialogBox::show_next_message() {

  MessageId next_message_id = current_message->get_next_message_id();

  if (next_message_id != "" && next_message_id != "_unknown") {
    show_message(next_message_id);
  }
  else {
    close();
  }
}

/**
 * Closes the dialog box.
 */
void DialogBox::close(void) {

  // the dialog box is being closed
  delete current_message;
  current_message = NULL;

  // restore the action and sword keys
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(action_key_effect_saved);
  keys_effect->set_sword_key_effect(sword_key_effect_saved);

  // notify the script if necessary
  if (!skipped && first_message_id[0] != '_') {
    // a dialog of the quest was just finished: notify the script
    game->get_current_script()->event_message_sequence_finished(first_message_id, last_answer);
  }
}

/**
 * This function is called by the game when a key is pressed
 * while displaying a message.
 * @param key the key pressed
 */
void DialogBox::key_pressed(Controls::GameKey key) {

  switch (key) {

    // action key
    case Controls::ACTION:
      action_key_pressed();
      break;

      // sword key
    case Controls::SWORD:
      sword_key_pressed();
      break;

    case Controls::UP:
    case Controls::DOWN:
      up_or_down_key_pressed();

    default:
      break;
  }
}

/**
 * This function is called when the user presses the action key.
 */
void DialogBox::action_key_pressed(void) {

  if (current_message->is_finished()) { // the current message is over
    show_next_message();
  }
}

/**
 * This function is called when the user presses the sword key.
 */
void DialogBox::sword_key_pressed(void) {

  if (skip_mode == SKIP_ALL) {
    skipped = true;
    close();
  }
  else if (current_message->is_finished()) {
    show_next_message();
  }
  else if (skip_mode == SKIP_CURRENT) {
    show_all_now();
  }
}

/**
 * This function is called when the user pressed the up or down arrow key.
 */
void DialogBox::up_or_down_key_pressed(void) {

  if (current_message->is_question() && current_message->is_finished()) {

    // switch the answer
    int answer = get_last_answer();
    set_last_answer(1 - answer);
    question_dst_position.set_y(box_dst_position.get_y() + ((answer == 1) ? 27 : 40));
    game->play_sound("cursor");
  }
}

/**
 * Shows immediately this message up to the end.
 * If the message was already finished, the next message starts.
 */
void DialogBox::show_all_now(void) {

  if (current_message->is_finished()) {
    show_next_message();
  }

  if (current_message != NULL) {
    current_message->show_all_now();
  }
}

/**
 * Returns the id of the first message shown in the current dialog box sequence.
 * @return the id of the first message shown
 */
MessageId DialogBox::get_first_message_id(void) {
  return first_message_id;
}

/**
 * Returns whether the dialog box has been closed, i.e.
 * whether the last message was shown and the
 * user has pressed the key, or the dialog was skiped.
 * @return true if the dialog is finished
 */
bool DialogBox::is_finished(void) {
  return current_message == NULL || skipped;
}

/**
 * When the dialog box is finished, returns whether
 * it was skipped.
 * @return true if the dialog was skipped
 */
bool DialogBox::was_skipped(void) {
  return skipped;
}

/**
 * Updates the dialog box.
 * This function is called repeatedly by the game
 * while the dialog box exists.
 */
void DialogBox::update(void) {

  if (current_message == NULL) {
    return; // nothing to update
  }

  // update the text displaying
  current_message->update();

  // handle the end of the message
  if (current_message->is_finished()) {

    // update the message end arrow
    end_message_sprite->update();

    // show the appropriate action icon
    KeysEffect *keys_effect = game->get_keys_effect();
    KeysEffect::ActionKeyEffect action_key_effect = keys_effect->get_action_key_effect();
    if (action_key_effect != KeysEffect::ACTION_KEY_NEXT
	&& action_key_effect != KeysEffect::ACTION_KEY_RETURN) {

      MessageId next_message_id = current_message->get_next_message_id();
      if (next_message_id != "" || current_message->is_question()) {
	if (end_message_sprite->get_current_animation() != "next") {
	  end_message_sprite->set_current_animation("next");
	}
	keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NEXT);
      }
      else {
	keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_RETURN);
	if (end_message_sprite->get_current_animation() != "last") {
	  end_message_sprite->set_current_animation("last");
	}
      }

      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_HIDDEN);
      game->play_sound("message_end");
    }
  }
}

/**
 * Displays the dialog box on a surface.
 * @param destination_surface the surface
 */
void DialogBox::display(Surface *destination_surface) {

  int x = box_dst_position.get_x();
  int y = box_dst_position.get_y();

  dialog_surface->fill_with_color(Color::get_black());

  if (style == STYLE_WITHOUT_FRAME) {
    // display a dark rectangle
    destination_surface->fill_with_color(Color::get_black(), box_dst_position);
  }
  else {
    // display the dialog box
    box_img->blit(box_src_position, dialog_surface, box_dst_position);
  }

  // display the message
  current_message->display(dialog_surface);

  // display the icon
  if (icon_number != -1) {
    Rectangle src_position(0, 0, 16, 16);
    src_position.set_xy(16 * (icon_number % 10), 16 * (icon_number / 10));
    icons_img->blit(src_position, dialog_surface, icon_dst_position);

    question_dst_position.set_x(x + 50);
  }
  else {
    question_dst_position.set_x(x + 18);
  }

  // display the question arrow
  if (current_message->is_question() && current_message->is_finished()) {
    box_img->blit(question_src_position, dialog_surface, question_dst_position);
  }

  // display the end message arrow
  if (current_message->is_finished()) {
    end_message_sprite->display(dialog_surface, x + 103, y + 56);
  }

  // final blit
  dialog_surface->blit(destination_surface);
}

