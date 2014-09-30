/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/DialogResource.h"
#include "solarus/lowlevel/FileTools.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/Language.h"
#include "solarus/lua/LuaTools.h"

namespace Solarus {

const std::string DialogResource::file_name = "text/dialogs.dat";
std::map<std::string, Dialog> DialogResource::dialogs;

/**
 * \brief Loads all dialogs of the game.
 *
 * The dialogs are loaded from the language-specific file "text/dialogs.dat"
 * and stored into memory for future access by get_dialog().
 */
void DialogResource::initialize() {

  dialogs.clear();

  // Read the dialogs file.
  lua_State* l = luaL_newstate();
  const std::string& buffer = FileTools::data_file_read(file_name, true);
  int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  if (load_result != 0) {
    Debug::error(std::string("Failed to load dialog file '") + file_name
        + "' for language '" + Language::get_language() + "': "
        + lua_tostring(l, -1));
    lua_pop(l, 1);
  }
  else {
    lua_register(l, "dialog", l_dialog);
    if (lua_pcall(l, 0, 0, 0) != 0) {
      Debug::error(std::string("Failed to load dialog file '") + file_name
          + "' for language '" + Language::get_language() + "': "
          + lua_tostring(l, -1));
      lua_pop(l, 1);
    }
  }

  lua_close(l);
}

/**
 * \brief Closes the dialog resource.
 */
void DialogResource::quit() {

  dialogs.clear();
}

/**
 * \brief Returns whether the specified dialog exists.
 * \param dialog_id Id of the dialog to test.
 * \return true if such a dialog exists.
 */
bool DialogResource::exists(const std::string& dialog_id) {

  return dialogs.find(dialog_id) != dialogs.end();
}

/**
 * \brief Returns a dialog stored in the language-specific file
 * "text/dialogs.dat".
 * \param dialog_id id of the dialog to retrieve
 * \return the corresponding localized dialog
 */
const Dialog& DialogResource::get_dialog(const std::string& dialog_id) {

  Debug::check_assertion(exists(dialog_id), std::string(
      "No such dialog: '") + dialog_id + "'");
  return dialogs[dialog_id];
}

/**
 * \brief Function called by Lua to add a dialog to the resource.
 *
 * - Argument 1 (table): properties of the dialog.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int DialogResource::l_dialog(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    LuaTools::check_type(l, 1, LUA_TTABLE);

    const std::string dialog_id = LuaTools::check_string_field(l, 1, "id");
    const std::string text = LuaTools::check_string_field(l, 1, "text");
    Dialog dialog;
    dialog.set_id(dialog_id);
    dialog.set_text(text);

    // Traverse the table, looking for custom properties.
    lua_pushnil(l); // first key
    while (lua_next(l, 1) != 0) {

      const std::string& key = LuaTools::check_string(l, -2);
      if (key == "id" || key == "text") {
        lua_pop(l, 1);
        continue;
      }

      // Custom property.
      std::string value;
      int type = lua_type(l, -1);
      if (type == LUA_TSTRING || type == LUA_TNUMBER) {
        value = lua_tostring(l, -1);
      }
      else if (type == LUA_TBOOLEAN) {
        value = lua_toboolean(l, -1) ? "1" : "0";
      }
      else {
        LuaTools::error(l, std::string("Invalid value '") + key + "' for dialog '"
            + dialog_id + "'");
      }
      dialog.set_property(key, value);
      lua_pop(l, 1);
    }

    dialog.set_id(dialog_id);
    if (dialog.get_id().empty()) {
      LuaTools::error(l, "Missing value dialog_id");
    }

    if (dialog.get_text().empty()) {
      LuaTools::error(l, std::string("Missing text for dialog '") + dialog_id + "'");
    }

    if (exists(dialog_id)) {
      LuaTools::error(l, std::string("Duplicate dialog '") + dialog_id + "'");
    }
    dialogs[dialog_id] = dialog;

    return 0;
  });
}

}

