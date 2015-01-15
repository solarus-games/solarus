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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/MapData.h"
#include "test_tools/TestEnvironment.h"

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
  Debug::check_assertion(exported_map_buffer == imported_map_buffer,
      "Exported map differs from the original one");

}

}

/**
 * \brief Tests reading and writing map data files.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);
  check_map(env, "basic_test");

  return 0;
}
