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
  speed = DIALOG_SPEED_SLOW;
  cancel_mode = DIALOG_CANCEL_NONE;
  current_message = new Message(this, first_message_id);
}

/**
 * Destructor.
 */
DialogBox::~DialogBox(void) {
  delete sprite_message_end_arrow;
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
