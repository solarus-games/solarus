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
#ifndef ZSDX_DIALOG_BOX_H
#define ZSDX_DIALOG_BOX_H

#include "Common.h"
#include "KeysEffect.h"
#include "Controls.h"
#include <map>
#include <vector>

/**
 * A dialog box where a succession of messages can be displayed.
 * This class displays the dialog box and handles its properties:
 * the text speed and the state of the action and sword keys.
 */
class DialogBox {

 public:

  /**
   * The possible speeds of a dialog's text.
   */
  enum Speed {
    SPEED_SLOW,
    SPEED_MEDIUM,
    SPEED_FAST
  };

  /**
   * Indicates what happens when the user tries to cancel
   * the current message.
   */
  enum CancelMode {
    CANCEL_NONE,
    CANCEL_CURRENT,
    CANCEL_ALL
  };

 private:
  
  // current message
  Message *current_message;
  MessageId current_message_id;
  std::map<MessageId, std::string> variables;

  MessageId first_message_id;
  static int answer_selected;

  // dialog properties
  Speed speed;
  CancelMode cancel_mode;
  bool cancel_dialog;          // true if the user has cancelled the dialog
  int icon_number;             // index of the 16*16 icon displayed, or -1 if there is no icon

  KeysEffect::ActionKeyEffect action_key_effect_saved;
  KeysEffect::SwordKeyEffect sword_key_effect_saved;

  // graphics
  SDL_Surface *dialog_surface;
  SDL_Surface *img_box;
  SDL_Surface *img_icons;
  Sprite *sprite_message_end_arrow;

  // sounds
  Sound *end_message_sound;
  Sound *switch_answer_sound;

  // position of the images
  int x;
  int y;
  SDL_Rect box_dst_position;
  SDL_Rect question_dst_position;
  SDL_Rect icon_dst_position;

  void show_message(MessageId messageId);

  void action_key_pressed(void);
  void sword_key_pressed(void);
  void up_or_down_key_pressed(void);

 public:

  // creation and destruction
  DialogBox(MessageId first_message_id, int x, int y);
  ~DialogBox(void);

  // dialog properties
  Speed get_speed(void);
  void set_speed(Speed speed);
  CancelMode get_cancel_mode(void);
  void set_cancel_mode(CancelMode cancel_mode);
  int get_icon_number(void);
  void set_icon_number(int icon_number);

  void set_variable(MessageId messageId, std::string value);
  void set_variable(MessageId messageId, int value);
  std::string get_variable(void);
  
  // current message
  void key_pressed(Controls::GameKey key);
  MessageId get_first_message_id(void);
  bool is_finished(void);
  static int get_last_answer(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
