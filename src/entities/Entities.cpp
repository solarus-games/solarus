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
#include "solarus/entities/Boomerang.h"
#include "solarus/entities/CrystalBlock.h"
#include "solarus/entities/Destination.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/EntityTypeInfo.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include "solarus/entities/Separator.h"
#include "solarus/entities/SeparatorPtr.h"
#include "solarus/entities/Stairs.h"
#include "solarus/entities/Tile.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include <sstream>
#include <lua.hpp>

namespace Solarus {

namespace {

/**
 * \brief Comparator that sorts entities according to their stacking order
 * on the map (layer and then Z index).
 */
class ZOrderComparator {

  public:

    /**
     * \brief Creates a Z order comparator.
     * \param entities The map entities to work with.
     */
    explicit ZOrderComparator(const Entities& entities) :
        entities(entities) {

    }

    /**
     * \brief Compares two con entities.
     * \param first An entity.
     * \param second Another entity.
     * \return \c true if the first entity's Z index is lower than the second one's.
     */
    bool operator()(const ConstEntityPtr& first, const ConstEntityPtr& second) const {

      if (first->get_layer() < second->get_layer()) {
        return true;
      }

      if (first->get_layer() > second->get_layer()) {
        return false;
      }

      // Same layer.
      return entities.get_entity_relative_z_order(first) < entities.get_entity_relative_z_order(second);
    }

  private:

    const Entities& entities;

};

/**
 * \brief Comparator that sorts entities in drawing order.
 * on the map (layer and then Z index).
 */
class DrawingOrderComparator {

  public:

