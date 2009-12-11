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

  std::string lines[3];        // the 3 lines of the message
  TextSurface *text_surfaces[3];
  bool question;               // is this message a question?
  MessageId next_message_id;   // id of the next message (or an empty string if this is the last message)
  MessageId next_message_id_2;

  int x;
  int y;

  void parse(MessageId message_id);
  void set_variable(const std::string &value);

  // current state of the display
  
  unsigned int line_index;     // line currently displayed (0 to 2)
  unsigned int char_index;     // index of the next character to show
  uint32_t delay;
  uint32_t next_char_date;
  bool show_all;

  uint32_t next_sound_date;

  void update_char_delay(void);
  void add_character(void);

 public:

  // creation and destruction
  Message(DialogBox *dialog_box, MessageId message_id, int x, int y);
  ~Message(void);

  // message properties
  bool is_question(void);
  MessageId get_next_message_id(void);

  // message current state
  bool is_finished(void);
  void show_all_now(void);

  // update and display
  void update(void);
  void display(Surface *destination_surface);

};

#endif

