#include "DialogBox.h"
#include "Message.h"
#include "FileTools.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"

bool DialogBox::answer_1_selected = true;

static SDL_Rect box_src_position = {0, 0, 220, 60};
static SDL_Rect box_dst_position = {51, 144, 0, 0};

static SDL_Rect question_src_position = {48, 60, 8, 8};
static SDL_Rect question_dst_position = {69, 171, 0, 0};

static SDL_Rect icon_dst_position = {69, 166, 0, 0};

/**
 * Creates a new dialog box.
 * @param first_message_id id of the message to show
 * (it can be followed by other messages)
 */
DialogBox::DialogBox(MessageId first_message_id) {

  // load the images and the sounds
  img_box = ResourceManager::load_image("hud/dialog_box.png");
  img_icons = ResourceManager::load_image("hud/message_icons.png");

  end_message_sound = ResourceManager::get_sound("message_end");
  switch_answer_sound = ResourceManager::get_sound("cursor");

  // create the sprite
  sprite_message_end_arrow = new Sprite("hud/dialog_box_message_end");

  // initialize the current message
  speed = DIALOG_SPEED_FAST;
  cancel_mode = DIALOG_CANCEL_NONE;
  show_message(first_message_id);
  cancel_dialog = false;

  // save the action and sword keys
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->save_action_key_effect();
  keys_effect->save_sword_key_effect();
}

/**
 * Destructor.
 */
DialogBox::~DialogBox(void) {
  SDL_FreeSurface(img_box);
  SDL_FreeSurface(img_icons);
  delete sprite_message_end_arrow;
  delete current_message;
}

/**
 * Returns the speed of the text.
 * @return the speed
 */
DialogSpeed DialogBox::get_speed(void) {
  return speed;
}

/**
 * Sets the speed of the text.
 * @param speed the new speed
 */
void DialogBox::set_speed(DialogSpeed speed) {
  this->speed = speed;
}

/**
 * Sets the cancel mode of the dialog box.
 * @param cancel_mode the new cancel mode
 */
void DialogBox::set_cancel_mode(DialogCancelMode cancel_mode) {
  this->cancel_mode = cancel_mode;
}

/**
 * Returns the cancel mode of the dialog box.
 * @return cancel_mode the cancel mode
 */
DialogCancelMode DialogBox::get_cancel_mode(void) {
  return cancel_mode;
}

/**
 * Specifies the value of a variable that will occur
 * in the specified message (with the '$v' sequence).
 * You can specify several variables, but only one per message.
 * If a variable was already specified for this message, it is replaced.
 * @param message_id id of the message where this value will appear
 * @param value the value to add
 */
void DialogBox::set_variable(MessageId message_id, string value) {

  variables[message_id] = value;
}

/**
 * Same thing as add_variable(MessageId, string) but with an integer parameter.
 * This function just converts the integer value to a string
 * add calls the other function.
 * @param message_id id of the message where this value will appear
 * @param value the value to add
 */
void DialogBox::set_variable(MessageId message_id, int value) {
  ostringstream oss;
  oss << value;
  set_variable(message_id, oss.str());
}

/**
 * Returns the variable specified by a previous
 * call to add_variable(), for the current message.
 * This function is called by
 * the message when it reads the '$v' sequence.
 */
string DialogBox::get_variable(void) {

  string value = variables[current_message_id];

  if (value == "") {
    DIE("Missing variable in message '" << current_message_id << "'");
  }
  
  return value;
}

/**
 * Shows a new message in the dialog box.
 * @param message_id id of the message to create (must be a valid id)
 */
void DialogBox::show_message(MessageId message_id) {

  // create the message
  current_message = new Message(this, message_id);
  current_message_id = message_id;

  if (current_message->is_question()) {
    answer_1_selected = true;
  }
  question_dst_position.y = 171;
  
  // hide the action icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(ACTION_KEY_NONE);

  if (get_cancel_mode() != DIALOG_CANCEL_NONE) {
    keys_effect->set_sword_key_effect(SWORD_KEY_SKIP);
  }
  else {
    keys_effect->set_sword_key_effect(SWORD_KEY_NONE);
  }
}

