/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "Dialog.h"
#include "GameCommands.h"

/**
 * \brief Manages the dialog box where dialogs can be displayed.
 *
 * A very basic built-in dialog box is used by default, but the quest
 * script can define its own one instead.
 * This class handles both cases.
 */
class DialogBox {

  public:

    DialogBox(Game& game);
    ~DialogBox();

    Game& get_game();
    bool is_enabled();

    void start_dialog(const std::string& dialog_id, int callback_ref);
    void notify_command_pressed(GameCommands::Command command);
    const std::string& get_dialog_id();

    void update();
    void draw(Surface& dst_surface);

  private:

    Game& game;                                     /**< the game this dialog box belongs to */
    std::string dialog_id;                          /**< id of the current dialog or an empty string */
    Dialog dialog;                                  /**< current dialog */
    int callback_ref;                               /**< Lua ref of a function to call when the dialog finishes. */

    // Built-in dialog box.
    static const int nb_visible_lines = 3;
    std::string lines[nb_visible_lines];            /**< text of the 3 lines currently shown */
    TextSurface* line_surfaces[nb_visible_lines];   /**< text surface of each line */
};

#endif
