#ifndef ZSDX_DIALOG_BOX_H
#define ZSDX_DIALOG_BOX_H

#include "Common.h"

/**
 * The possible speeds of a dialog's text.
 */
enum DialogSpeed {
  DIALOG_SPEED_SLOW,
  DIALOG_SPEED_MEDIUM,
  DIALOG_SPEED_FAST
};

/**
 * Indicates what happens when the user tries to cancel
 * the current message.
 */
enum DialogCancelMode {
  DIALOG_CANCEL_NONE,
  DIALOG_CANCEL_CURRENT,
  DIALOG_CANCEL_ALL
};

/**
 * A dialog box where a succession of messages can be displayed.
 */
class DialogBox {

 private:
  
  // current message
  Message *current_message;

  // dialog properties
  DialogSpeed speed;
  DialogCancelMode cancel_mode;

  // graphics
  static SDL_Surface *img_box;
  Sprite *sprite_message_end_arrow;

  // sounds
  static Sound *end_message_sound;

 public:

  // creation and destruction
  DialogBox(MessageId first_message_id);
  ~DialogBox(void);

  // dialog properties
  DialogSpeed get_speed(void);
  void set_speed(DialogSpeed speed);

  DialogCancelMode get_cancel_mode(void);
  void set_cancel_mode(DialogCancelMode cancel_mode);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
