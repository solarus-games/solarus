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
#include "FileTools.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Color.h"
#include "MapScript.h"

static SDL_Rect box_src_position = {0, 0, 220, 60};
static SDL_Rect question_src_position = {96, 60, 8, 8};

/**
 * Creates a new dialog box.
 * @param first_message_id id of the message to show
 * (it can be followed by other messages)
 * @param x x coordinate of the top-left corner of the box
 * @param y y coordinate of the top-left corner of the box
 */
DialogBox::DialogBox(const MessageId &first_message_id, int x, int y) {

  // initialize the surface
  dialog_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
  SDL_SetColorKey(dialog_surface, SDL_SRCCOLORKEY, Color::black);
  SDL_SetAlpha(dialog_surface, SDL_SRCALPHA, 216);

  // load the images
  img_box = ResourceManager::load_image("hud/dialog_box.png");
  img_icons = ResourceManager::load_image("hud/message_and_treasure_icons.png");

  this->x = x;
  this->y = y;
  box_dst_position.x = x;
  box_dst_position.y = y;
  question_dst_position.x = x + 18;
  question_dst_position.y = y + 27;
  icon_dst_position.x = x + 18;
  icon_dst_position.y = y + 22;

  // load the sounds
  end_message_sound = ResourceManager::get_sound("message_end");
  switch_answer_sound = ResourceManager::get_sound("cursor");

  // create the sprites
  end_message_sprite = new Sprite("hud/dialog_box_message_end");

  // save the action and sword keys
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  action_key_effect_saved = keys_effect->get_action_key_effect();
  sword_key_effect_saved = keys_effect->get_sword_key_effect();

  // initialize the current message
  speed = SPEED_FAST;
  cancel_mode = CANCEL_NONE;
  icon_number = -1;
  this->first_message_id = first_message_id;
  show_message(first_message_id);
  cancelled = false;
}

/**
 * Destructor.
 */
DialogBox::~DialogBox(void) {

  // the dialog box is being closed: restore the action and sword keys
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(action_key_effect_saved);
  keys_effect->set_sword_key_effect(sword_key_effect_saved);

  // free the memory
  SDL_FreeSurface(dialog_surface);
  SDL_FreeSurface(img_box);
  SDL_FreeSurface(img_icons);
  delete end_message_sprite;
  delete current_message;
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
 * Returns the cancel mode of the dialog box.
 * @return cancel_mode the cancel mode
 */
DialogBox::CancelMode DialogBox::get_cancel_mode(void) {
  return cancel_mode;
}

/**
 * Sets the cancel mode of the dialog box.
 * @param cancel_mode the new cancel mode
 */
void DialogBox::set_cancel_mode(CancelMode cancel_mode) {
  this->cancel_mode = cancel_mode;
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
    zsdx->game->set_dialog_last_answer(0);
  }
  else {
    zsdx->game->set_dialog_last_answer(-1);
  }
  question_dst_position.y = y + 27;
  
  // hide the action icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (get_cancel_mode() != CANCEL_NONE) {
    keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SKIP);
  }
  else {
    keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_HIDDEN);
  }

  // notify the script
  zsdx->game->get_current_script()->event_message_started(message_id);
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

    // show the next message (if any)
    MessageId next_message_id = current_message->get_next_message_id();
    delete current_message;

    if (next_message_id != "" && next_message_id != "_unknown") {
      show_message(next_message_id);
    }
    else {
      current_message = NULL;
    }
  }
}

/**
 * This function is called when the user presses the sword key.
 */
void DialogBox::sword_key_pressed(void) {

  if (cancel_mode == CANCEL_ALL) {
    cancelled = true;
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    keys_effect->restore_action_key_effect();
    keys_effect->restore_sword_key_effect();
  }
  else if (current_message->is_finished()) {
    action_key_pressed();
  }
  else if (cancel_mode == CANCEL_CURRENT) {
    current_message->show_all_now();
  }
}

/**
 * This function is called when the user pressed the up or down arrow key.
 */
void DialogBox::up_or_down_key_pressed(void) {

  if (current_message->is_question() && current_message->is_finished()) {
    
    // switch the answer
    int answer = zsdx->game->get_dialog_last_answer();
    zsdx->game->set_dialog_last_answer(1 - answer);
    question_dst_position.y = y + ((answer == 1) ? 27 : 40);
    switch_answer_sound->play();
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
 * user has pressed the key, or the dialog was cancelled.
 * @return true if the dialog is finished
 */
bool DialogBox::is_finished(void) {
  return current_message == NULL || cancelled;
}

/**
 * When the dialog box is finished, returns whether
 * it was cancelled.
 * @return true if the dialog was cancelled
 */
bool DialogBox::was_cancelled(void) {
  return cancelled;
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
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
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
void DialogBox::display(SDL_Surface *destination_surface) {

  SDL_FillRect(dialog_surface, NULL, Color::black);

  // display the dialog box
  SDL_BlitSurface(img_box, &box_src_position, dialog_surface, &box_dst_position);

  // display the message
  current_message->display(dialog_surface);

  // display the icon
  if (icon_number != -1) {
    SDL_Rect src_position = {0, 0, 16, 16};
    src_position.x = 16 * (icon_number % 10);
    src_position.y = 16 * (icon_number / 10);
    SDL_BlitSurface(img_icons, &src_position, dialog_surface, &icon_dst_position);

    question_dst_position.x = x + 50;
  }
  else {
    question_dst_position.x = x + 18;
  }

  // display the question arrow
  if (current_message->is_question() && current_message->is_finished()) {
    SDL_BlitSurface(img_box, &question_src_position, dialog_surface, &question_dst_position);
  }

  // display the end message arrow
  if (current_message->is_finished()) {
    end_message_sprite->display(dialog_surface, x + 103, y + 56);
  }

  // final blit
  SDL_BlitSurface(dialog_surface, NULL, destination_surface, NULL);
}

