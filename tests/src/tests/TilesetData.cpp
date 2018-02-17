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
#include "solarus/entities/TilesetData.h"
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/Debug.h"
#include "solarus/core/QuestDatabase.h"
#include "solarus/core/QuestFiles.h"
#include "test_tools/TestEnvironment.h"
#include <iostream>

using namespace Solarus;

namespace {

/**
 * \brief Checks reading and writing a tileset data file.
 */
void check_tileset(TestEnvironment& /* env */, const std::string& tileset_id) {

  TilesetData tileset_data;
  bool success = false;

  // Import the tileset data file.
  std::string file_name = "tilesets/" + tileset_id + ".dat";
  std::string imported_tileset_buffer = QuestFiles::data_file_read(file_name);
  success = tileset_data.import_from_buffer(imported_tileset_buffer, file_name);
  Debug::check_assertion(success, "Tileset import failed");

  // Export it.
  std::string exported_tileset_buffer;
  success = tileset_data.export_to_buffer(exported_tileset_buffer);
  Debug::check_assertion(success, "Tileset export failed");

  // Check that the file is identical after import/export.
  if (exported_tileset_buffer != imported_tileset_buffer) {
    std::cerr << "Tileset '" << tileset_id << "' differs from the original one after export." << std::endl
        << "*** Original tileset file:" << std::endl << imported_tileset_buffer << std::endl
        << "*** Exported tileset file:" << std::endl << exported_tileset_buffer << std::endl;
    Debug::die("Tileset '" + tileset_id + "': exported file differs from the original one");
  }
}

}

/**
 * \brief Tests reading and writing tileset data files.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  const std::map<std::string, std::string>& tileset_elements =
      CurrentQuest::get_database().get_resource_elements(ResourceType::TILESET);
  Debug::check_assertion(!tileset_elements.empty(), "No tilesets");
  for (const auto& kvp : tileset_elements) {
    const std::string& tileset_id = kvp.first;
    check_tileset(env, tileset_id);
  }

  return 0;
}
