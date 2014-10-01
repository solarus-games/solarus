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
#include "solarus/lowlevel/FileTools.h"
#include "solarus/CurrentQuestResources.h"
#include "solarus/QuestResources.h"

namespace Solarus {

namespace {

QuestResources resources;  /**< Resource list of the current quest. */

}

/**
 * \brief Reads the resource list file data file project_db.dat of the
 * current quest and stores it.
 */
void CurrentQuestResources::initialize() {

  // Read the quest resource list file.
  const std::string& file_name = "project_db.dat";
  const std::string& buffer = FileTools::data_file_read(file_name);
  resources.load_from_buffer(buffer);
}

/**
 * \brief Clears the loaded quest resource list.
 */
void CurrentQuestResources::quit() {

  resources.clear();
}

/**
 * \brief Returns whether there exists an element with the specified id.
 * \param resource_type A type of resource.
 * \param id The id to look for.
 * \return \c true if there exists an element with the specified id in this
 * resource type.
 */
bool CurrentQuestResources::exists(ResourceType resource_type, const std::string& id) {

  return resources.exists(resource_type, id);
}

/**
 * \brief Returns the list of element IDs of the specified resource type.
 * \param resource_type A type of resource.
 * \return The IDs of all declared element of this type, in their declaration
 * order.
 */
const std::map<std::string, std::string>&
CurrentQuestResources::get_elements(ResourceType resource_type) {

  return resources.get_elements(resource_type);
}

}

