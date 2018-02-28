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
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/Debug.h"
#include "solarus/core/DialogResources.h"
#include "solarus/core/Logger.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/QuestDatabase.h"
#include "solarus/core/QuestProperties.h"
#include "solarus/core/StringResources.h"
#include <lua.hpp>

namespace Solarus {

namespace CurrentQuest {

namespace {

bool initialized = false;

}

/**
 * \brief Reads the resource list, the quest properties and stores them.
 */
void initialize() {

  // Read the quest database file.
  QuestDatabase& database = get_database();
  database .import_from_quest_file("project_db.dat");

  // Read the quest properties file.
  QuestProperties& properties = get_properties();

  const std::string file_name("quest.dat");
  lua_State* l = luaL_newstate();
  const std::string& buffer = QuestFiles::data_file_read(file_name);
  int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  if (load_result != 0) {
    // Syntax error in quest.dat.
    // Loading quest.dat failed.
    // There may be a syntax error, or this is a quest for Solarus 0.9.
    // There was no version number at that time.

    const std::string error_message = lua_tostring(l, -1);
    lua_close(l);

    if (std::string(buffer).find("[info]")) {
      // Quest format of Solarus 0.9.
      Debug::die(std::string("This quest is made for Solarus 0.9 but you are running Solarus ")
          + SOLARUS_VERSION);
    }
    else {
      Debug::die(std::string("Failed to load quest.dat: ") + error_message);
    }
  }

  // Normal case.
  properties.import_from_lua(l);
  lua_close(l);

  initialized = true;
}

/**
 * \brief Clears the loaded quest resource list.
 */
void quit() {

  get_database().clear();
  get_strings().clear();
  get_dialogs().clear();

  initialized = false;
}

/**
 * \brief Returns whether there is a current quest.
 * \return \c true if there is a current quest.
 */
bool is_initialized() {
  return initialized;
}


/**
 * \brief Returns the properties of current quest.
 * \return The current quest properties.
 */
QuestProperties& get_properties() {

  // The quest properties object must be in a function to avoid static
  // initialization order problems.
  static QuestProperties properties;
  return properties;
}

/**
 * \brief Returns the database of the current quest.
 * \return The current quest resource list.
 */
QuestDatabase& get_database() {

  // The database object must be in a function to avoid static initialization
  // order problems.
  static QuestDatabase database;
  return database;
}

/**
 * \brief Returns whether there exists an element with the specified id.
 * \param resource_type A type of resource.
 * \param id The id to look for.
 * \return \c true if there resource_exists an element with the specified id in this
 * resource type.
 */
bool resource_exists(ResourceType resource_type, const std::string& id) {

  return get_database().resource_exists(resource_type, id);
}

/**
 * \brief Returns the list of element ids and descriptions of the specified resource type.
 * \param resource_type A type of resource.
 * \return The id and description of every declared element of this type,
 * in their declaration order.
 */
const std::map<std::string, std::string>&
get_resources(ResourceType resource_type) {

  return get_database().get_resource_elements(resource_type);
}

/**
 * \brief Returns whether a language exists for the current quest.
 * \param language_code Code of the language to test.
 * \return \c true if this language exists.
 */
bool has_language(const std::string& language_code) {

  return resource_exists(ResourceType::LANGUAGE, language_code);
}

/**
 * \brief Sets the current language.
 *
 * The language-specific data will be loaded from the directory of this language.
 * This function must be called before the first language-specific file is loaded.
 *
 * \param language_code Code of the language to set.
 */
void set_language(const std::string& language_code) {

  Debug::check_assertion(has_language(language_code),
      std::string("No such language: '") + language_code + "'");

  get_language() = language_code;

  // Read the quest string list file.
  get_strings().clear();
  get_strings().import_from_quest_file("text/strings.dat", true);

  // Read the quest dialog list file.
  DialogResources resources;
  std::map<std::string, Dialog>& dialogs = get_dialogs();

  bool success = resources.import_from_quest_file("text/dialogs.dat", true);

  // Create dialogs.
  dialogs.clear();
  if (success) {
    for (const auto& kvp : resources.get_dialogs()) {

      const std::string& id = kvp.first;
      const DialogData& data = kvp.second;

      Dialog dialog;
      dialog.set_id(id);
      dialog.set_text(data.get_text());

      for (const auto& pkvp : data.get_properties()) {
        dialog.set_property(pkvp.first, pkvp.second);
      }

      dialogs.emplace(id, dialog);
    }
  }

  Logger::info(std::string("Language: ") + language_code);
}

/**
 * \brief Returns the current language.
 *
 * The language-specific data are be loaded from the directory of this language.
 *
 * \return code of the language, or an empty string if no language is set
 */
std::string& get_language() {

  // The language code must be in a function to avoid static initialization
  // order problems.
  static std::string language_code;
  return language_code;
}

/**
 * \brief Returns the user-friendly name of a language for this quest.
 * \param language_code Code of a language.
 * \return Name of this language of an empty string.
 */
std::string get_language_name(const std::string& language_code) {

  const std::map<std::string, std::string>& languages =
      get_resources(ResourceType::LANGUAGE);

  const auto& it = languages.find(language_code);
  if (it != languages.end()) {
    return it->second;
  }

  return "";
}

/**
 * \brief Returns the string list of the current quest.
 * \return The current quest string list.
 */
StringResources& get_strings() {

  // The string resources object must be in a function to avoid static
  // initialization order problems.
  static StringResources strings;
  return strings;
}

/**
 * \brief Returns whether a string exists in the language-specific file
 * "text/strings.dat" for the current language.
 * \param key Id of a string.
 * \return true if the string exists.
 */
bool string_exists(const std::string& key) {

  return get_strings().has_string(key);
}

/**
 * \brief Returns a string stored in the language-specific file
 * "text/strings.dat" for the current language.
 * \param key Id of the string to retrieve. It must exist.
 * \return The corresponding localized string.
 */
const std::string& get_string(const std::string& key) {

  return get_strings().get_string(key);
}

/**
 * \brief Returns the dialog list of the current quest.
 * \return The current quest dialog list.
 */
std::map<std::string, Dialog>& get_dialogs() {

  // The dialog objects must be in a function to avoid static initialization
  // order problems.
  static std::map<std::string, Dialog> dialogs;
  return dialogs;
}

/**
 * \brief Returns whether the specified dialog exists.
 * \param dialog_id Id of the dialog to test.
 * \return true if such a dialog exists.
 */
bool dialog_exists(const std::string& dialog_id) {

  const auto& dialogs = get_dialogs();
  return dialogs.find(dialog_id) != dialogs.end();
}

/**
 * \brief Returns a dialog stored in the language-specific file
 * "text/dialogs.dat".
 * \param dialog_id id of the dialog to retrieve
 * \return the corresponding localized dialog
 */
const Dialog& get_dialog(const std::string& dialog_id) {

  Debug::check_assertion(dialog_exists(dialog_id), std::string(
    "No such dialog: '") + dialog_id + "'");
  return get_dialogs()[dialog_id];
}

/**
 * \brief Returns whether the quest format is greater than or equal
 * to a Solarus version.
 * \param version A Solarus version (major and minor numbers).
 * \return \c true if the quest format is greater than to equal to this
 * Solarus version.
 */
bool is_format_at_least(const std::pair<int, int>& version) {

  const std::pair<int, int>& quest_format = get_properties().get_solarus_version_major_minor();
  return quest_format.first > version.first ||
      (quest_format.first == version.first && quest_format.second >= version.second);
}

/**
 * \brief Returns whether the quest format is lower than or equal
 * to a Solarus version.
 * \param version A Solarus version (major and minor numbers).
 * \return \c true if the quest format is lower than to equal to this
 * Solarus version.
 */
bool is_format_at_most(const std::pair<int, int>& version) {

  const std::pair<int, int>& quest_format = get_properties().get_solarus_version_major_minor();
  return quest_format.first < version.first ||
      (quest_format.first == version.first && quest_format.second <= version.second);
}

}  // namespace CurrentQuest

}
