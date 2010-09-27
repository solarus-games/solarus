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
#include "GameControls.h"
#include "lowlevel/Rectangle.h"
#include <map>

/**
 * @brief A dialog box where a succession of messages can be displayed.
 *
 * This class displays the dialog box and handles its properties:
 * the text speed and the state of the action and sword keys.
 * The dialog box can show a dialog. A dialog is a succession of messages.
 */
class DialogBox {

  public:

    /**
     * The possible styles of the dialog box.
     */
    enum Style {
      STYLE_WITH_FRAME,    /**< the usual dialog box with a frame and a semi-transparent dark blue background */
      STYLE_WITHOUT_FRAME  /**< no dialog box is shown, only the text is displayed */
    };

    /**
     * The possible vertical positions of the dialog box on the screen.
     */
    enum VerticalPosition {
      POSITION_AUTO,       /**< displayed such that the hero is not hidden */
      POSITION_TOP,        /**< displayed at the top */
      POSITION_BOTTOM      /**< displayed at the bottom */
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

    // dialog properties
    Game &game;                                     /**< the game this dialog box belongs to */
    Style style;                                    /**< style of the dialog box */

    // current message
    KeysEffect::ActionKeyEffect action_key_effect_saved;  /**< effect of the action key before starting the message sequence */
    KeysEffect::SwordKeyEffect sword_key_effect_saved;    /**< effect of the sword key before starting the message sequence */
    MessageId first_message_id;                     /**< id of the first message of the current sequence */
    Message *current_message;                       /**< the message currently shown (NULL if the dialog box is disabled) */
    MessageId current_message_id;                   /**< id of the message currently shown */
    std::map<MessageId, std::string> variables;     /**< variables to display if the next messages */

    Speed speed;                                    /**< speed of the text */
    SkipMode skip_mode;                             /**< indicates what happens when the user tries to skip the current message */
    int icon_number;                                /* index of the 16*16 icon displayed, or -1 if there is no icon */
    bool skipped;                                   /* true if the user has skipped the dialog */
    int last_answer;                                /**< the answer selected in the last message sequence: 0 for the first one, 1 for the second one,
						     * -1 if there was no question */ 

    // graphics
    Surface *dialog_surface;                        /**< surface where the dialog is drawn*/
    Surface *box_img;                               /**< image of the dialog box frame */
    Surface *icons_img;                             /**< image containing all possible icons used in messages */
    Sprite *end_message_sprite;                     /**< sprite displayed when the current message is finished*/

    // position of the images
    Rectangle box_src_position;                     /**< rectangle of the dialog box in its source image */
    Rectangle box_dst_position;                     /**< destination rectangle of the dialog box image */
    Rectangle question_src_position;                /**< rectangle of the question icon in the source image */
    Rectangle question_dst_position;                /**< destination rectangle of the question image (depends on the answer currently selected) */
    Rectangle icon_dst_position;                    /**< destination rectangle of the icon */

    void set_vertical_position(VerticalPosition vertical_position);
    void show_message(const MessageId &message_id);
    void show_next_message();
    void close();

    void action_key_pressed();
    void sword_key_pressed();
    void up_or_down_key_pressed();

  public:

    // creation and destruction
    DialogBox(Game &game);
    ~DialogBox();

    // dialog properties
    Game& get_game();
    void set_style(Style style);
    bool is_enabled();

    // current message
    void start_dialog(const MessageId &first_message_id, VerticalPosition vertical_position = POSITION_AUTO);
    Speed get_speed();
    void set_speed(Speed speed);
    SkipMode get_skip_mode();
    void set_skip_mode(SkipMode skip_mode);
    int get_icon_number();
    void set_icon_number(int icon_number);
    bool is_letter_sound_enabled();
    void set_variable(const MessageId &first_message_id, const std::string &value);
    void set_variable(const MessageId &first_message_id, int value);
    const std::string& get_variable();
    int get_last_answer();
    void set_last_answer(int answer);
    void key_pressed(GameControls::GameKey key);
    MessageId get_first_message_id();
    bool is_finished();
    bool was_skipped();
    void show_all_now();

    // update and display
    void update();
    void display(Surface *destination_surface);
};

#endif

