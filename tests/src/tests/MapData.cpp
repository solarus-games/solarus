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
#include "solarus/core/QuestDatabase.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/MapData.h"
#include "test_tools/TestEnvironment.h"
#include <iostream>

using namespace Solarus;

namespace {

/**
 * Checks writing to text and reading again some existing entity data.
 */
void check_entity(const EntityData& entity) {

  // Export the entity to a string and import it again.
  std::string exported_entity_buffer;
  bool success = entity.export_to_buffer(exported_entity_buffer);
  Debug::check_assertion(success, "Entity export failed");

  EntityData imported_entity;
  success = imported_entity.import_from_buffer(exported_entity_buffer, "entity");
  Debug::check_assertion(success, "Entity import failed");

  Debug::check_assertion(imported_entity.get_type() == entity.get_type(), "Entity type differs");
  Debug::check_assertion(imported_entity.get_name() == entity.get_name(), "Entity name differs");
  Debug::check_assertion(imported_entity.get_layer() == entity.get_layer(), "Entity layer differs");
  Debug::check_assertion(imported_entity.get_xy() == entity.get_xy(), "Entity xy differs");
  Debug::check_assertion(imported_entity.get_user_properties() == entity.get_user_properties(), "Entity user properties differ");
  Debug::check_assertion(imported_entity.get_specific_properties() == entity.get_specific_properties(), "Entity specific properties differ");
}

/**
 * \brief Checks reading and writing a map data file.
 */
void check_map(TestEnvironment& /* env */, const std::string& map_id) {

  MapData map_data;
  bool success = false;

  // Import the map data file.
  std::string file_name = "maps/" + map_id + ".dat";
  std::string imported_map_buffer = QuestFiles::data_file_read(file_name);
  success = map_data.import_from_buffer(imported_map_buffer, file_name);
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

  // Then export and import every entity of the map.
  for (int layer = map_data.get_min_layer(); layer <= map_data.get_max_layer(); ++layer) {
    for (int j = 0; j < map_data.get_num_entities(layer); ++j) {
      EntityIndex index = { layer, j };
      const EntityData& entity = map_data.get_entity(index);
      check_entity(entity);
    }
  }
}

}

/**
 * \brief Tests reading and writing map data files.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  const std::map<std::string, std::string>& map_elements =
      CurrentQuest::get_database().get_resource_elements(ResourceType::MAP);
  Debug::check_assertion(!map_elements.empty(), "No maps");
  for (const auto& kvp : map_elements) {
    const std::string& map_id = kvp.first;
    check_map(env, map_id);
  }

  return 0;
}
