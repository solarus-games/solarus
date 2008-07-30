#ifndef ZSDX_DIALOG_BOX_H
#define ZSDX_DIALOG_BOX_H

#include "Common.h"

/**
 * A dialog box where a succession of messages can be displayed.
 */
class DialogBox {

 private:
  
  Message *current_message;

  static SDL_Surface *img_box;
  Sprite *sprite_message_end_arrow;

 public:

  // creation and destruction
  DialogBox(MessageId first_message_id);
  ~DialogBox(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
