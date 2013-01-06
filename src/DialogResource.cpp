/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "DialogResource.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

const std::string DialogResource::file_name = "text/dialogs.dat";
std::map<std::string, Dialog> DialogResource::dialogs;

/**
 * @brief Constructor.
 */
DialogResource::DialogResource() {

}

/**
 * @brief Destructor.
 */
DialogResource::~DialogResource() {

}

/**
 * @brief Loads all dialogs of the game.
 *
 * The dialogs are loaded from the language-specific file "text/dialogs.lua"
 * and stored into memory for future access by get_dialog().
 */
void DialogResource::initialize() {

  dialogs.clear();

  // read the dialogs file
  lua_State* l = luaL_newstate();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size, true);
  luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  lua_register(l, "dialog", l_dialog);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(StringConcat() << "Error: failed to load dialog file "
        << lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  lua_close(l);
}

/**
 * @brief Closes the dialog resource.
 */
void DialogResource::quit() {

  dialogs.clear();
}

/**
 * @brief Returns a dialog stored in the language-specific file
 * "text/dialogs.lua".
 * @param dialog_id id of the dialog to retrieve
 * @return the corresponding localized dialog
 */
const Dialog& DialogResource::get_dialog(const std::string& dialog_id) {

  Debug::check_assertion(dialogs.count(dialog_id) > 0, StringConcat()
      << "Cannot find dialog with id '" << dialog_id << "'");
  return dialogs[dialog_id];
}

/**
 * @brief Function called by Lua to add a dialog to the resource.
 *
 * - Argument 1 (table): properties of the dialog (possible keys are id, icon,
 * skip, question, next, next2 and text)
 *
 * @param l the Lua context that is calling this function
 * @return number of values returned to Lua
 */
int DialogResource::l_dialog(lua_State* l) {

  luaL_checktype(l, 1, LUA_TTABLE);

  std::string dialog_id;
  Dialog dialog;

  // traverse the table, looking for properties
  lua_pushnil(l); // first key
  while (lua_next(l, 1) != 0) {

    const std::string& key = luaL_checkstring(l, -2);
    if (key == "id") {
      dialog_id = luaL_checkstring(l, -1);
    }
    else if (key == "icon") {
      int icon = int(luaL_checkinteger(l, -1));
      dialog.set_icon(icon);
    }
    else if (key == "skip") {
      const std::string skip_mode = luaL_checkstring(l, -1);
      if (skip_mode == "current") {
        dialog.set_skip_mode(Dialog::SKIP_CURRENT);
      }
      else if (skip_mode == "all") {
        dialog.set_skip_mode(Dialog::SKIP_ALL);
      }
      else {
        dialog.set_skip_mode(Dialog::SKIP_NONE);
      }
    }
    else if (key == "question") {
      bool question = lua_toboolean(l, -1);
      dialog.set_question(question);
    }
    else if (key == "next") {
      const std::string next = luaL_checkstring(l, -1);
      dialog.set_next(next);
    }
    else if (key == "next2") {
      const std::string next2 = luaL_checkstring(l, -1);
      dialog.set_next2(next2);
    }
    else if (key == "text") {
      const std::string text = luaL_checkstring(l, -1);
      dialog.set_text(text);
    }
    else {
      luaL_error(l, (StringConcat() << "Invalid key '" << key << "'").c_str());
    }
    lua_pop(l, 1); // pop the value, let the key for the iteration
  }

  dialogs[dialog_id] = dialog;

  return 0;
}

