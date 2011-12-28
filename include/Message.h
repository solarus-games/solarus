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
#ifndef SOLARUS_MESSAGE_H
#define SOLARUS_MESSAGE_H

#include "Common.h"
#include "DialogBox.h"

/**
 * @brief Lines of text displayed in a dialog box.
 *
 * This class displays three lines of text in a dialog box.
 * A message is usually part of a sequence of several messages called a dialog.
 */
class Message {

  private:

    // properties of the message

    DialogBox& dialog_box;            /**< the dialog box where this message is displayed */
    std::string lines[3];             /**< the 3 lines of text of the message */
    TextSurface* text_surfaces[3];    /**< the 3 corresponding text surfaces */
    bool question;                    /**< is this message a question? */
    bool last;                        /**< is this message the last one of the dialog? */

    // current state of displaying

    unsigned int line_index;          /**< line currently displayed (0 to 2) */
    unsigned int char_index;          /**< index of the next character to show */
    uint32_t delay;                   /**< delay between two characters in milliseconds */
    uint32_t next_char_date;          /**< when the next character is displayed */
    uint32_t next_sound_date;         /**< date of the next character sound */
    bool show_all;                    /**< makes all text be displayed now */

    void update_char_delay();
    void add_character();
    void set_variable(const std::string& value);

  public:

    // creation and destruction
    Message(DialogBox& dialog_box, const std::string& dialog_id, int x, int y);
    ~Message();

    // message properties
    bool is_question();

    // message current state
    bool is_finished();
    void show_all_now();

    // update and display
    void update();
    void display(Surface *destination_surface);

};

#endif

