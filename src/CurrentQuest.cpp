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
#include "solarus/CurrentQuest.h"
#include "solarus/QuestResources.h"

namespace Solarus {

namespace CurrentQuest {

/**
 * \brief Reads the resource list file data file project_db.dat of the
 * current quest and stores it.
 */
void initialize() {

  // Read the quest resource list file.
  QuestResources& resources = get_resources();
  resources.import_from_quest_file("project_db.dat");
}

/**
 * \brief Clears the loaded quest resource list.
 */
void quit() {

  get_resources().clear();
}

/**
 * \brief Returns the resource list of the current quest.
 * \return The current quest resource list.
 */
QuestResources& get_resources() {

  // The resources object must be in a function to avoid static initialization
  // order problems.
  static QuestResources resources;
  return resources;
}

/**
 * \brief Returns whether there exists an element with the specified id.
 * \param resource_type A type of resource.
 * \param id The id to look for.
 * \return \c true if there resource_exists an element with the specified id in this
 * resource type.
 */
bool resource_exists(ResourceType resource_type, const std::string& id) {

  return get_resources().exists(resource_type, id);
}

/**
 * \brief Returns the list of element IDs of the specified resource type.
 * \param resource_type A type of resource.
 * \return The IDs of all declared element of this type, in their declaration
 * order.
 */
const std::map<std::string, std::string>&
get_resources(ResourceType resource_type) {

  return get_resources().get_elements(resource_type);
}

}

}