    /**
     * \brief Compares two entities.
     * \param first An entity.
     * \param second Another entity.
     * \return \c true if the first entity should be drawn before the secone one.
     */
    bool operator()(const EntityPtr& first, const EntityPtr& second) const {

      if (first->get_layer() < second->get_layer()) {
        return true;
      }

      if (first->get_layer() > second->get_layer()) {
        return false;
      }

      // Same layer.
      // All entities displayed in Z order are displayed before entities displayed in Y order.
      if (!first->is_drawn_in_y_order() && second->is_drawn_in_y_order()) {
        return true;
      }

      if (first->is_drawn_in_y_order() && !second->is_drawn_in_y_order()) {
        return false;
      }

      if (first->is_drawn_in_y_order()) {
        // Both entities are displayed in Y order.
        return first->get_y() < second->get_y();
      }

      // Both entities are displayed in Z order.
      const Entities& entities = first->get_entities();
      return entities.get_entity_relative_z_order(first) < entities.get_entity_relative_z_order(second);
    }

};

}  // Anonymous namespace.

/**
 * \brief Constructor.
 * \param game The game.
 * \param map The map.
 */
Entities::Entities(Game& game, Map& map):
  game(game),
  map(map),
  map_width8(0),
  map_height8(0),
  tiles_grid_size(0),
  tiles_ground(),
  non_animated_regions(),
  tiles_in_animated_regions(),
  hero(game.get_hero()),
  camera(nullptr),
  named_entities(),
  all_entities(),
  quadtree(),
  z_caches(),
  entities_drawn_not_at_their_position(),
  entities_to_draw(),
  entities_to_remove(),
  default_destination(nullptr) {

  // Initialize the size.
  initialize_layers();
  map_width8 = map.get_width8();
  map_height8 = map.get_height8();
  tiles_grid_size = map.get_width8() * map.get_height8();
  for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {

    Ground initial_ground = (layer == map.get_min_layer()) ? Ground::TRAVERSABLE : Ground::EMPTY;
    tiles_ground[layer].assign(tiles_grid_size, initial_ground);

    non_animated_regions[layer] = std::unique_ptr<NonAnimatedRegions>(
        new NonAnimatedRegions(map, layer)
    );
  }

  // Initialize the quadtree.
  const int margin = 64;
  Rectangle quadtree_space(-margin, -margin, map.get_width() + 2 * margin, map.get_height() + 2 * margin);
  quadtree.initialize(quadtree_space);

  // Create the camera.
  add_entity(std::make_shared<Camera>(map));
}

/**
 * \brief Creates live entities from the given data.
 */
void Entities::create_entities(const MapData& data) {

  // Create entities from the map data file.
  LuaContext& lua_context = map.get_lua_context();
  for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {
    for (int i = 0; i < data.get_num_entities(layer); ++i) {
      const EntityData& entity_data = data.get_entity({ layer, i });
      EntityType type = entity_data.get_type();
      if (!EntityTypeInfo::can_be_stored_in_map_file(type)) {
        Debug::error("Illegal entity type in map data: " + enum_to_name(type));
      }
      if (lua_context.create_map_entity_from_data(map, entity_data)) {
        lua_pop(lua_context.get_internal_state(), 1);  // Discard the created entity on the stack.
      }
    }
  }
}

/**
 * \brief Notifies an entity that it is being removed.
 * \param entity The entity being removed.
 */
void Entities::notify_entity_removed(Entity& entity) {

  if (!entity.is_being_removed()) {
    entity.notify_being_removed();
  }
}

/**
 * \brief Returns the hero.
 * \return The hero.
 */
Hero& Entities::get_hero() {
  return *hero;
}

/**
 * \brief Returns all entities expect tiles.
 * \return The entities except tiles.
 */
EntityVector Entities::get_entities() {

  EntityVector result;
  result.insert(result.begin(), all_entities.begin(), all_entities.end());
  return result;
}

/**
 * \brief Returns the default destination of the map.
 * \return The default destination, or nullptr if there exists no destination
 * on this map.
 */
const std::shared_ptr<Destination>& Entities::get_default_destination() {
  return default_destination;
}

/**
 * \brief Sets the tile ground property of an 8*8 square of the map.
 *
 * Coordinates outside the range of the map are not an error:
 * in this case, this function does nothing.
 *
 * \param layer Layer of the square.
 * \param x8 X coordinate of the square (divided by 8).
 * \param y8 Y coordinate of the square (divided by 8).
 * \param ground The ground property to set.
 */
void Entities::set_tile_ground(int layer, int x8, int y8, Ground ground) {

  if (x8 >= 0 && x8 < map_width8 && y8 >= 0 && y8 < map_height8) {
    int index = y8 * map_width8 + x8;
    tiles_ground[layer][index] = ground;
  }
}

/**
 * \brief Returns the entity with the specified name.
 *
 * The program stops if there is no such entity.
 *
 * \param name Name of the entity to get.
 * \return The entity requested.
 */
EntityPtr Entities::get_entity(const std::string& name) {

  const EntityPtr& entity = find_entity(name);

  if (entity == nullptr) {
    Debug::die(std::string("Map '") + map.get_id()
        + "': Cannot find entity with name '" + name + "'");
  }

  return entity;
}

/**
 * \brief Returns the entity with the specified name,
 * or nullptr if it does not exist.
 * \param name Name of the entity to find.
 * \return The entity requested, or nullptr if there is no entity with the
 * given name.
 */
EntityPtr Entities::find_entity(const std::string& name) {

  auto it = named_entities.find(name);
  if (it == named_entities.end()) {
    return nullptr;
  }

  const EntityPtr& entity = it->second;

  if (entity->is_being_removed()) {
    return nullptr;
  }

  return entity;
}

/**
 * \brief Returns the entities of the map having the specified name prefix.
 *
 * The hero is included if the prefix matches.
 *
 * \param prefix Prefix of the name.
 * \return The entities having this prefix in their name, in arbitrary order.
 */
EntityVector Entities::get_entities_with_prefix(const std::string& prefix) {

  EntityVector entities;

  if (prefix.empty()) {
    // No prefix: return all entities no matter their name.
    for (const EntityPtr& entity: all_entities) {
      if (!entity->is_being_removed()) {
        entities.push_back(entity);
      }
    }
    entities.push_back(hero);
    return entities;
  }

  // Normal case: add entities whose name starts with the prefix.
  for (const auto& kvp: named_entities) {
    const EntityPtr& entity = kvp.second;
    if (entity->has_prefix(prefix) &&
        !entity->is_being_removed()) {
      entities.push_back(entity);
    }
  }

  return entities;
}

/**
 * \brief Like get_entities_with_prefix(const std::string&), but sorts entities according to
 * their Z index on the map.
 * \param prefix Prefix of the name.
 * \return The entities having this prefix in their name, in Z order.
 */
EntityVector Entities::get_entities_with_prefix_sorted(const std::string& prefix) {

  EntityVector entities = get_entities_with_prefix(prefix);
  std::sort(entities.begin(), entities.end(), ZOrderComparator(*this));

  return entities;
}

/**
 * \brief Returns the entities of the map with the specified type and having
 * the specified name prefix.
 * \param type Type of entity.
 * \param prefix Prefix of the name.
 * \return The entities of this type and having this prefix in their name, in arbitrary order.
 */
EntityVector Entities::get_entities_with_prefix(
    EntityType type, const std::string& prefix) {

  EntityVector entities;

  if (prefix.empty()) {
    // No prefix: return all entities of the type, no matter their name.
    for (const EntityPtr& entity: get_entities_by_type(type)) {
      if (!entity->is_being_removed()) {
        entities.push_back(entity);
      }
    }
    return entities;
  }

  // Normal case: add entities whose name starts with the prefix.
  for (const auto& kvp: named_entities) {
    const EntityPtr& entity = kvp.second;
    if (entity->get_type() == type &&
        entity->has_prefix(prefix) &&
        !entity->is_being_removed()
    ) {
      entities.push_back(entity);
    }
  }

  return entities;
}

/**
 * \brief Like get_entities_with_prefix(EntityType, const std::string&),
 * but sorts entities according to their Z index on the map.
 * \param type Type of entity.
 * \param prefix Prefix of the name.
 * \return The entities having this prefix in their name, in Z order.
 */
EntityVector Entities::get_entities_with_prefix_sorted(
    EntityType type, const std::string& prefix) {

  EntityVector entities = get_entities_with_prefix(type, prefix);
  std::sort(entities.begin(), entities.end(), ZOrderComparator(*this));

  return entities;
}

/**
 * \brief Returns whether there exists at least one entity with the specified
 * name prefix on the map.
 * \param prefix Prefix of the name.
 * \return \c true if there exists an entity with this prefix.
 */
bool Entities::has_entity_with_prefix(const std::string& prefix) const {

  for (const EntityPtr& entity: all_entities) {
    if (entity->has_prefix(prefix) && !entity->is_being_removed()) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Returns all entities whose bounding box overlaps the given rectangle.
 * \param[in] rectangle A rectangle.
 * \param[out] result The entities in that rectangle, in arbitrary order.
 */
void Entities::get_entities_in_rectangle(
    const Rectangle& rectangle, ConstEntityVector& result
) const {

  EntityVector non_const_result = quadtree.get_elements(rectangle);

  result.reserve(non_const_result.size());
  for (const ConstEntityPtr& entity : non_const_result) {
      result.push_back(entity);
  }
}

/**
 * \overload Non-const version.
 */
void Entities::get_entities_in_rectangle(
    const Rectangle& rectangle, EntityVector& result
) {

  result = quadtree.get_elements(rectangle);
}

/**
 * \brief Like get_entities_in_rectangle(), but sorts entities according to
 * their Z index on the map.
 * \param[in] rectangle A rectangle.
 * \param[out] result The entities in that rectangle, in arbitrary order.
 */
void Entities::get_entities_in_rectangle_sorted(
    const Rectangle& rectangle,
    ConstEntityVector& result
) const {

  get_entities_in_rectangle(rectangle, result);
  std::sort(result.begin(), result.end(), ZOrderComparator(*this));
}

/**
 * \overload Non-const version.
 */
void Entities::get_entities_in_rectangle_sorted(
    const Rectangle& rectangle,
    EntityVector& result
) {

  get_entities_in_rectangle(rectangle, result);
  std::sort(result.begin(), result.end(), ZOrderComparator(*this));
}

/**
 * \brief Returns all entities in the same separator region as the given point.
 *
 * Regions are assumed to be rectangular (convex: no "L" shape).
 *
 * \param[in] xy A point.
 * \param[out] result The entities in the same region as the point,
 * where regions are delimited by separators and map limits.
 */
void Entities::get_entities_in_region(
    const Point& xy, EntityVector& result
) {

  // Find the bounding box of the region.
  Rectangle region_box = get_region_box(xy);

  // Get entities in that rectangle.
  get_entities_in_rectangle(region_box, result);
}

/**
 * \brief Determines the bounding box of a same separator region.
 *
 * Regions are assumed to be rectangular (convex: no "L" shape).
 *
 * \param point A point.
 * \return The box of the region.
 */
Rectangle Entities::get_region_box(const Point& point) const {

  // Start with a rectangle of the whole map.
  int top = 0;
  int bottom = map.get_height();
  int left = 0;
  int right = map.get_width();

  // Find the closest separator in each direction.

  const std::set<ConstSeparatorPtr>& separators =
      get_entities_by_type<Separator>();
  for (const ConstSeparatorPtr& separator: separators) {

    const Point& separator_center = separator->get_center_point();

    if (separator->is_vertical()) {

      // Vertical separation.
      if (point.y < separator->get_top_left_y() ||
          point.y >= separator->get_top_left_y() + separator->get_height()) {
        // This separator is irrelevant: the point is not in either side,
        // it is too much to the north or to the south.
        //
        //     |
        //     |
        //     |
        //
        //  x
        //
        continue;
      }

      if (separator_center.x <= point.x) {
        // The separator is on the left.
        left = std::max(left, separator_center.x);
      }
      else {
        // The separator is on the right.
        right = std::min(right, separator_center.x);
      }
    }
    else {
      // Horizontal separation.
      if (point.x < separator->get_top_left_x() ||
          point.x >= separator->get_top_left_x() + separator->get_width()) {
        // This separator is irrelevant: the point is not in either side.
        continue;
      }

      if (separator_center.y <= point.y) {
        // The separator is on the top.
        top = std::max(top, separator_center.y);
      }
      else {
        // The separator is on the bottom.
        bottom = std::min(bottom, separator_center.y);
      }
    }
  }

  Debug::check_assertion(top < bottom && left < right, "Invalid region rectangle");

  return Rectangle(left, top, right - left, bottom - top);
}

/**
 * \brief Like get_entities_in_region(), but sorts entities according to
 * their Z index on the map.
 * \param[in] xy A point.
 * \param[out] result The entities in the same region as the point,
 * where regions are delimited by separators and map limits.
 */
void Entities::get_entities_in_region_sorted(
    const Point& xy, EntityVector& result
) {

  get_entities_in_region(xy, result);
  std::sort(result.begin(), result.end(), ZOrderComparator(*this));
}

/**
 * \brief Returns all entities of a type.
 * \param type An entity type.
 * \return All entities of the type.
 */
EntitySet Entities::get_entities_by_type(EntityType type) {

  EntitySet result;
  for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {
    const EntitySet& layer_entities = get_entities_by_type(type, layer);
    result.insert(layer_entities.begin(), layer_entities.end());
  }
  return result;
}

/**
 * \brief Like get_entities_by_type(EntityType),
 * but sorts entities according to their Z index on the map.
 * \param type An entity type.
 * \return All entities of the type.
 */
EntityVector Entities::get_entities_by_type_sorted(EntityType type) {

  EntitySet entity_set = get_entities_by_type(type);
  EntityVector entities;
  entities.insert(entities.begin(), entity_set.begin(), entity_set.end());
  std::sort(entities.begin(), entities.end(), ZOrderComparator(*this));
  return entities;
}

/**
 * \brief Returns all entities of a type on the given layer.
 * \param type Type of entities to get.
 * \param layer The layer to get entities from.
 * \return All entities of the type on this layer.
 */
EntitySet Entities::get_entities_by_type(EntityType type, int layer) {

  Debug::check_assertion(map.is_valid_layer(layer), "Invalid layer");

  EntitySet result;

  const auto& it = entities_by_type.find(type);
  if (it == entities_by_type.end()) {
    return result;
  }

  const ByLayer<EntitySet>& sets = it->second;
  const auto& layer_it = sets.find(layer);
  if (layer_it == sets.end()) {
    return result;
  }
  for (const EntityPtr& entity : layer_it->second) {
    result.insert(entity);
  }
  return result;
}

/**
 * \brief Returns a hint on the Z order of this entity.
 *
 * It can be used to know if an entity is above or below another one on the
 * same layer. It is unique for each entity on the same layer.
 *
 * This is an arbitrary value: it is not necessarily between 0 and the number
 * of entities on the layer. Its value only makes sense when compared to the
 * value of other entities.
 * Due to entities being removed from the map, entities whose layer changes,
 * and bring_to_front()/bring_to_back() calls,
 * there may be holes in the sequence.
 * The returned value can even be negative.
 *
 * \param entity An entity of the map.
 * \return Its relative Z order.
 */
int Entities::get_entity_relative_z_order(const ConstEntityPtr& entity) const {

  const int layer = entity->get_layer();
  return z_caches.at(layer).get_z(entity);
}

/**
 * \brief Brings to front an entity in its layer.
 * \param entity The entity to bring to front.
 */
void Entities::bring_to_front(Entity& entity) {

  const EntityPtr& shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());
  int layer = entity.get_layer();
  z_caches.at(layer).bring_to_front(shared_entity);
}

/**
 * \brief Brings to back an entity in its layer.
 * \param entity The entity to bring to back.
 */
void Entities::bring_to_back(Entity& entity) {

  const EntityPtr& shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());
  int layer = entity.get_layer();
  z_caches.at(layer).bring_to_back(shared_entity);
}

/**
 * \brief Notifies all entities of the map that the map has just become active.
 */
void Entities::notify_map_started() {

  // Setup non-animated tiles pre-drawing.
  for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {
    std::vector<TileInfo> tiles_in_animated_regions_info;
    non_animated_regions.at(layer)->build(tiles_in_animated_regions_info);
    for (const TileInfo& tile_info : tiles_in_animated_regions_info) {
      // This tile is non-optimizable, create it for real.
      TilePtr tile = std::make_shared<Tile>(tile_info);
      tiles_in_animated_regions.at(layer).push_back(tile);
      add_entity(tile);
    }
  }

  // Now, tiles_in_animated_regions contains the tiles that won't be optimized.
  // Notify entities.
  for (const EntityPtr& entity: all_entities) {
    entity->notify_map_started();
    entity->notify_tileset_changed();
  }
  hero->notify_map_started();
  hero->notify_tileset_changed();
}

/**
 * \brief Notifies all entities that the opening transition
 * of the map is finished.
 */
void Entities::notify_map_opening_transition_finished() {

  for (const EntityPtr& entity: all_entities) {
    entity->notify_map_opening_transition_finished();
  }
  hero->notify_map_opening_transition_finished();
}

/**
 * \brief Notifies this entity manager that the tileset of the map has
 * changed.
 */
void Entities::notify_tileset_changed() {

  // Redraw optimized tiles (i.e. non animated ones).
  for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {
    non_animated_regions[layer]->notify_tileset_changed();
  }

  for (const EntityPtr& entity: all_entities) {
    entity->notify_tileset_changed();
  }
  hero->notify_tileset_changed();
}

/**
 * \brief Notifies all entities that the map is being stopped.
 */
void Entities::notify_map_finished() {

  for (const EntityPtr& entity: all_entities) {
    notify_entity_removed(*entity);
  }
}

/**
 * \brief Creates the internal layer structures.
 *
 * The min and max layers in the map must be known at this point.
 */
void Entities::initialize_layers() {

  Debug::check_assertion(z_caches.empty(), "Layers already initialized");

  for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {
    tiles_ground[layer] = std::vector<Ground>();
    non_animated_regions[layer] = std::unique_ptr<NonAnimatedRegions>();
    tiles_in_animated_regions[layer] = std::vector<TilePtr>();
    z_caches[layer] = ZCache();
  }
}

/**
 * \brief Adds tile creation info to the map.
 *
 * If possible, the actual tile will never be created for performance reasons:
 * instead, only its picture and its obstacle info are stored.
 *
 * This function is called for each tile when loading the map.
 *
 * \param tile_info The tile info to add.
 */
void Entities::add_tile_info(const TileInfo& tile_info) {

  const Rectangle& box = tile_info.box;
  const int layer = tile_info.layer;
  Debug::check_assertion(map.is_valid_layer(layer),
                         "Invalid layer");

  Debug::check_assertion(tile_info.pattern != nullptr,
                         "Missing tile pattern");
  const TilePattern& pattern = *tile_info.pattern;

  // The size of a runtime tile should be the size of its pattern
  // for performance reasons, to optimize away more tiles.
  Debug::check_assertion(
      box.get_width() == pattern.get_width() &&
      box.get_height() == pattern.get_height(),
      "Static tile size must match tile pattern size");

  // Update the animated regions manager.
  non_animated_regions[tile_info.layer]->add_tile(tile_info);

  // Update the ground list.
  const Ground ground = pattern.get_ground();

  const int tile_x8 = box.get_x() / 8;
  const int tile_y8 = box.get_y() / 8;
  const int tile_width8 = box.get_width() / 8;
  const int tile_height8 = box.get_height() / 8;

  int i, j;
  Ground non_obstacle_triangle;

  switch (ground) {

    // If the obstacle property is the same for all points inside the tile
    // pattern, then all 8x8 squares of the extended tile have the same
    // property.
  case Ground::TRAVERSABLE:
  case Ground::LOW_WALL:
  case Ground::SHALLOW_WATER:
  case Ground::DEEP_WATER:
  case Ground::GRASS:
  case Ground::HOLE:
  case Ground::ICE:
  case Ground::LAVA:
  case Ground::PRICKLE:
  case Ground::LADDER:
  case Ground::WALL:
    for (i = 0; i < tile_height8; i++) {
      for (j = 0; j < tile_width8; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, ground);
      }
    }
    break;

    // If the top right corner of the tile is an obstacle,
    // then the top right 8x8 squares are Ground::WALL, the bottom left 8x8
    // squares are Ground::TRAVERSABLE or Ground::DEEP_WATER and the 8x8 squares
    // on the diagonal are Ground::WALL_TOP_RIGHT.
  case Ground::WALL_TOP_RIGHT:
  case Ground::WALL_TOP_RIGHT_WATER:

    non_obstacle_triangle = (ground == Ground::WALL_TOP_RIGHT) ?
        Ground::TRAVERSABLE : Ground::DEEP_WATER;

    // We traverse each row of 8x8 squares on the tile.
    for (i = 0; i < tile_height8; i++) {

      // 8x8 square on the diagonal.
      set_tile_ground(layer, tile_x8 + i, tile_y8 + i, Ground::WALL_TOP_RIGHT);

      // Left part of the row: we are in the bottom-left corner.
      for (j = 0; j < i; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, non_obstacle_triangle);
      }

      // Right part of the row: we are in the top-right corner.
      for (j = i + 1; j < tile_width8; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, Ground::WALL);
      }
    }
    break;

  case Ground::WALL_TOP_LEFT:
  case Ground::WALL_TOP_LEFT_WATER:

    non_obstacle_triangle = (ground == Ground::WALL_TOP_LEFT) ?
        Ground::TRAVERSABLE : Ground::DEEP_WATER;

    // We traverse each row of 8x8 squares on the tile.
    for (i = 0; i < tile_height8; i++) {

      // Right part of the row: we are in the bottom-right corner.
      for (j = tile_width8 - i; j < tile_width8; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, non_obstacle_triangle);
      }

      // Left part of the row: we are in the top-left corner.
      for (j = 0; j < tile_width8 - i - 1; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, Ground::WALL);
      }

      // 8x8 square on the diagonal.
      set_tile_ground(layer, tile_x8 + j, tile_y8 + i, Ground::WALL_TOP_LEFT);
    }
    break;

  case Ground::WALL_BOTTOM_LEFT:
  case Ground::WALL_BOTTOM_LEFT_WATER:

    non_obstacle_triangle = (ground == Ground::WALL_BOTTOM_LEFT) ?
        Ground::TRAVERSABLE : Ground::DEEP_WATER;

    // We traverse each row of 8x8 squares on the tile.
    for (i = 0; i < tile_height8; i++) {

      // Right part of the row: we are in the top-right corner.
      for (j = i + 1; j < tile_width8; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, non_obstacle_triangle);
      }
      // Left part of the row: we are in the bottom-left corner.
      for (j = 0; j < i; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, Ground::WALL);
      }

      // 8x8 square on the diagonal.
      set_tile_ground(layer, tile_x8 + j, tile_y8 + i, Ground::WALL_BOTTOM_LEFT);
    }
    break;

  case Ground::WALL_BOTTOM_RIGHT:
  case Ground::WALL_BOTTOM_RIGHT_WATER:

    non_obstacle_triangle = (ground == Ground::WALL_BOTTOM_RIGHT) ?
        Ground::TRAVERSABLE : Ground::DEEP_WATER;

    // We traverse each row of 8x8 squares on the tile.
    for (i = 0; i < tile_height8; i++) {

      // 8x8 square on the diagonal
      set_tile_ground(layer, tile_x8 + tile_width8 - i - 1, tile_y8 + i, Ground::WALL_BOTTOM_RIGHT);

      // Left part of the row: we are in the top-left corner.
      for (j = 0; j < tile_width8 - i - 1; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, non_obstacle_triangle);
      }

      // Right part of the row: we are in the bottom-right corner.
      for (j = tile_width8 - i; j < tile_width8; j++) {
        set_tile_ground(layer, tile_x8 + j, tile_y8 + i, Ground::WALL);
      }
    }
    break;

  case Ground::EMPTY:
    // Keep the ground property from any tile placed before.
    break;
  }
}

