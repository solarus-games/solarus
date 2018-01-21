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
#include "solarus/core/MapData.h"
#include "solarus/entities/EntityTypeInfo.h"
#include "solarus/lua/LuaTools.h"
#include <ostream>
#include <sstream>

namespace Solarus {

/**
 * \brief Creates an empty map data object.
 */
MapData::MapData():
    min_layer(0),
    max_layer(-1),
    size(0, 0),
    world(),
    location(0, 0),
    floor(NO_FLOOR),
    tileset_id(),
    music_id("none"),
    entities(),
    named_entities() {

  set_min_layer(0);
  set_max_layer(0);
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
 * \brief Returns the lowest layer of the map.
 * \return The lowest layer.
 */
int MapData::get_min_layer() const {
  return min_layer;
}

/**
 * \brief Changes the lowest layer of the map.
 * \param min_layer The new lowest layer.
 * If you increase it, entities in removed layers are destroyed.
 */
void MapData::set_min_layer(int min_layer) {

  Debug::check_assertion(min_layer <= 0, "The min layer should be lower than or equal to 0");

  if (min_layer == this->min_layer) {
    // No change.
    return;
  }

  if (min_layer > this->min_layer) {
    // Removing some layers: entities may be removed.
    for (int layer = this->min_layer; layer < min_layer; ++layer) {

      // Update the entities by layer structure.
      entities.erase(layer);

      // Update the entities by name structure.
      for (const EntityData& entity : entities[layer].entities) {
        if (entity.has_name()) {
          named_entities.erase(entity.get_name());
        }
      }
    }
  }
  else {
    // Adding layers.
    for (int layer = this->min_layer - 1; layer >= min_layer; --layer) {
      entities.emplace(layer, EntityDataList());
    }
  }

  this->min_layer = min_layer;
}

/**
 * \brief Returns the highest layer of the map.
 * \return The highest layer.
 */
int MapData::get_max_layer() const {
  return max_layer;
}

/**
 * \brief Changes the highest layer of the map.
 * \param max_layer The new highest layer.
 * If you decrease it, entities in removed layers are destroyed.
 */
void MapData::set_max_layer(int max_layer) {

  Debug::check_assertion(max_layer >= 0, "The max layer should be higher than or equal to 0");

  if (max_layer == this->max_layer) {
    // No change.
    return;
  }

  if (max_layer < this->max_layer) {
    // Removing some layers: entities may be removed.
    for (int layer = this->max_layer; layer > max_layer; --layer) {

      // Update the entities by layer structure.
      entities.erase(layer);

      // Update the entities by name structure.
      for (const EntityData& entity : entities[layer].entities) {
        if (entity.has_name()) {
          named_entities.erase(entity.get_name());
        }
      }
    }
  }
  else {
    // Adding layers.
    for (int layer = this->max_layer + 1; layer <= max_layer; ++layer) {
      entities.emplace(layer, EntityDataList());
    }
  }

  this->max_layer = max_layer;
}

/**
 * \brief Returns whether the specified layer exists in the map.
 * \param layer The layer to check.
 * \return \c true if the layer is >= 0 and < get_num_layers().
 */
bool MapData::is_valid_layer(int layer) const {

  return layer >= get_min_layer() && layer <= get_max_layer();
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
 * \brief Returns whether a music is set.
 * \return \c true if the music id is not "none".
 */
bool MapData::has_music() const {
  return music_id != "none";
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
  for (int layer = get_min_layer(); layer <= get_max_layer(); ++layer) {
    num_entities += get_num_entities(layer);
  }

  return num_entities;
}

/**
 * \brief Returns the number of entities on a layer of this map.
 * \param layer A layer.
 * \return The number of entities on that layer.
 */
int MapData::get_num_entities(int layer) const {
  return get_entities(layer).size();
}

/**
 * \brief Returns the number of tiles on a layer of this map.
 * \param layer A layer.
 * \return The number of tiles on that layer.
 */
int MapData::get_num_tiles(int layer) const {

  Debug::check_assertion(is_valid_layer(layer), "Invalid layer");

  return entities.at(layer).num_tiles;
}

/**
 * \brief Returns the number of dynamic entities on a layer of this map.
 * \param layer A layer.
 * \return The number of dynamic entities on that layer.
 */
int MapData::get_num_dynamic_entities(int layer) const {
  return get_num_entities(layer) - get_num_tiles(layer);
}

/**
 * \brief Returns the entities on a layer of this map.
 * \param layer A layer.
 * \return The entities on that layer.
 */
const std::deque<EntityData>& MapData::get_entities(int layer) const {

  Debug::check_assertion(is_valid_layer(layer), "Invalid layer");

  return entities.at(layer).entities;
}

/**
 * \overload
 *
 * Non-const version.
 */
std::deque<EntityData>& MapData::get_entities(int layer) {

  Debug::check_assertion(is_valid_layer(layer), "Invalid layer");

  return entities.at(layer).entities;
}

/**
 * \brief Changes the layer of an entity.
 * \param src_index The current index of the entity to change.
 * \param dst_layer The new layer to set.
 * \return The new index of the entity.
 */
EntityIndex MapData::set_entity_layer(const EntityIndex& src_index, int dst_layer) {

  Debug::check_assertion(is_valid_layer(dst_layer), "Invalid layer in MapData::set_entity_layer()");

  int src_layer = src_index.layer;
  if (dst_layer == src_index.layer) {
    // No change.
    return src_index;
  }

  // Update the entity itself, but also entities and named_entities.
  const auto& src_it = get_entities(src_layer).begin() + src_index.order;
  EntityData& entity = *src_it;
  entity.set_layer(dst_layer);

  // Insert on top of existing entities in the destination layer.
  const bool dynamic = entity.is_dynamic();
  int dst_order = dynamic ? get_num_entities(dst_layer) : get_num_tiles(dst_layer);
  EntityIndex dst_index(dst_layer, dst_order);

  if (entity.has_name()) {
    named_entities[entity.get_name()] = dst_index;
  }

  auto dst_it = get_entities(dst_layer).begin() + dst_order;
  get_entities(dst_layer).insert(dst_it, entity);
  get_entities(src_layer).erase(src_it);  // Erase after insert because entity is a ref.

  if (!dynamic) {
    // The tile count has changed.
    ++entities[dst_layer].num_tiles;
    --entities[src_layer].num_tiles;
  }

  return dst_index;
}

/**
 * \brief Changes the order of an entity in its layer.
 * \param src_index The current index of the entity to change.
 * \param dst_order The new order to set.
 * It must be valid: in particular, tiles must remain before dynamic entities.
 */
void MapData::set_entity_order(const EntityIndex& src_index, int dst_order) {

  int layer = src_index.layer;
  int src_order = src_index.order;

  if (dst_order == src_order) {
    // No change.
    return;
  }

  EntityData entity = get_entity(src_index);  // Make a copy.
  bool dynamic = entity.is_dynamic();
  int min_order = dynamic ? get_num_tiles(layer) : 0;
  int max_order = dynamic ? (get_num_entities(layer) - 1) : (get_num_tiles(layer) - 1);

  Debug::check_assertion(dst_order >= min_order, "Entity order out of range (lower than min)");
  Debug::check_assertion(dst_order <= max_order, "Entity order out of range (higher than max)");

  std::deque<EntityData>& entities = get_entities(layer);

  // Update entities and named_entities.
  auto src_it = entities.begin() + src_order;

  if (entity.has_name()) {
    named_entities[entity.get_name()] = { layer, dst_order };
  }

  entities.erase(src_it);
  auto dst_it = entities.begin() + dst_order;
  entities.insert(dst_it, entity);

  if (dst_order < src_order) {
    // Moving downwards.
    // Indexes between dst_order exclusive and src_order inclusive get incremented.
    for (int i = dst_order + 1;
        i <= src_order;
        ++i
    ) {
      const EntityData& current_entity = get_entity({ layer, i });
      const std::string& name = current_entity.get_name();
      if (!name.empty() && name != entity.get_name()) {
        EntityIndex& index = named_entities[name];
        ++index.order;
      }
    }
  }
  else {
    // Moving upwards.
    // Indexes between src_order inclusive and dst_order exclusive get decremented.
    for (int i = src_order;
        i < dst_order;
        ++i
    ) {
      const EntityData& current_entity = get_entity({ layer, i });
      const std::string& name = current_entity.get_name();
      if (!name.empty() && name != entity.get_name()) {
        EntityIndex& index = named_entities[name];
        --index.order;
      }
    }
  }
}

/**
 * \brief Brings an entity to the front on its layer.
 *
 * If it is a tile, it will still be below all dynamic entities.
 *
 * \param index Index of the entity on the map.
 * \return The new index of the entity.
 */
EntityIndex MapData::bring_entity_to_front(const EntityIndex& index) {

  const EntityData& entity = get_entity(index);
  int bound = entity.is_dynamic() ? get_num_entities(index.layer) : get_num_tiles(index.layer);

  EntityIndex dst_index(index.layer, bound - 1);
  Debug::check_assertion(index.order <= dst_index.order, "Entity index out of range");
  if (dst_index.order == index.order) {
    // Already to the front.
    return index;
  }

  set_entity_order(index, dst_index.order);
  return dst_index;
}

/**
 * \brief Brings an entity to the back on its layer.
 *
 * If it is a dynamic entity, it will still be above all tiles.
 *
 * \param index Index of the entity on the map.
 * \return The new index of the entity.
 */
EntityIndex MapData::bring_entity_to_back(const EntityIndex& index) {

  const EntityData& entity = get_entity(index);
  int bound = entity.is_dynamic() ? get_num_tiles(index.layer) : 0;

  EntityIndex dst_index(index.layer, bound);
  Debug::check_assertion(index.order >= dst_index.order, "Entity index out of range");
  if (dst_index.order == index.order) {
    // Already to the back.
    return index;
  }

  set_entity_order(index, dst_index.order);
  return dst_index;
}

/**
 * \brief Returns whether there is an entity at the given index.
 * \param index An index.
 * \return \c true if there is an entity at this index.
 */
bool MapData::entity_exists(const EntityIndex& index) const {

  return index.order >= 0 &&
      index.order < get_num_entities(index.layer);
}

/**
 * \brief Returns the entity at the given index.
 * \param index Index of the entity to get on the map.
 * \return The entity data.
 * The object remains valid until entities are added or removed.
 */
const EntityData& MapData::get_entity(const EntityIndex& index) const {

  Debug::check_assertion(entity_exists(index),
      "Entity index out of range"
  );

  return get_entities(index.layer)[index.order];
}

/**
 * \brief Returns the entity at the given layer and index.
 *
 * Non-const version.
 *
 * \param index Index of the entity to get on the map.
 * \return The entity data.
 * The object remains valid until entities are added or removed.
 */
EntityData& MapData::get_entity(const EntityIndex& index) {

  Debug::check_assertion(entity_exists(index),
      "Entity index out of range"
  );

  return get_entities(index.layer)[index.order];
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
 * \brief Returns all entities that have a name on the map.
 * \return The named entities.
 */
const std::map<std::string, EntityIndex>& MapData::get_named_entities_indexes() const {
  return named_entities;
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
 * The entity will be added in front of other ones that are on the same layer.
 * Use insert_entity() if you want to insert it at a specific index.
 *
 * If the new entity has a name, it should be unique on the map.
 *
 * \param entity The information of an entity.
 * \return The index of this entity on the map.
 * Returns an invalid index in case of failure, that is,
 * if the name was already in use or if the entity type or layer is illegal.
 */
EntityIndex MapData::add_entity(const EntityData& entity) {

  if (!is_valid_layer(entity.get_layer())) {
    // Illegal layer.
    return EntityIndex();
  }

  // Compute the appropriate index.
  int layer = entity.get_layer();
  int bound = entity.is_dynamic() ? get_num_entities(layer) : get_num_tiles(layer);
  EntityIndex index = { layer, bound };

  // Insert the entity there.
  if (!insert_entity(entity, index)) {
    // Failure.
    return EntityIndex();
  }
  return index;
}

/**
 * \brief Adds an entity to the map at the specified index.
 *
 * If the new entity has a name, it should be unique on the map.
 *
 * \param entity The information of an entity.
 * \param index The index this entity should have on the map.
 * \return \c false in case of failure, that is,
 * if the name was already in use, if the entity type is illegal
 * or if the index was out of range.
 */
bool MapData::insert_entity(const EntityData& entity, const EntityIndex& index) {

  if (!EntityTypeInfo::can_be_stored_in_map_file(entity.get_type())) {
    // Illegal type of entity in a map file.
    return false;
  }

  const int layer = index.layer;
  int order = index.order;
  bool dynamic = entity.is_dynamic();
  int min_order = dynamic ? get_num_tiles(layer) : 0;
  int max_order = dynamic ? get_num_entities(layer) : get_num_tiles(layer);

  if (order < min_order || order > max_order) {
    // Index out of range.
    return false;
  }

  if (entity.has_name()) {
    if (entity_exists(entity.get_name())) {
      // This name is already used by another entity.
      return false;
    }

    named_entities.emplace(entity.get_name(), index);
  }

  if (!dynamic) {
    ++this->entities[layer].num_tiles;
  }

  std::deque<EntityData>& entities = get_entities(layer);
  auto it = entities.begin() + order;
  entities.emplace(it, entity);

  // Indexes after this one get shifted.
  for (it = entities.begin() + order + 1;
      it != entities.end();
      ++it
  ) {
    const EntityData& current_entity = *it;
    const std::string& name = current_entity.get_name();
    if (!name.empty()) {
      EntityIndex& index = named_entities[name];
      ++index.order;
    }
  }

  return true;
}

/**
 * \brief Removes an entity from the map.
 * \param index Index of the entity on the map.
 * \return \c false in case of failure, that is,
 * if the index is out of range.
 */
bool MapData::remove_entity(const EntityIndex& index) {

  if (!entity_exists(index)) {
    return false;
  }

  int layer = index.layer;
  int order = index.order;
  const EntityData& entity = get_entity(index);
  bool dynamic = entity.is_dynamic();

  if (entity.has_name()) {
    named_entities.erase(entity.get_name());
  }

  if (!dynamic) {
    --this->entities[layer].num_tiles;
  }

  std::deque<EntityData>& entities = get_entities(layer);
  auto it = entities.begin() + order;
  entities.erase(it);

  // Indexes after this one get shifted.
  for (it = entities.begin() + order;
      it != entities.end();
      ++it) {
    const EntityData& current_entity = *it;
    const std::string& name = current_entity.get_name();
    if (!name.empty()) {
      EntityIndex& index = named_entities[name];
      --index.order;
    }
  }
  return true;
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
    EntityType type = LuaTools::check_enum<EntityType>(
        l, lua_upvalueindex(1)
    );
    const EntityData& entity = EntityData::check_entity_data(l, 1, type);

    if (!map.is_valid_layer(entity.get_layer())) {
      std::ostringstream oss;
      oss << "Invalid layer: " << entity.get_layer();
      LuaTools::error(l, oss.str());
    }

    EntityIndex index = map.add_entity(entity);
    if (!index.is_valid()) {
      LuaTools::error(l, "Failed to add entity");
    }

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
    const int min_layer = LuaTools::check_int_field(l, 1, "min_layer");
    const int max_layer = LuaTools::check_int_field(l, 1, "max_layer");
    const std::string& world = LuaTools::opt_string_field(l, 1 , "world", "");
    const int floor = LuaTools::opt_int_field(l, 1, "floor", MapData::NO_FLOOR);
    const std::string& tileset_id = LuaTools::check_string_field(l, 1, "tileset");
    const std::string& music_id = LuaTools::opt_string_field(l, 1, "music", "none");

    if (min_layer > 0) {
      LuaTools::arg_error(l, 1, "min_layer must be lower than or equal to 0");
    }
    if (max_layer < 0) {
      LuaTools::arg_error(l, 1, "max_layer must be higher than or equal to 0");
    }

    // Initialize the map data.
    map.set_location({ x, y });
    map.set_size({ width, height });
    map.set_min_layer(min_layer);
    map.set_max_layer(max_layer);
    map.set_music_id(music_id);
    map.set_world(world);
    map.set_floor(floor);
    map.set_tileset_id(tileset_id);

    // Properties are set: we now allow the data file to declare entities.

    for (const auto& kvp : EntityData::get_entity_type_descriptions()) {
      const std::string& type_name = enum_to_name(kvp.first);
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
bool MapData::export_to_lua(std::ostream& out) const {

  // Write map properties.
  out << "properties{\n"
      << "  x = " << get_location().x << ",\n"
      << "  y = " << get_location().y << ",\n"
      << "  width = " << get_size().width << ",\n"
      << "  height = " << get_size().height << ",\n"
      << "  min_layer = " << get_min_layer() << ",\n"
      << "  max_layer = " << get_max_layer() << ",\n";
  if (has_world()) {
    out << "  world = \"" << escape_string(get_world()) << "\",\n";
  }
  if (has_floor()) {
    out << "  floor = " << get_floor() << ",\n";
  }
  out << "  tileset = \"" << escape_string(get_tileset_id()) << "\",\n";
  if (has_music()) {
    out << "  music = \"" << escape_string(get_music_id()) << "\",\n";
  }
  out << "}\n\n";

  for (const auto& kvp : entities) {
    const EntityDataList& layer_entities = kvp.second;
    for (const EntityData& entity_data : layer_entities.entities) {
      bool success = entity_data.export_to_lua(out);
      Debug::check_assertion(success, "Entity export failed");
    }
  }

  return true;
}

}  // namespace Solarus

