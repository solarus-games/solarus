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
#include "DialogBox.h"
#include "Message.h"
#include "Sprite.h"
#include "Game.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "MapScript.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Color.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"

DialogBox::Style DialogBox::style = DialogBox::WITH_FRAME;
static Rectangle box_src_position(0, 0, 220, 60);
static Rectangle question_src_position(96, 60, 8, 8);

/**
 * Creates a new dialog box.
 * @param game the game this dialog box belongs to
 * @param first_message_id id of the message to show
 * (it can be followed by other messages)
 * @param x x coordinate of the top-left corner of the box
 * @param y y coordinate of the top-left corner of the box
 */
DialogBox::DialogBox(Game *game, const MessageId &first_message_id, int x, int y):
  game(game) {

  // initialize the surface
  dialog_surface = new Surface(320, 240);
  dialog_surface->set_transparency_color(Color::get_black());

  if (style != WITHOUT_FRAME) {
    dialog_surface->set_opacity(216);
  }

  // load the images
  img_box = ResourceManager::load_image("hud/dialog_box.png");
  img_icons = ResourceManager::load_image("hud/message_and_treasure_icons.png");

  this->x = x;
  this->y = y;
  if (style == WITHOUT_FRAME) {
    if (this->y < 120) {
      this->y -= 24;
    }
    else {
      this->y += 24;
    }
  }

  box_dst_position.set_xy(this->x, this->y);
  box_dst_position.set_size(box_src_position);
  question_dst_position.set_xy(x + 18, y + 27);
  icon_dst_position.set_xy(x + 18, y + 22);

  // load the sounds
  end_message_sound = ResourceManager::get_sound("message_end");
  switch_answer_sound = ResourceManager::get_sound("cursor");

  // create the sprites
  end_message_sprite = new Sprite("hud/dialog_box_message_end");

  // save the action and sword keys
  KeysEffect *keys_effect = game->get_keys_effect();
  action_key_effect_saved = keys_effect->get_action_key_effect();
  sword_key_effect_saved = keys_effect->get_sword_key_effect();

  // initialize the current message
  speed = SPEED_FAST;
  skip_mode = SKIP_NONE;
  icon_number = -1;
  this->first_message_id = first_message_id;
  show_message(first_message_id);
  skipped = false;
}

/**
 * Destructor.
 */
DialogBox::~DialogBox(void) {

  // the dialog box is being closed: restore the action and sword keys
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(action_key_effect_saved);
  keys_effect->set_sword_key_effect(sword_key_effect_saved);

  // free the memory
  delete dialog_surface;
  delete img_box;
  delete img_icons;
  delete end_message_sprite;
  delete current_message;
}
    
/**
 * Sets the dialog box style for all subsequent dialogs.
 * The default style is DialogBox::WITH_FRAME.
 * @param style the new style to set
 */
void DialogBox::set_style(Style style) {
  DialogBox::style = style;
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
  return style != WITHOUT_FRAME;
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
 * Shows a new message in the dialog box.
 * @param message_id id of the message to create (must be a valid id)
 */
void DialogBox::show_message(const MessageId &message_id) {

  // create the message
  current_message = new Message(this, message_id, x, y);
  current_message_id = message_id;

  if (current_message->is_question()) {
    game->set_dialog_last_answer(0);
  }
  else {
    game->set_dialog_last_answer(-1);
  }
  question_dst_position.set_y(y + 27);
  
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
  delete current_message;

  if (next_message_id != "" && next_message_id != "_unknown") {
    show_message(next_message_id);
  }
  else {
    current_message = NULL;
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
    int answer = game->get_dialog_last_answer();
    game->set_dialog_last_answer(1 - answer);
    question_dst_position.set_y(y + ((answer == 1) ? 27 : 40));
    switch_answer_sound->play();
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
 * Returns whether the dialog box has to be closed, i.e.
 * whether the last message was shown and the
 * user has pressed the key, or the dialog was skipled.
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
	keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NEXT);
      }
      else {
	keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_RETURN);
	if (end_message_sprite->get_current_animation() != "last") {
	  end_message_sprite->set_current_animation("last");
	}
      }

      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_HIDDEN);
      end_message_sound->play();
    }
  }
}

/**
 * Displays the dialog box on a surface.
 * @param destination_surface the surface
 */
void DialogBox::display(Surface *destination_surface) {

  dialog_surface->fill_with_color(Color::get_black());

  if (style == WITHOUT_FRAME) {
    // display a dark rectangle
    destination_surface->fill_with_color(Color::get_black(), box_dst_position);
  }
  else {
    // display the dialog box
    img_box->blit(box_src_position, dialog_surface, box_dst_position);
  }

  // display the message
  current_message->display(dialog_surface);

  // display the icon
  if (icon_number != -1) {
    Rectangle src_position(0, 0, 16, 16);
    src_position.set_xy(16 * (icon_number % 10), 16 * (icon_number / 10));
    img_icons->blit(src_position, dialog_surface, icon_dst_position);

    question_dst_position.set_x(x + 50);
  }
  else {
    question_dst_position.set_x(x + 18);
  }

  // display the question arrow
  if (current_message->is_question() && current_message->is_finished()) {
    img_box->blit(question_src_position, dialog_surface, question_dst_position);
  }

  // display the end message arrow
  if (current_message->is_finished()) {
    end_message_sprite->display(dialog_surface, x + 103, y + 56);
  }

  // final blit
  dialog_surface->blit(destination_surface);
}

