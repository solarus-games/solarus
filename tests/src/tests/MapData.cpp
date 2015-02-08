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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/CurrentQuest.h"
#include "solarus/MapData.h"
#include "test_tools/TestEnvironment.h"
#include <iostream>

using namespace Solarus;

namespace {

/**
 * \brief Checks reading and writing a map data file.
 */
void check_map(TestEnvironment& /* env */, const std::string& map_id) {

  MapData map_data;
  bool success = false;

  // Import the map data file.
  std::string file_name = "maps/" + map_id + ".dat";
  std::string imported_map_buffer = QuestFiles::data_file_read(file_name);
  success = map_data.import_from_buffer(imported_map_buffer);
  Debug::check_assertion(success, "Map import failed");

  // Export it.
  std::string exported_map_buffer;
  success = map_data.export_to_buffer(exported_map_buffer);
  Debug::check_assertion(success, "Map export failed");

  // Check that the file is identical after import/export.
  if (exported_map_buffer != imported_map_buffer) {
    std::cerr << "Map '" << map_id << "' differs from the original one after export." << std::endl
        << "*** Original map file:" << std::endl << imported_map_buffer << std::endl
        << "*** Exported map file:" << std::endl << exported_map_buffer << std::endl;
    Debug::die("Map '" + map_id + "': exported file differs from the original one");
  }
}

}

/**
 * \brief Tests reading and writing map data files.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  const std::map<std::string, std::string>& map_elements =
      CurrentQuest::get_resources().get_elements(ResourceType::MAP);
  Debug::check_assertion(!map_elements.empty(), "No sprites");
  for (const auto& kvp : map_elements) {
    const std::string& map_id = kvp.first;
    check_map(env, map_id);
  }

  return 0;
}
