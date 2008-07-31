#ifndef ZSDX_MESSAGE_H
#define ZSDX_MESSAGE_H

#include "Common.h"

/**
 * A message displayed in the dialog box.
 */
class Message {

 private:

  // the dialog box where this message is displayed
  DialogBox *dialog_box;

  // the 3 lines of the message
  string lines[3];
  TextDisplayed *text_surfaces[3];

  // the icon displayed, or NULL if there is no icon
  SDL_Surface *icon;

  // is this message a question?
  bool question;
  bool first_answer;

  // id of the next message (or an empty string if this is the last message)
  MessageId next_message_id;
  MessageId next_message_id_2;

 public:

  // creation and destruction
  Message(DialogBox *dialog_box, MessageId message_id);
  ~Message(void);

  // message properties
  MessageId get_next_message_id(void);

  // message current state
  bool is_over(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);

};

#endif
