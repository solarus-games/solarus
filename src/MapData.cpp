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
#include "solarus/entities/MapEntity.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/MapData.h"

namespace Solarus {

/**
 * \brief Creates an empty map data object.
 */
MapData::MapData():
    size(0, 0),
    world(),
    location(0, 0),
    floor(NO_FLOOR),
    tileset_id(),
    music_id("none"),
    entities(),
    named_entities() {

}

/**
 * \brief Returns the size of the map.
 * \return The map size in pixels.
 */
Size MapData::get_size() const {
  return size;
}

/**
 * \brief Sets the size of the map.
 * \param size The map size in pixels.
 */
void MapData::set_size(const Size& size) {
  this->size = size;
}

/**
 * \brief Returns the location of this map in its world.
 * \return The location of the upper-left corner of the map in pixels.
 */
Point MapData::get_location() const {
  return location;
}

/**
 * \brief Sets the location of this map in its world.
 * \param location The location of the upper-left corner of the map in pixels.
 */
void MapData::set_location(const Point& location) {
  this->location = location;
}

/**
 * \brief Returns whether this map is in a world.
 * \return \c true if the map is in a world.
 */
bool MapData::has_world() const {
  return !world.empty();
}

/**
 * \brief Returns the world this maps belongs to.
 * \return The world or an empty string.
 */
const std::string& MapData::get_world() const {
  return world;
}

/**
 * \brief Sets the world this maps belongs to.
 * \param world The world or an empty string.
 */
void MapData::set_world(const std::string& world) {
  this->world = world;
}

/**
 * \brief Returns whether this map has a floor number.
 * \return \c true if the map has a floor number.
 */
bool MapData::has_floor() const {
  return floor != NO_FLOOR;
}

/**
 * \brief Returns the floor number of this map.
 * \return The floor number or Map::NO_FLOOR.
 */
int MapData::get_floor() const {
  return floor;
}

/**
 * \brief Sets the floor number of this map.
 * \param floor The floor number or Map::NO_FLOOR.
 */
void MapData::set_floor(int floor) {
  this->floor = floor;
}

/**
 * \brief Returns the tileset used to draw this map.
 * \return The tileset id.
 * An empty string means that the tileset is not set yet.
 */
const std::string& MapData::get_tileset_id() const {
  return tileset_id;
}

/**
 * \brief Sets the tileset used to draw this map.
 * \param tileset_id The tileset id.
 * An empty string means that the tileset is not set yet.
 */
void MapData::set_tileset_id(const std::string& tileset_id) {
  this->tileset_id = tileset_id;
}

/**
 * \brief Returns the music played on this map.
 * \return The music id or "none" or "same".
 */
const std::string& MapData::get_music_id() const {
  return music_id;
}

/**
 * \brief Sets the music played on this map.
 * \param music_id The music id or "none" or "same".
 */
void MapData::set_music_id(const std::string& music_id) {
  this->music_id = music_id;
}

/**
 * \brief Returns the total number of entities on this map.
 * \return The number of entities.
 */
int MapData::get_num_entities() const {

  int num_entities = 0;
  for (const EntityList& layer_entities : entities) {
    num_entities += layer_entities.size();
  }

  return num_entities;
}

/**
 * \brief Returns the number of entities on a layer of this map.
 * \param layer A layer.
 * \return The number of entities on that layer.
 */
int MapData::get_num_entities(Layer layer) const {
  return entities[layer].size();
}

/**
 * \brief Changes the layer of an entity.
 * \param src_index The current index of the entity to change.
 * \param dst_layer The new layer to set.
 */
void MapData::set_entity_layer(const EntityIndex& src_index, Layer dst_layer) {

  // Update the entity itself, but also entities and named_entities.
  const auto& it = entities[src_index.layer].begin() + src_index.index;
  EntityData& entity = *it;
  entity.set_layer(dst_layer);

  if (entity.has_name()) {
    int dst_index = entities[dst_layer].size();
    named_entities[entity.get_name()] = { dst_layer, dst_index };
  }

  entities[dst_layer].push_back(*it);  // Insert after existing entities on the destination layer.
  entities[src_index.layer].erase(it);
}

/**
 * \brief Brings an entity to front on its layer.
 * \param index Index of the entity on the map.
 */
void MapData::bring_entity_to_front(const EntityIndex& index) {

  Layer layer = index.layer;
  if (index.index == (int) (entities[layer].size() - 1)) {
    // Already to the front.
    return;
  }

  const auto& it = entities[layer].begin() + index.index;
  const EntityData& entity = *it;

  if (entity.has_name()) {
    named_entities[entity.get_name()] = {
        layer,
        (int) (entities[layer].size() - 1)
    };
  }

  entities[layer].push_back(*it);
  entities[layer].erase(it);
}

/**
 * \brief Brings an entity to back on its layer.
 * \param index Index of the entity on the map.
 */
void MapData::bring_entity_to_back(const EntityIndex& index) {

  Layer layer = index.layer;
  if (index.index == 0) {
    // Already to the back.
    return;
  }

  const auto& it = entities[layer].begin() + index.index;
  const EntityData& entity = *it;

  if (entity.has_name()) {
    named_entities[entity.get_name()] = { layer, 0 };
  }

  entities[layer].push_front(*it);
  entities[layer].erase(it);
}

/**
 * \brief Returns the entity at the given index.
 * \param index Index of the entity to get on the map.
 * \return The entity data.
 * The object remains valid until entities are added or removed.
 */
const EntityData& MapData::get_entity(const EntityIndex& index) const {

  Debug::check_assertion(
      index.index >= 0 && index.index < get_num_entities(index.layer),
      "Entity index out of range"
  );

  return entities[index.layer][index.index];
}

/**
 * \brief Returns the entity at the given layer and index.
 *
 * Non-const version.
 *
 * \param layer The layer of the entity to get.
 * \param index Index of the entity in that layer.
 * \return The entity data.
 * The object remains valid until entities are added or removed.
 */
EntityData& MapData::get_entity(const EntityIndex& index) {

  Debug::check_assertion(
      index.index >= 0 && index.index < get_num_entities(index.layer),
      "Entity index out of range"
  );

  return entities[index.layer][index.index];
}

/**
 * \brief Returns the layer and index of an entity given its name.
 * \param name Name of the entity to get.
 * \return The layer and index of this entity.
 * Returns an invalid index if there is no such entity.
 */
EntityIndex MapData::get_entity_index(const std::string& name) const {

  const auto& it = named_entities.find(name);

  if (it == named_entities.end()) {
    return EntityIndex();
  }

  return it->second;
}

/**
 * \brief Returns an entity given its name.
 * \param name Name of the entity to get.
 * \return The entity or nullptr if there is no such entity.
 * The object remains valid until entities are added or removed.
 */
const EntityData* MapData::get_entity_by_name(const std::string& name) const {

  const EntityIndex& index = get_entity_index(name);
  if (!index.is_valid()) {
    // No such entity.
    return nullptr;
  }
  return &get_entity(index);
}

/**
 * \brief Returns an entity given its name.
 *
 * Non-const version. Ownership of the pointer remains to MapData.
 *
 * \param name Name of the entity to get.
 * \return The entity or nullptr if there is no such entity.
 * The object remains valid until entities are added or removed.
 */
EntityData* MapData::get_entity_by_name(const std::string& name) {

  const EntityIndex& index = get_entity_index(name);
  if (!index.is_valid()) {
    // No such entity.
    return nullptr;
  }
  return &get_entity(index);
}

/**
 * \brief Returns whether there exists an entity with the specified name.
 * \param name The name to check.
 * \return \c true if there exists an entity with this name on the map.
 */
bool MapData::entity_exists(const std::string& name) const {

  return named_entities.find(name) != named_entities.end();
}

/**
 * \brief Changes the name of an entity.
 * \param index Index of the entity on the map.
 * \param name The new name. An empty string means no name.
 * \return \c true in case of success, \c false if the name was already used.
 */
bool MapData::set_entity_name(const EntityIndex& index, const std::string& name) {

  EntityData& entity = get_entity(index);
  const std::string& old_name = entity.get_name();

  if (name == old_name) {
    // Nothing to do.
    return true;
  }

  if (!name.empty()) {
    if (entity_exists(name)) {
      // This name is already used by another entity.
      return false;
    }
  }

  // Do the change.
  entity.set_name(name);

  if (!old_name.empty()) {
    named_entities.erase(old_name);
  }

  if (!name.empty()) {
    named_entities[name] = index;
  }

  return false;
}

/**
 * \brief Adds an entity to the map.
 *
 * If the new entity has a name, it should be unique on the map.
 *
 * \param entity The information of an entity.
 * \return The index of this entity on the map.
 * Returns an invalid index in case of failure, that is,
 * if the name was already in use.
 */
EntityIndex MapData::add_entity(const EntityData& entity) {

  Layer layer = entity.get_layer();
  EntityIndex index = { layer, (int) entities[layer].size() };

  if (entity.has_name()) {
    if (entity_exists(entity.get_name())) {
      // This name is already used by another entity.
      return EntityIndex();
    }

    named_entities.insert(std::make_pair(entity.get_name(), index));
  }

  entities[layer].push_back(entity);

  return index;
}

/**
 * \brief Removes an entity from the map.
 * \param index Index of the entity on the map.
 */
void MapData::remove_entity(const EntityIndex& index) {

  const EntityData& entity = get_entity(index);

  if (entity.has_name()) {
    named_entities.erase(entity.get_name());
  }

  Layer layer = index.layer;
  const auto& it = entities[layer].begin() + index.index;
  entities[layer].erase(it);
}

namespace {

/**
 * \brief Implementation of all entity creation functions for the map data file.
 *
 * The type of entity to create is indicated by the first upvalue.
 * It will be added to the MapData.
 */
int l_add_entity(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    // Retrieve the map data to build.
    lua_getfield(l, LUA_REGISTRYINDEX, "map");
    MapData& map = *static_cast<MapData*>(lua_touserdata(l, -1));
    lua_pop(l, 1);

    // Get the type of entity to create.
    EntityType type = LuaTools::check_enum(
        l, lua_upvalueindex(1), MapEntity::entity_type_names
    );
    const EntityData& entity = EntityData::check_entity_data(l, -1, type);

    map.add_entity(entity);

    return 0;
  });
}

