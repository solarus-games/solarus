/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_DIALOG_BOX_H
#define SOLARUS_DIALOG_BOX_H

#include "Common.h"
#include "KeysEffect.h"
#include "Controls.h"
#include "lowlevel/Rectangle.h"
#include <map>

/**
 * A dialog box where a succession of messages can be displayed.
 * This class displays the dialog box and handles its properties:
 * the text speed and the state of the action and sword keys.
 */
class DialogBox {

  public:

    /**
     * The possible styles of the dialog box
     */
    enum Style {
      WITH_FRAME,    /**< the usual dialog box with a frame and a semi-transparent dark blue background */
      WITHOUT_FRAME, /**< no dialog box is shown, only the text is displayed */
    };

    /**
     * The possible speeds of a dialog's text.
     */
    enum Speed {
      SPEED_SLOW,
      SPEED_MEDIUM,
      SPEED_FAST     // default
    };

    /**
     * Indicates what happens when the user tries to skip
     * the current message.
     */
    enum SkipMode {
      SKIP_NONE,    /**< the current message cannot be skipped */
      SKIP_CURRENT, /**< the player can display the current message to its end immediately */
      SKIP_ALL      /**< the current message and the next ones can be totally skipped */
    };

  private:

    // current message
    Message *current_message;
    MessageId current_message_id;
    std::map<MessageId, std::string> variables;

    MessageId first_message_id;

    // dialog properties
    Game *game;
    static Style style;
    Speed speed;
    SkipMode skip_mode;
    bool skipped;              // true if the user has skipped the dialog
    int icon_number;           // index of the 16*16 icon displayed, or -1 if there is no icon

    KeysEffect::ActionKeyEffect action_key_effect_saved;
    KeysEffect::SwordKeyEffect sword_key_effect_saved;

    // graphics
    Surface *dialog_surface;
    Surface *img_box;
    Surface *img_icons;
    Sprite *end_message_sprite;

    // sounds
    Sound *end_message_sound;
    Sound *switch_answer_sound;

    // position of the images
    int x;
    int y;
    Rectangle box_dst_position;
    Rectangle question_dst_position;
    Rectangle icon_dst_position;

    void show_message(const MessageId &messageId);
    void show_next_message(void);

    void action_key_pressed(void);
    void sword_key_pressed(void);
    void up_or_down_key_pressed(void);

  public:

    // creation and destruction
    DialogBox(Game *game, const MessageId &first_message_id, int x, int y);
    ~DialogBox(void);

    // dialog properties
    Game * get_game(void);
    static void set_style(Style style);
    Speed get_speed(void);
    void set_speed(Speed speed);
    SkipMode get_skip_mode(void);
    void set_skip_mode(SkipMode skip_mode);
    int get_icon_number(void);
    void set_icon_number(int icon_number);
    bool is_letter_sound_enabled(void);

    void set_variable(const MessageId &first_message_id, const std::string &value);
    void set_variable(const MessageId &first_message_id, int value);
    const std::string& get_variable(void);

    // current message
    void key_pressed(Controls::GameKey key);
    MessageId get_first_message_id(void);
    bool is_finished(void);
    bool was_skipped(void);
    void show_all_now(void);

    // update and display
    void update(void);
    void display(Surface *destination_surface);
};

#endif

