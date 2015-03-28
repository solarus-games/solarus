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
#include "solarus/DialogResource.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/Language.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/DialogResourceData.h"

namespace Solarus {

const std::string DialogResource::file_name = "text/dialogs.dat";
std::map<std::string, Dialog> DialogResource::dialogs;

/**
 * \brief Adds a new dialog.
 *
 * This function is called while loading the dialog data file.
 *
 * \param id Id of this dialog.
 * \param dialog_data Properties of the dialog to create.
 */
void DialogResource::add_dialog(
    const std::string& id, const DialogData& dialog_data) {

  // Create the dialog.
  Dialog dialog;
  dialog.set_id(id);
  dialog.set_text(dialog_data.get_text());

  for (const auto kvp : dialog_data.get_properties()) {
    dialog.set_property(kvp.first, kvp.second);
  }

  dialogs.emplace(id, dialog);
}

/**
 * \brief Loads all dialogs of the game.
 *
 * The dialogs are loaded from the language-specific file "text/dialogs.dat"
 * and stored into memory for future access by get_dialog().
 */
void DialogResource::initialize() {

  dialogs.clear();

  // Read the dialogs file.
  DialogResourceData data;
  const std::string& buffer = QuestFiles::data_file_read(file_name, true);
  bool success = data.import_from_buffer(buffer);
  if (success) {
    // Get the imported data.
    for (const auto& kvp : data.get_dialogs()) {
      add_dialog(kvp.first, kvp.second);
    }
  } else {
    Debug::error(std::string("Failed to load dialog file '") + file_name
        + "' for language '" + Language::get_language());
  }
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

}