/**
 * \brief Adds an entity to the map.
 *
 * This function can be called when loading the map or,
 * except for tiles, when the map is already loaded.
 * If the entity specified is nullptr
 * (because some entity creation functions may return nullptr),
 * nothing is done.
 *
 * \param entity The entity to add or nullptr.
 */
void Entities::add_entity(const EntityPtr& entity) {

  if (entity == nullptr) {
    return;
  }

  Debug::check_assertion(map.is_valid_layer(entity->get_layer()),
      "No such layer on this map");

  const EntityType type = entity->get_type();
  if (type != EntityType::TILE) {  // Tiles are optimized specifically.
    const int layer = entity->get_layer();

    // Update the quadtree.
    quadtree.add(entity, entity->get_max_bounding_box());

    // Update the specific entities lists.
    switch (entity->get_type()) {

      case EntityType::CAMERA:
        Debug::check_assertion(camera == nullptr, "Only one camera is supported");
        camera = std::static_pointer_cast<Camera>(entity);
        break;

      case EntityType::DESTINATION:
        {
          std::shared_ptr<Destination> destination =
              std::static_pointer_cast<Destination>(entity);
          if (this->default_destination == nullptr || destination->is_default()) {
            this->default_destination = destination;
          }
        }
        break;

      default:
      break;
    }

    // Update the list of entities drawn not at their position.
    if (!entity->is_drawn_at_its_position()) {
      entities_drawn_not_at_their_position[layer].push_back(entity);
    }

    // Track the insertion order.
    z_caches[layer].add(entity);

    // Update the list of entities by type.
    auto it = entities_by_type.find(type);
    if (it == entities_by_type.end()) {
      it = entities_by_type.emplace(type, ByLayer<EntitySet>()).first;
    }
    ByLayer<EntitySet>& sets = it->second;
    sets[layer].insert(entity);

    // Update the list of all entities.
    if (type != EntityType::HERO) {
      all_entities.push_back(entity);
    }
  }

  // Rename the entity if there is already an entity with the same name.
  std::string name = entity->get_name();
  if (!name.empty()) {

    if (named_entities.find(name) != named_entities.end()) {
      // This name is already used by another entity. Add a suffix.
      std::ostringstream oss;
      std::istringstream iss;
      int suffix_number = 1;
      std::string prefix = name;
      size_t index = name.rfind('_');
      if (index != std::string::npos) {
        // If there is already a numbered suffix, we will increment it.
        const std::string& suffix = name.substr(index + 1);
        iss.clear();
        iss.str(suffix);
        if (iss >> suffix_number) {
          prefix = name.substr(0, index);
        }
      }

      // Now we have the final prefix. Find the first available suffix.
      do {
        ++suffix_number;
        oss.str("");
        oss.clear();
        oss << prefix << '_' << suffix_number;
        name = oss.str();
      }
      while (named_entities.find(name) != named_entities.end());

      entity->set_name(name);
    }
    named_entities[name] = entity;
  }

  // Notify the entity.
  if (type != EntityType::HERO) {
    entity->set_map(map);
  }
}

