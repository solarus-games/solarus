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
#ifndef SOLARUS_SELECTION_MENU_H
#define SOLARUS_SELECTION_MENU_H

#include "Common.h"
#include "Screen.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/Color.h"

/**
 * @brief Represents the selection screen where the player chooses his savegame.
 *
 * This screen is divided in several phase. Each phase is a subclass of SelectionMenuPhase.
 */
class SelectionMenu: public Screen {

  private:

    // phase
    SelectionMenuPhase *current_phase;                   /**< current phase of the selection menu */
    SelectionMenuPhase *next_phase;                      /**< next phase of the selection menu (NULL if we are not changing the phase) */
    Surface *surface;                                    /**< surface where the current menu is drawn */

    // transitions
    Transition *transition;                              /**< a transition effect currently displayed (or NULL) */

    // images
    Color background_color;                              /**< background color */
    Surface *cloud_img;                                  /**< clouds moving in background */
    Surface *background_img;                             /**< the selection menu background image (above the clouds) */
    Surface *save_container_img;                         /**< image where the information about a savegame can be displayed */
    Surface *option_container_img;                       /**< image where an option can be displayed (e.g. "Erase" or "Quit") */
    Surface *number_imgs[3];                             /**< the numbers 1 to 3 */

    // cursor
    Sprite *cursor_sprite;                               /**< sprite of the cursor */
    int cursor_position;                                 /**< current cursor position (1 to 5) */
    uint32_t allow_cursor_date;                          /**< date when it is possible to move the cursor again */

    // music
    Music *music;                                        /**< the music currently playing */

    // text
    TextSurface *title_text;                             /**< text surface of the title of the current phase*/
    TextSurface *option1_text;                           /**< text surface of the left option */
    TextSurface *option2_text;                           /**< text surface of the right option */

    // savegames data
    Savegame *savegames[3];                              /**< the savegames data */
    TextSurface *text_player_names[3];                   /**< name of each player */
    HeartsView *hearts_views[3];                         /**< life view of each player */

    // clouds animation
    Rectangle cloud_positions[16];                       /**< current position of each cloud in background */
    uint32_t next_cloud_move;                            /**< date when the clouds move */

    // initialization functions
    void initialize_clouds();
    void read_savegames();

    // displaying elements
    void display_title_text(Surface *destination_surface);
    void display_transition(Surface *destination_surface);

  public:

    // creation and destruction
    SelectionMenu(Solarus &solarus);
    ~SelectionMenu();

    // update and display (called by Solarus)
    void update();
    void display(Surface *screen_surface);
    void notify_event(InputEvent &event);

    // phase
    void set_next_phase(SelectionMenuPhase *next_phase);
    void start_game(Savegame &savegame);
    void set_exiting();

    // transitions
    void set_transition(Transition *transition);
    bool has_transition();
    bool is_transition_finished();

    // sounds
    void play_cursor_sound();
    void play_ok_sound();
    void play_error_sound();
    void play_erase_sound();
    void play_letter_sound();

    // savegames
    Savegame ** get_savegames();
    Savegame * get_savegame(int save_number);
    void reload_savegames();

    // texts
    void set_title_text(const std::string &title_string_key);
    void set_bottom_options(const std::string &left_string_key, const std::string &right_string_key);

    // cursor
    Sprite *get_cursor_sprite();
    int get_cursor_position();
    void set_cursor_position(int cursor_position);
    void move_cursor_up();
    void move_cursor_down();
    void move_cursor_left_or_right();

    // displaying elements (subclasses of SelectionMenuPhase may call these functions)
    void display_savegame(Surface *destination_surface, int save_number);
    void display_savegame_number(Surface *destination_surface, int save_number);
    void display_savegame_cursor(Surface *destination_surface);
    void display_bottom_options(Surface *destination_surface);
};

#endif

