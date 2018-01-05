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
#include "solarus/ResourceProvider.h"

namespace Solarus {

/**
 * \brief Creates a resource provider.
 */
ResourceProvider::ResourceProvider() {
}

/**
 * \brief Provides the tileset with the given id.
 * \param tileset_id A tileset id.
 * \return The corresponding tileset.
 */
const Tileset& ResourceProvider::get_tileset(const std::string& tileset_id) {

  auto it = tileset_cache.find(tileset_id);
  if (it != tileset_cache.end()) {
    return *it->second;
  }

  it = tileset_cache.emplace(
        tileset_id,
        std::unique_ptr<Tileset>(new Tileset(tileset_id))
  ).first;
  Tileset& tileset = *it->second;
  tileset.load();
  return tileset;
}

/**
 * \brief Notifies the resource provider that cached data (if any) is no longer valid.
 *
 * This function must be called when a resource element has changed on disk.
 *
 * \param resource_type Type of resource that has changed.
 * \param element_id Resource element that has changed.
 */
void ResourceProvider::invalidate_resource_element(
    ResourceType resource_type,
    const std::string& element_id) {

  switch (resource_type) {

  case ResourceType::TILESET:
    tileset_cache.erase(element_id);
    break;

  default:
    break;
  }
}

}