/**
 * \brief Removes an entity from the map and schedules it to be destroyed.
 * \param entity the entity to remove
 */
void Entities::remove_entity(Entity& entity) {

  if (!entity.is_being_removed()) {
    // Destroy the entity next frame.
    const EntityPtr& shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());
    entities_to_remove.push_back(shared_entity);

    // Tell the entity.
    entity.notify_being_removed();

    // Remove the entity from the by name list
    // to allow users to create a new one with
    // the same name right now.
    if (!entity.get_name().empty()) {
      named_entities.erase(entity.get_name());
    }
  }
}

/**
 * \brief Removes an entity from the map.
 * \param name Name of the entity.
 */
void Entities::remove_entity(const std::string& name) {

  const EntityPtr& entity = find_entity(name);
  if (entity != nullptr) {
    remove_entity(*entity);
  }
}

/**
 * \brief Removes all entities of a type whose name starts with the specified prefix.
 * \param prefix Prefix of the name of the entities to remove.
 */
void Entities::remove_entities_with_prefix(const std::string& prefix) {

  std::vector<EntityPtr> entities = get_entities_with_prefix(prefix);
  for (const EntityPtr& entity: entities) {
    remove_entity(*entity);
  }
}

/**
 * \brief Removes and destroys the entities placed in the entities_to_remove list.
 */
