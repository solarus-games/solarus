/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "Dialog.h"
#include "Sprite.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/Surface.h"
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

  public:

    // creation and destruction
    DialogBox(Game& game);
    ~DialogBox();

    // dialog properties
    Game& get_game();
    void set_style(Style style);
    bool is_enabled();

    // current message
    void start_dialog(const std::string& dialog_id, Script *issuer_script = NULL,
        VerticalPosition vertical_position = POSITION_AUTO);
    bool is_letter_sound_enabled();
    void set_variable(const std::string& dialog_id, const std::string &value);
    void set_variable(const std::string& dialog_id, int value);
    const std::string& get_variable();
    int get_last_answer();
    void key_pressed(GameControls::GameKey key);
    const std::string& get_dialog_id();
    void show_all_now();

    // update and display
    void update();
    void display(Surface& dst_surface);

  private:

    static const int nb_visible_lines = 3;          /**< maximum number of lines displayed at the same time */
    static const uint32_t char_delays[];            /**< delays between two characters depending on the dialog speed */

    // dialog box properties
    Game& game;                                     /**< the game this dialog box belongs to */
    KeysEffect::ActionKeyEffect action_key_effect_saved;  /**< effect of the action key before starting the dialog */
    KeysEffect::SwordKeyEffect sword_key_effect_saved;    /**< effect of the sword key before starting the dialog */
    std::string dialog_id;                          /**< id of the current dialog or an empty string */
    Dialog dialog;                                  /**< current dialog */
    std::map<std::string, std::string> variables;   /**< variables to display in dialogs */
    Script* issuer_script;                          /**< the script (if any) that started the current dialog */
    Style style;                                    /**< style of the dialog box */
    Dialog::SkipMode skip_mode;                     /**< indicates what happens when the user tries to skip the dialog */
    int icon_number;                                /* index of the 16*16 icon displayed, or -1 if there is no icon */
    bool skipped;                                   /* true if the user has skipped the dialog (SKIP_ALL) */
    int last_answer;                                /**< the answer selected in the last dialog: 0 for the first one, 1 for the second one,
                                                     * -1 if there was no question */

    // displaying text gradually
    std::list<std::string>::const_iterator line_it; /**< iterator over the lines of the current dialog */
    std::string lines[nb_visible_lines];            /**< text of the 3 lines currently shown */
    TextSurface* line_surfaces[nb_visible_lines];   /**< text surface of each line */
    int line_index;                                 /**< line of the dialog box currently shown (0 to nb_visible_lines) */
    unsigned int char_index;                        /**< index of the next character to show in the current line */
    uint32_t char_delay;                            /**< delay between two characters in milliseconds */
    uint32_t next_char_date;                        /**< when the next character should be displayed */
    uint32_t next_sound_date;                       /**< date of the next letter scrolling sound */

    // graphics
    Surface dialog_surface;                         /**< surface where the dialog is drawn*/
    Surface box_img;                                /**< image of the dialog box frame */
    Surface icons_img;                              /**< image containing all possible icons used in dialogs */
    Sprite end_lines_sprite;                        /**< sprite displayed when 3 lines are finished */

    // position of the images
    Rectangle box_src_position;                     /**< rectangle of the dialog box in its source image */
    Rectangle box_dst_position;                     /**< destination rectangle of the dialog box image */
    Rectangle question_src_position;                /**< rectangle of the question icon in the source image */
    Rectangle question_dst_position;                /**< destination rectangle of the question image (depends on the answer currently selected) */
    Rectangle icon_dst_position;                    /**< destination rectangle of the icon */

    void set_vertical_position(VerticalPosition vertical_position);
    void show_more_lines();
    void show_next_dialog();
    bool is_full();
    bool has_more_lines();
    void close();

    void action_key_pressed();
    void sword_key_pressed();
    void up_or_down_key_pressed();

    void update_lines();
    void add_character();
};

#endif