/**
 * \brief Implementation of the properties() function of the Lua map data file.
 *
 * Reads the properties of the map: location, size, tileset, music, etc.
 *
 * \param l The Lua state that is calling this function.
 * \return Number of values to return to Lua.
 */
int l_properties(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    // Retrieve the map data to build.
    lua_getfield(l, LUA_REGISTRYINDEX, "map");
    MapData& map = *static_cast<MapData*>(lua_touserdata(l, -1));
    lua_pop(l, 1);

    // Retrieve the map properties from the table parameter.
    LuaTools::check_type(l, 1, LUA_TTABLE);

    const int x = LuaTools::opt_int_field(l, 1, "x", 0);
    const int y = LuaTools::opt_int_field(l, 1, "y", 0);
    const int width = LuaTools::check_int_field(l, 1, "width");
    const int height = LuaTools::check_int_field(l, 1, "height");
    const std::string& world = LuaTools::opt_string_field(l, 1 , "world", "");
    const int floor = LuaTools::opt_int_field(l, 1, "floor", MapData::NO_FLOOR);
    const std::string& tileset_id = LuaTools::check_string_field(l, 1, "tileset");
    const std::string& music_id = LuaTools::opt_string_field(l, 1, "music", "none");

    // Initialize the map data.
    map.set_location({ x, y });
    map.set_size({ width, height });
    map.set_music_id(music_id);
    map.set_world(world);
    map.set_floor(floor);
    map.set_tileset_id(tileset_id);

    // Properties are set: we now allow the data file to declare entities.

    for (const auto& kvp : EntityData::get_entity_type_descriptions()) {
      const std::string& type_name = MapEntity::get_entity_type_name(kvp.first);
      lua_pushstring(l, type_name.c_str());
      lua_pushcclosure(l, l_add_entity, 1);
      lua_setglobal(l, type_name.c_str());
    }

    return 0;
  });
}

}  // Anonymous namespace

/**
 * \copydoc LuaData::import_from_lua
 */
bool MapData::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "map");
  lua_register(l, "properties", l_properties);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Failed to load map: ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool MapData::export_to_lua(std::ostream& /* out */) const {
  // TODO
  return false;
}

}  // namespace Solarus