void Entities::remove_marked_entities() {

  // Remove the marked entities.
  for (const EntityPtr& entity: entities_to_remove) {

    const EntityType type = entity->get_type();
    const int layer = entity->get_layer();

    // Remove it from the quadtree.
    quadtree.remove(entity);

    // Remove it from the whole list.
    all_entities.remove(entity);
    const std::string& name = entity->get_name();
    if (!name.empty()) {
      named_entities.erase(name);
    }

    // Update the specific entities lists.
    switch (type) {

      case EntityType::CAMERA:
        camera = nullptr;
        break;

      default:
      break;
    }

    // Track the insertion order.
    z_caches.at(layer).remove(entity);

    // Update the list of entities by type.
    const auto& it = entities_by_type.find(type);
    if (it != entities_by_type.end()) {
      ByLayer<EntitySet>& sets = it->second;
      sets[layer].erase(entity);
    }

    // Destroy it.
    notify_entity_removed(*entity);
  }
  entities_to_remove.clear();
}

/**
 * \brief Suspends or resumes the movement and animations of the entities.
 *
 * This function is called by the map when the game
 * is being suspended or resumed.
 *
 * \param suspended true to suspend the movement and the animations,
 * false to resume them
 */
void Entities::set_suspended(bool suspended) {

  // the hero first
  hero->set_suspended(suspended);

  // other entities
  for (const EntityPtr& entity: all_entities) {
    entity->set_suspended(suspended);
  }

  // note that we don't suspend the tiles
}

