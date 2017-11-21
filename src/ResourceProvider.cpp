/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/shaders/ShaderContext.h"

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
Tileset& ResourceProvider::get_tileset(const std::string& tileset_id) {

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
 * \brief Provides the shader with the given id.
 * \param shader_id A shader id.
 * \return The corresponding shader, or nullptr if such a shader does not exist
 * or if it failed to compile, or if shaders are not supported.
 */
Shader* ResourceProvider::get_shader(const std::string& shader_id) {

  auto it = shader_cache.find(shader_id);
  if (it != shader_cache.end()) {
    return it->second.get();
  }

  std::unique_ptr<Shader> shader = ShaderContext::create_shader(shader_id);
  if (shader == nullptr) {
    return nullptr;
  }

  it = shader_cache.emplace(
        shader_id,
        std::move(shader)
  ).first;
  return it->second.get();
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
