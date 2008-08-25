#ifndef ZSDX_MESSAGE_H
#define ZSDX_MESSAGE_H

#include "Common.h"
#include "DialogBox.h"

/**
 * A message displayed in a dialog box.
 * This class parses the message from a data file
 * and displays it in a dialog box.
 */
class Message {

 private:

  // the dialog box where this message is displayed
  DialogBox *dialog_box;

  // properties of the message

  string lines[3];             // the 3 lines of the message
  TextSurface *text_surfaces[3];
  SDL_Surface *icon;           // the icon displayed, or NULL if there is no icon
  bool question;               // is this message a question?
  MessageId next_message_id;   // id of the next message (or an empty string if this is the last message)
  MessageId next_message_id_2;

  void parse(MessageId message_id);
  void set_variable(string value);

  // current state of the display
  
  unsigned int line_index;     // line currently displayed (0 to 2)
  unsigned int char_index;     // index of the next character to show
  Uint32 delay;
  Uint32 next_char_date;
  bool show_all;

  void update_char_delay(void);
  void add_character(void);

 public:

  // creation and destruction
  Message(DialogBox *dialog_box, MessageId message_id);
  ~Message(void);

  // message properties
  bool is_question(void);
  MessageId get_next_message_id(void);

  // message current state
  bool is_over(void);
  void show_all_now(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);

};

#endif