/**
 * \brief Updates the position, movement and animation each entity.
 */
void Entities::update() {

  Debug::check_assertion(map.is_started(), "The map is not started");

  // First update the hero.
  hero->update();

  // Update the dynamic entities.
  for (const EntityPtr& entity: all_entities) {

    if (
        !entity->is_being_removed() &&
        entity->get_type() != EntityType::CAMERA  // The camera is updated after.
    ) {
      entity->update();
    }
  }

  // Update the camera after everyone else.
  camera->update();
  entities_to_draw.clear();  // Invalidate entities to draw.

  // Remove the entities that have to be removed now.
  remove_marked_entities();
}

/**
 * \brief Draws the entities on the map surface.
 */
void Entities::draw() {

  const CameraPtr& camera = get_camera();
  if (camera == nullptr) {
    return;
  }

  const SurfacePtr& camera_surface = camera->get_surface();

  // Lazily build the list of entities to draw.
  if (entities_to_draw.empty()) {

    // Add entities in the camera,
    // or nearby because of possible
    // on_pre_draw()/on_draw()/on_post_draw() reimplementations.
    // TODO it would probably be better to detect entities with
    // such events and make their is_drawn_at_its_position()
    // method return false.
    EntityVector entities_in_camera;
    Rectangle around_camera(
        Point(
            camera->get_x() - camera->get_size().width,
            camera->get_y() - camera->get_size().height
        ),
        camera->get_size() * 3
    );
    get_entities_in_rectangle(around_camera, entities_in_camera);

    for (const EntityPtr& entity : entities_in_camera) {
      int layer = entity->get_layer();
      Debug::check_assertion(map.is_valid_layer(layer), "Invalid layer");
      if (entity->is_enabled() &&
          entity->is_visible()) {
        entities_to_draw[layer].push_back(entity);
      }
    }

    // Add entities displayed even when out of the camera.
    for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {
      for (const EntityPtr& entity : entities_drawn_not_at_their_position[layer]) {
        if (entity->is_enabled() &&
            entity->is_visible()) {
          entities_to_draw[layer].push_back(entity);
        }
      }

      // Sort them and remove duplicates.
      // Duplicate drawings are a problem for entities with semi-transparency.
      // Using an std::set would be slower because duplicates are rare:
      // there are not often a lot of dynamic entities displayed out of the camera.
      std::sort(entities_to_draw[layer].begin(), entities_to_draw[layer].end(), DrawingOrderComparator());
      entities_to_draw[layer].erase(
            std::unique(entities_to_draw[layer].begin(), entities_to_draw[layer].end()),
            entities_to_draw[layer].end()
      );
    }

  }

  for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {

    // Draw the animated tiles and the tiles that overlap them:
    // in other words, draw all regions containing animated tiles
    // (and maybe more, but we don't care because non-animated tiles
    // will be drawn later).
    for (unsigned int i = 0; i < tiles_in_animated_regions[layer].size(); i++) {
      Tile& tile = *tiles_in_animated_regions[layer][i];
      if (tile.overlaps(*camera) || !tile.is_drawn_at_its_position()) {
        tile.draw_on_map();
      }
    }

    // Draw the non-animated tiles (with transparent rectangles on the regions of animated tiles
    // since they are already drawn).
    non_animated_regions[layer]->draw_on_map();

    // Draw dynamic entities, ordered by their data structure.
    for (const EntityPtr& entity: entities_to_draw[layer]) {
      entity->draw_on_map();
    }
  }

  if (EntityTree::debug_quadtrees) {
    // Draw the quadtree structure for debugging.
    quadtree.draw(camera_surface, -camera->get_top_left_xy());
  }
}

