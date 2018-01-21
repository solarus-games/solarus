/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/DialogResources.h"
#include "solarus/core/Debug.h"
#include "solarus/lua/LuaTools.h"
#include <ostream>
#include <sstream>

namespace Solarus {

/**
 * \brief Creates an empty dialog.
 */
DialogData::DialogData() {
}

/**
 * \brief Returns the whole text of this dialog.
 * \return the text of this dialog
 */
const std::string& DialogData::get_text() const {
  return text;
}

/**
 * \brief Sets the text of this dialog.
 * \param text the text of this dialog
 */
void DialogData::set_text(const std::string& text) {
  this->text = text;
}

/**
 * \brief Returns all custom properties of this dialog.
 * \return The custom properties.
 */
const std::map<std::string, std::string>& DialogData::get_properties() const {
  return properties;
}

/**
 * \brief Returns a property exists in this dialog.
 * \param key Key of the property to check.
 * \return true if a property exists with this key.
 */
bool DialogData::has_property(const std::string& key) const {

  if (key.empty() || key == "text" || key == "id") {
    return false;
  }
  return properties.find(key) != properties.end();
}

/**
 * \brief Returns a custom properties of this dialog.
 * \param key Key of the property to get. It must exist.
 * \return The value of this property.
 */
const std::string& DialogData::get_property(const std::string& key) const {

  Debug::check_assertion(has_property(key),
    std::string("No such property: '") + key + "'");
  return properties.at(key);
}

/**
 * \brief Sets a custom property of this dialog.
 * \param key Key of the property to set.
 * \param value The value to set.
 */
void DialogData::set_property(const std::string& key, const std::string& value) {

  if (key.empty() || key == "text" || key == "id") {
    return;
  }
  properties[key] = value;
}

/**
 * \brief Removes a custom property of this dialog.
 * \param key Key of the property to remove.
 * \return \c true in case of success.
 */
bool DialogData::remove_property(const std::string& key) {
  return properties.erase(key) > 0;
}

/**
 * \brief Creates an empty dialog resources.
 */
DialogResources::DialogResources() {
}

/**
 * \brief Clears all dialogs.
 */
void DialogResources::clear() {
  dialogs.clear();
}

/**
 * \brief Returns all dialogs.
 * \return The dialogs indexed by their id.
 */
const std::map<std::string, DialogData>& DialogResources::get_dialogs() const {
  return dialogs;
}

/**
 * \brief Returns whether there exists a dialog with the specified id.
 * \param dialog_id The id to test.
 * \return \c true if a dialog exists with this id.
 */
bool DialogResources::has_dialog(const std::string& dialog_id) const {
  return dialogs.find(dialog_id) != dialogs.end();
}

/**
 * \brief Returns the dialog with the specified id.
 * \param dialog_id A dialog id. It must exist.
 * \return The dialog with this id.
 */
const DialogData& DialogResources::get_dialog(
    const std::string& dialog_id) const {

  const auto& it = dialogs.find(dialog_id);
  Debug::check_assertion(it != dialogs.end(),
    std::string("No such dialog: '") + dialog_id + "'");

  return it->second;
}

/**
 * \brief Returns the dialog with the specified id.
 *
 * Non-const version.
 *
 * \param dialog_id A dialog id. It must exist.
 * \return The dialog with this id.
 */
DialogData& DialogResources::get_dialog(
    const std::string& dialog_id) {

  const auto& it = dialogs.find(dialog_id);
  Debug::check_assertion(it != dialogs.end(),
    std::string("No such dialog: '") + dialog_id + "'");

  return it->second;
}

/**
 * \brief Adds a dialog.
 * \param dialog_id Id of the new dialog.
 * \param dialog The dialog to add.
 * \return \c true in case of success.
 */
bool DialogResources::add_dialog(
    const std::string& dialog_id, const DialogData& dialog) {

  const auto& result = dialogs.emplace(dialog_id, dialog);
  if (!result.second) {
    // Insertion failed: the id already exists.
    return false;
  }

  return true;
}

/**
 * \brief Removes a dialog.
 * \param dialog_id Id of the dialog to remove.
 * \return \c true in case of success.
 */
bool DialogResources::remove_dialog(const std::string& dialog_id) {

  return dialogs.erase(dialog_id) > 0;
}

/**
 * \brief Changes the id of a dialog.
 * \param old_dialog_id Old id of the dialog.
 * \param new_dialog_id New id to set.
 * \return \c true in case of success.
 * In case of failure, the old dialog is unchanged.
 */
bool DialogResources::set_dialog_id(
    const std::string& old_dialog_id, const std::string& new_dialog_id) {

  if (!has_dialog(old_dialog_id)) {
    // No dialog was found with the old id.
    return false;
  }

  if (has_dialog(new_dialog_id)) {
    // The new id is already used.
    return false;
  }

  DialogData dialog = get_dialog(old_dialog_id);
  remove_dialog(old_dialog_id);
  add_dialog(new_dialog_id, dialog);

  return true;
}

/**
 * \brief Function called by Lua to add a dialog to the resource.
 *
 * - Argument 1 (table): properties of the dialog.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int DialogResources::l_dialog(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_getfield(l, LUA_REGISTRYINDEX, "dialogs");
    DialogResources& dialogs = *static_cast<DialogResources*>(
          lua_touserdata(l, -1));
    lua_pop(l, 1);

    LuaTools::check_type(l, 1, LUA_TTABLE);

    std::string dialog_id = LuaTools::check_string_field(l, 1, "id");
    std::string text = LuaTools::check_string_field(l, 1, "text");
    text = unescape_multiline_string(text);
    DialogData dialog;
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

    if (dialog_id.empty()) {
      LuaTools::error(l, "Missing value id");
    }

    if (dialogs.has_dialog(dialog_id)) {
      LuaTools::error(l, std::string("Duplicate dialog '") + dialog_id + "'");
    }

    dialogs.add_dialog(dialog_id, dialog);

    return 0;
  });
}

/**
 * \copydoc LuaData::import_from_lua
 */
bool DialogResources::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "dialogs");
  lua_register(l, "dialog", l_dialog);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Error in dialog data file: ")
                 + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool DialogResources::export_to_lua(std::ostream& out) const {

  for (const auto kvp : dialogs) {
    const std::string& id = kvp.first;
    const DialogData& dialog = kvp.second;

    out << "dialog{\n  id = \"" << escape_string(id) << "\",\n";
    for (const auto pkvp : dialog.get_properties()) {
      out << "  " << pkvp.first << " = \"" << pkvp.second << "\",\n";
    }
    const std::string& text = dialog.get_text();
    export_multiline_string("text", text, out);
    out << "}\n\n";
  }
  return true;
}

}
