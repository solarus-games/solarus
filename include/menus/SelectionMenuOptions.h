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
#ifndef SOLARUS_SELECTION_MENU_OPTIONS_H
#define SOLARUS_SELECTION_MENU_OPTIONS_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"
#include "lowlevel/Rectangle.h"

/**
 * A phase of the selection menu where the user change set global options
 * such as the language, the video mode and the sound volume.
 * Those options are global to all savegames even though the video mode
 * can still be changed during the game.
 * They are saved in the file config.ini.
 * This class acts as a graphical interface for the config.ini file.
 */
class SelectionMenuOptions: public SelectionMenuPhase {

  private:

    // text of all options
    static const int nb_options = 4;                              /**< number of options to customize (currently 4) */
    static const std::string label_keys[nb_options];              /**< string key describing each option */

    int nb_values[nb_options];                                    /**< number of possible values for each option */
    std::string *all_values[nb_options];                          /**< for each option, array of all possible values */
    int current_indices[nb_options];                              /**< for each option, its currently selected index */

    TextSurface *label_texts[nb_options];                         /**< text surface describing each option */
    TextSurface *value_texts[nb_options];                         /**< text surface showing the current value of each option */
    Sprite *arrow_sprite;                                         /**< sprite of the arrow cursor */

    // option-specific data
    std::string *language_codes;                                  /**< code of each language */

    // cursors
    int cursor_position;                                          /**< cursor specific to the options screen:
							           * 0 = language
							           * 1 = video mode
							           * 2 = music volume
							           * 3 = sound volume
							           * (nb_options) = validation button */
    bool modifying;                                               /**< indicates that the user is currently setting an option */
    Sprite *left_arrow_sprite;                                    /**< sprite of a blinking left arrow to indicate how to change a value */
    Sprite *right_arrow_sprite;                                   /**< sprite of a blinking right arrow to indicate how to change a value */

    void set_cursor_position(int cursor_position);
    void move_cursor_up(void);
    void move_cursor_down(void);
    void move_cursor_left_or_right(void);

    void set_option_next_value(void);
    void set_option_previous_value(void);
    void set_option_value(int index);
    void set_option_value(int option, int index);

    void load_configuration(void);
    void reload_strings(void);

  public:

    // creation and destruction
    SelectionMenuOptions(SelectionMenu *selection_menu);
    ~SelectionMenuOptions(void);

    // update and display
    void update(void);
    void display(Surface *destination_surface);
    void notify_event(InputEvent &event);
};

#endif