/**
 * \brief Changes the layer of an entity.
 * \param entity An entity.
 * \param layer The new layer.
 */
void Entities::set_entity_layer(Entity& entity, int layer) {

  int old_layer = entity.get_layer();

  if (layer != old_layer) {

    const EntityPtr& shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());

    // Track the insertion order.
    z_caches.at(old_layer).remove(shared_entity);
    z_caches.at(layer).add(shared_entity);

    // Update the list of entities by type and layer.
    const EntityType type = entity.get_type();
    const auto& it = entities_by_type.find(type);
    if (it != entities_by_type.end()) {
      ByLayer<EntitySet>& sets = it->second;
      sets[old_layer].erase(shared_entity);
      sets[layer].insert(shared_entity);
    }

    // Update the entity after the lists because this function might be called again.
    entity.set_layer(layer);
  }
}

/**
 * \brief This function should be called whenever the size, coordinates or
 * sprite bounding box of an entity changes.
 * \param entity The entity modified.
 */
void Entities::notify_entity_bounding_box_changed(Entity& entity) {

  // Update the quadtree.

  // Note that if the entity is not in the quadtree
  // (i.e. not managed by MapEntities) this does nothing.
  EntityPtr shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());
  quadtree.move(shared_entity, shared_entity->get_max_bounding_box());
}

