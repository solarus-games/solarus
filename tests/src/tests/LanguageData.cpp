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
#include "solarus/core/MapData.h"
#include "solarus/core/QuestDatabase.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/StringResources.h"
#include "test_tools/TestEnvironment.h"
#include <iostream>

using namespace Solarus;

namespace {

/**
 * \brief Checks reading and writing a strings data file.
 */
void check_strings(TestEnvironment& /* env */, const std::string& language_id) {

  StringResources string_resources;
  bool success = false;

  // Import the strings data file.
  std::string file_name = "languages/" + language_id + "/text/strings.dat";
  std::string imported_string_buffer = QuestFiles::data_file_read(file_name);
  success = string_resources.import_from_buffer(imported_string_buffer, file_name);
  Debug::check_assertion(success, "Strings import failed");

  // Export it.
  std::string exported_string_buffer;
  success = string_resources.export_to_buffer(exported_string_buffer);
  Debug::check_assertion(success, "Strings export failed");

  // Check that the file is identical after import/export.
  if (exported_string_buffer != imported_string_buffer) {
    std::cerr << "Strings '" << language_id << "' differs from the original one after export." << std::endl
        << "*** Original strings file:" << std::endl << imported_string_buffer << std::endl
        << "*** Exported strings file:" << std::endl << exported_string_buffer << std::endl;
    Debug::die("Strings '" + language_id + "': exported file differs from the original one");
  }
}

/**
 * \brief Checks reading and writing a dialogs data file.
 */
void check_dialogs(TestEnvironment& /* env */, const std::string& language_id) {

  DialogResources dialog_resources;
  bool success = false;

  // Import the dialogs data file.
  std::string file_name = "languages/" + language_id + "/text/dialogs.dat";
  std::string imported_dialog_buffer = QuestFiles::data_file_read(file_name);
  success = dialog_resources.import_from_quest_file(file_name);
  Debug::check_assertion(success, "Dialogs import failed");

  // Export it.
  std::string exported_dialog_buffer;
  success = dialog_resources.export_to_buffer(exported_dialog_buffer);
  Debug::check_assertion(success, "Dialogs export failed");

  // Check that the file is identical after import/export.
  if (exported_dialog_buffer != imported_dialog_buffer) {
    std::cerr << "Dialogs '" << language_id << "' differs from the original one after export." << std::endl
        << "*** Original dialogs file:" << std::endl << imported_dialog_buffer << std::endl
        << "*** Exported dialogs file:" << std::endl << exported_dialog_buffer << std::endl;
    Debug::die("Dialogs '" + language_id + "': exported file differs from the original one");
  }
}

}

/**
 * \brief Tests reading and writing language data files.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  const std::map<std::string, std::string>& language_elements =
      CurrentQuest::get_database().get_resource_elements(ResourceType::LANGUAGE);
  Debug::check_assertion(!language_elements.empty(), "No languages");
  for (const auto& kvp : language_elements) {
    const std::string& language_id = kvp.first;
    check_strings(env, language_id);
    check_dialogs(env, language_id);
  }

  return 0;
}