/**
 * This function is called when the user presses the action key.
 */
void DialogBox::action_key_pressed(void) {

  if (current_message->is_over()) { // the current message is over

    // show the next message (if any)
    MessageId next_message_id = current_message->get_next_message_id();
    delete current_message;

    if (next_message_id != "") {
      show_message(next_message_id);
    }
    else {
      current_message = NULL;
    }

    if (is_over()) { // the last message of the dialog is over
      // restore the action and sword keys
      KeysEffect *keys_effect = zsdx->game->get_keys_effect();
      keys_effect->restore_action_key_effect();
      keys_effect->restore_sword_key_effect();
    }
  }
}

/**
 * This function is called when the user presses the sword key.
 */
void DialogBox::sword_key_pressed(void) {

  if (cancel_mode == DIALOG_CANCEL_ALL) {
    cancel_dialog = true;
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    keys_effect->restore_action_key_effect();
    keys_effect->restore_sword_key_effect();
  }
  else if (current_message->is_over()) {
    action_key_pressed();
  }
  else if (cancel_mode == DIALOG_CANCEL_CURRENT) {
    current_message->show_all_now();
  }
}

/**
 * This function is called when the user pressed the up or down arrow key.
 */
void DialogBox::up_or_down_key_pressed(void) {

  if (current_message->is_question() && current_message->is_over()) {
    
    // switch the anser
    answer_1_selected = !answer_1_selected;
    question_dst_position.y = answer_1_selected ? 171 : 184;
    switch_answer_sound->play();
  }
}

/**
 * For a message with a question, returns true if the player
 * has selected the first answer or false if he has selected
 * the second one.
 * @return the answer selected
 */
bool DialogBox::was_answer_1_selected(void) {
  return answer_1_selected;
}

/**
 * Returns whether the dialog box has to be closed, i.e.
 * whether the last message was shown and the
 * user has pressed the key, or the dialog was cancelled.
 */
bool DialogBox::is_over(void) {
  return current_message == NULL || cancel_dialog;
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
  if (current_message->is_over()) {

    // update the message end arrow
    sprite_message_end_arrow->update_current_frame();

    // show the appropriate action icon
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    ActionKeyEffect action_key_effect = keys_effect->get_action_key_effect();
    if (action_key_effect != ACTION_KEY_NEXT
	&& action_key_effect != ACTION_KEY_RETURN) {

      MessageId next_message_id = current_message->get_next_message_id();
      if (next_message_id != "") {
	keys_effect->set_action_key_effect(ACTION_KEY_NEXT);
      }
      else {
	keys_effect->set_action_key_effect(ACTION_KEY_RETURN);
      }

      keys_effect->set_sword_key_effect(SWORD_KEY_NONE);
      end_message_sound->play();
    }
  }
}

/**
 * Displays the dialog box on a surface.
 * @param destination_surface the surface
 */
void DialogBox::display(SDL_Surface *destination_surface) {

  // display the dialog box
  SDL_BlitSurface(img_box, &box_src_position, destination_surface, &box_dst_position);

  // display the message
  current_message->display(destination_surface);

  // display the icon
  int icon_number = current_message->get_icon_number();
  if (icon_number != -1) {
    SDL_Rect src_position = {0, 0, 16, 16};
    src_position.x = icon_number % 10;
    src_position.y = icon_number / 10;
    SDL_BlitSurface(img_icons, &src_position, destination_surface, &icon_dst_position);
  }

  // display the question arrow
  if (current_message->is_question() && current_message->is_over()) {
    SDL_BlitSurface(img_box, &question_src_position, destination_surface, &question_dst_position);
  }

  // display the end message arrow
  if (current_message->is_over()) {
    sprite_message_end_arrow->display(destination_surface, 154, 200);
  }
}
