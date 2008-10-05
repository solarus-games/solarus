#ifndef ZSDX_DIALOG_BOX_H
#define ZSDX_DIALOG_BOX_H

#include "Common.h"
#include "KeysEffect.h"
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
  std::map<MessageId, string> variables;
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

  void set_variable(MessageId messageId, string value);
  void set_variable(MessageId messageId, int value);
  string get_variable(void);
  
  // current message
  void key_pressed(const SDL_keysym &keysym);
  bool is_over(void);
  static int get_last_answer(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