/**
 * \brief Returns whether a rectangle overlaps with a raised crystal block.
 * \param layer The layer to check.
 * \param rectangle A rectangle.
 * \return \c true if this rectangle overlaps a raised crystal block.
 */
bool Entities::overlaps_raised_blocks(int layer, const Rectangle& rectangle) {

  EntityVector entities_nearby;
  get_entities_in_rectangle(rectangle, entities_nearby);
  for (const EntityPtr& entity : entities_nearby) {

    if (entity->get_type() != EntityType::CRYSTAL_BLOCK) {
      continue;
    }

    if (entity->get_layer() != layer) {
      continue;
    }

    const CrystalBlock& crystal_block = static_cast<CrystalBlock&>(*entity);
    if (crystal_block.is_raised()) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Creates a Z order tracking data structure.
 */
Entities::ZCache::ZCache() :
    z_values(),
    min(0),
    max(-1) {

}

/**
 * \brief Returns the relative Z order of an entity.
 * \param entity An entity of the map. It must be in the structure.
 * \return Its relative Z order.
 */
int Entities::ZCache::get_z(const ConstEntityPtr& entity) const {

  SOLARUS_ASSERT(z_values.find(entity) != z_values.end(),
      std::string("No such entity in Z cache: " +
          entity->get_lua_type_name() +
          " '" +
          entity->get_name() +
          "'"
      )
  );

  return z_values.at(entity);
}

/**
 * \brief Inserts an entity at the end of the structure.
 *
 * Nothing happens if the entity was already present.
 */
void Entities::ZCache::add(const ConstEntityPtr& entity) {

  ++max;
  z_values.insert(std::make_pair(entity, max));
}

/**
 * \brief Removes an entity from the structure.
 *
 * Nothing happens if the entity was not present.
 */
void Entities::ZCache::remove(const ConstEntityPtr& entity) {

  z_values.erase(entity);
  // Z values remain unchanged: removing an entity does not break the order.
}

/**
 * \brief Puts an entity above all others.
 *
 * It will then have a Z order greater than all other entities in the structure.
 */
void Entities::ZCache::bring_to_front(const ConstEntityPtr& entity) {

  remove(entity);
  add(entity);
}

/**
 * \brief Puts an entity behind of all others.
 *
 * It will then have a Z order lower than all other entities in the structure.
 */
void Entities::ZCache::bring_to_back(const ConstEntityPtr& entity) {

  remove(entity);
  --min;
  z_values.insert(std::make_pair(entity, min));
}

}

