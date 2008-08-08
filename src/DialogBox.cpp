#include "DialogBox.h"
#include "Message.h"
#include "FileTools.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "KeysEffect.h"
#include "GameResource.h"
#include "Sound.h"

SDL_Surface * DialogBox::img_box = NULL;
Sound * DialogBox::end_message_sound = NULL;

static SDL_Rect box_src_position = {0, 0, 220, 60};
static SDL_Rect box_dst_position = {51, 144, 0, 0};

/**
 * Creates a new dialog box.
 * @param first_message_id id of the message to show
 * (it can be followed by other messages)
 */
DialogBox::DialogBox(MessageId first_message_id) {

  // first instance: load the image and the sound
  if (img_box == NULL) {
    img_box = FileTools::open_image("hud/dialog_box.png");
    end_message_sound = zsdx->game_resource->get_sound("message_end");
  }

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
 * in a next message (with the '$v' sequence).
 * You can specify several variables by calling this
 * function more than once. The first '$v' will be
 * substituted by the first value specified, and so on.
 * @param value the value to add
 */
void DialogBox::add_variable(string value) {
  variables.push_back(value);
}

/**
 * Specifies the value of a variable that will occur
 * in a next message (with the '$v' sequence).
 * You can specify several variables by calling this
 * function more than once. The first '$v' will be
 * substituted by the first value specified, and so on.
 * This function just converts the int value to a string
 * add calls add_variable(string).
 * @param value the value to add
 */
void DialogBox::add_variable(int value) {
  ostringstream oss;
  oss << value;
  add_variable(oss.str());
}

/**
 * Returns the next variable specified by a previous
 * call to add_variable(). This function is called by
 * the message when it reads the '$v' sequence.
 * The number of '$v' must be equal to the number
 * of add_variable() previous calls.
 */
string DialogBox::get_variable(void) {

  if (variables.empty()) {
    DIE("Missing variable in message");
  }

  string variable = variables.front();
  variables.erase(variables.begin());
  return variable;
}

/**
 * Shows a new message in the dialog box.
 * @param message_id id of the message to create (must be a valid id)
 */
void DialogBox::show_message(MessageId message_id) {

  // create the message
  current_message = new Message(this, message_id);
  
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
  }
  else if (current_message->is_over()) {
    action_key_pressed();
  }
  else if (cancel_mode == DIALOG_CANCEL_CURRENT) {
    current_message->show_all_now();
  }
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

  // display the end message arrow
  if (current_message->is_over()) {
    sprite_message_end_arrow->display(destination_surface, 154, 200);
  }
}
