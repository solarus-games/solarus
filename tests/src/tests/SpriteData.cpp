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
#include "solarus/core/Debug.h"
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/QuestDatabase.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/ResourceType.h"
#include "solarus/graphics/SpriteData.h"
#include "test_tools/TestEnvironment.h"
#include <iostream>

using namespace Solarus;

namespace {

/**
 * \brief Checks reading and writing a sprite data file.
 */
void check_sprite(TestEnvironment& /* env */, const std::string& sprite_id) {

  SpriteData sprite_data;
  bool success = false;

  // Import the sprite data file.
  std::string file_name = "sprites/" + sprite_id + ".dat";
  std::string imported_sprite_buffer = QuestFiles::data_file_read(file_name);
  success = sprite_data.import_from_buffer(imported_sprite_buffer, file_name);
  Debug::check_assertion(success, "Sprite import failed");

  // Export it.
  std::string exported_sprite_buffer;
  success = sprite_data.export_to_buffer(exported_sprite_buffer);
  Debug::check_assertion(success, "Sprite export failed");

  // Check that the file is identical after import/export.
  if (exported_sprite_buffer != imported_sprite_buffer) {
    std::cerr << "Sprite '" << sprite_id << "' differs from the original one after export." << std::endl
        << "*** Original sprite file:" << std::endl << imported_sprite_buffer << std::endl
        << "*** Exported sprite file:" << std::endl << exported_sprite_buffer << std::endl;
    Debug::die("Sprite '" + sprite_id + "': exported file differs from the original one");
  }
}

}

/**
 * \brief Tests reading and writing sprite data files.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  const std::map<std::string, std::string>& sprite_elements =
      CurrentQuest::get_database().get_resource_elements(ResourceType::SPRITE);
  Debug::check_assertion(!sprite_elements.empty(), "No sprites");
  for (const auto& kvp : sprite_elements) {
    const std::string& sprite_id = kvp.first;
    std::string file_name = "sprites/" + sprite_id + ".dat";
    if (QuestFiles::data_file_exists(file_name)) {
      check_sprite(env, sprite_id);
    }
  }

  return 0;
}
