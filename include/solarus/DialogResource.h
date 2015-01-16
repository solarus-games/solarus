/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_DIALOG_RESOURCE_H
#define SOLARUS_DIALOG_RESOURCE_H

#include "solarus/Common.h"
#include "solarus/Dialog.h"
#include <map>

struct lua_State;

namespace Solarus {

/**
 * \brief Provides access to dialogs translated in the current language.
 *
 * This class provides the game dialogs loaded from an external file
 * that depends on the current language.
 * This class is used to load all dialogs displayed in the dialog box during
 * the game.
 */
class DialogResource {

  public:

    static void initialize();
    static void quit();

    static bool exists(const std::string& dialog_id);
    static const Dialog& get_dialog(const std::string& dialog_id);

  private:

    static const std::string file_name;           /**< the dialog file */
    static std::map<std::string, Dialog> dialogs; /**< all dialogs of the game */

    // we don't need to instantiate this class
    DialogResource() = delete;

    static int l_dialog(lua_State* l);
};

}

#endif

