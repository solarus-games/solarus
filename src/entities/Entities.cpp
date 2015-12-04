/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Hero.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include "solarus/entities/Separator.h"
#include "solarus/entities/Stairs.h"
#include "solarus/entities/Tile.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include <sstream>

namespace Solarus {

namespace {

/**
 * \brief Comparator that sorts entities according to their Y coordinate.
 */
class YOrderComparator {

  public:

    /**
     * \brief Compares two entities.
     * \param first An entity.
     * \param second Another entity.
     * \return \c true if the first entity's Y coordinate is lower than the second one's.
     */
    bool operator()(const EntityPtr& first, const EntityPtr& second) {
      return first->get_y() < second->get_y();
    }

};

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
     * \brief Compares two entities.
     * \param first An entity.
     * \param second Another entity.
     * \return \c true if the first entity's Z index is lower than the second one's.
     */
    bool operator()(const EntityPtr& first, const EntityPtr& second) {

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

}  // Anonymous namespace.

/**
 * \brief Constructor.
 * \param game The game.
 * \param map The map (not loaded yet).
 */
Entities::Entities(Game& game, Map& map):
  game(game),
  map(map),
  map_width8(0),
  map_height8(0),
  num_layers(0),
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
  entities_drawn_first(),
  entities_drawn_y_order(),
  default_destination(nullptr) {

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
    const Rectangle& rectangle, EntityVector& result
) const {

  quadtree.get_elements(rectangle, result);
}

/**
 * \brief Like get_entities_in_rectangle(), but sorts entities according to
 * their Z index on the map.
 * \param[in] rectangle A rectangle.
 * \param[out] result The entities in that rectangle, in arbitrary order.
 */
void Entities::get_entities_in_rectangle_sorted(
    const Rectangle& rectangle,
    EntityVector& result
) const {

  get_entities_in_rectangle(rectangle, result);
  std::sort(result.begin(), result.end(), ZOrderComparator(*this));
}

/**
 * \brief Returns all entities of a type.
 * \param type An entity type.
 * \return All entities of the type.
 */
EntitySet Entities::get_entities_by_type(EntityType type) {

  EntitySet result;
  for (int layer = 0; layer < num_layers; ++layer) {
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
 * \param layer The layer to get entities from.
 * \return All entities of the type on this layer.
 */
EntitySet Entities::get_entities_by_type(EntityType type, int layer) {

  EntitySet result;

  const auto& it = entities_by_type.find(type);
  if (it == entities_by_type.end()) {
    return result;
  }

  const std::vector<EntitySet>& sets = it->second;
  for (const EntityPtr& entity : sets[layer]) {
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
 * \param An entity of the map.
 * \return Its relative Z order.
 */
int Entities::get_entity_relative_z_order(const EntityPtr& entity) const {

  const int layer = entity->get_layer();
  return z_caches[layer].get_z(entity);
}

/**
 * \brief Brings to front an entity in its layer.
 * \param entity The entity to bring to front.
 */
void Entities::bring_to_front(Entity& entity) {

  const EntityPtr& shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());
  int layer = entity.get_layer();
  if (entity.can_be_drawn() && !entity.is_drawn_in_y_order()) {
    entities_drawn_first[layer].remove(shared_entity);
    entities_drawn_first[layer].push_back(shared_entity);  // Displayed last.
  }

  z_caches[layer].bring_to_front(shared_entity);
}

/**
 * \brief Brings to back an entity in its layer.
 * \param entity The entity to bring to back.
 */
void Entities::bring_to_back(Entity& entity) {

  const EntityPtr& shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());
  int layer = entity.get_layer();
  if (entity.can_be_drawn() && !entity.is_drawn_in_y_order()) {
    entities_drawn_first[layer].remove(shared_entity);
    entities_drawn_first[layer].push_front(shared_entity);  // Displayed first.
  }

  z_caches[layer].bring_to_back(shared_entity);
}

/**
 * \brief Notifies all entities of the map that the map has just become active.
 */
void Entities::notify_map_started() {

  // Notify entities.
  for (const EntityPtr& entity: all_entities) {
    entity->notify_map_started();
    entity->notify_tileset_changed();
  }
  hero->notify_map_started();
  hero->notify_tileset_changed();

  // Setup non-animated tiles pre-drawing.
  for (int layer = 0; layer < map.get_num_layers(); layer++) {
    non_animated_regions[layer]->build(tiles_in_animated_regions[layer]);
    // Now, tiles_in_animated_regions contains the tiles that won't be optimized.
  }
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
  for (int layer = 0; layer < map.get_num_layers(); layer++) {
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
 * The number of layers in the map must be known at this point.
 */
void Entities::initialize_layers() {

  Debug::check_assertion(num_layers == 0, "Layers already initialized");

  num_layers = map.get_num_layers();

  Debug::check_assertion(num_layers > 0, "Unknown number of layers");

  tiles_ground.resize(num_layers);
  non_animated_regions.resize(num_layers);
  tiles_in_animated_regions.resize(num_layers);
  z_caches.resize(num_layers);
  entities_drawn_first.resize(num_layers);
  entities_drawn_y_order.resize(num_layers);
}

/**
 * \brief Adds a tile on the map.
 *
 * This function is called for each tile when loading the map.
 * The tiles cannot change during the game.
 *
 * \param tile The tile to add.
 */
void Entities::add_tile(const TilePtr& tile) {

  const int layer = tile->get_layer();

  // Add the tile to the map.
  non_animated_regions[layer]->add_tile(tile);

  const TilePattern& pattern = tile->get_tile_pattern();
  Debug::check_assertion(
      tile->get_width() == pattern.get_width()
      && tile->get_height() == pattern.get_height(),
      "Static tile size must match tile pattern size");

  // Update the ground list.
  const Ground ground = tile->get_tile_pattern().get_ground();

  const int tile_x8 = tile->get_x() / 8;
  const int tile_y8 = tile->get_y() / 8;
  const int tile_width8 = tile->get_width() / 8;
  const int tile_height8 = tile->get_height() / 8;

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
  if (type == EntityType::TILE) {
    // Tiles are optimized specifically for obstacle checks and rendering.
    add_tile(std::static_pointer_cast<Tile>(entity));
  }
  else {
    const int layer = entity->get_layer();

    // update the quadtree
    quadtree.add(entity, entity->get_max_bounding_box());

    // update the sprites list
    if (entity->is_drawn_in_y_order()) {
      entities_drawn_y_order[layer].push_back(entity);
    }
    else if (entity->can_be_drawn()) {
      entities_drawn_first[layer].push_back(entity);
    }

    // update the specific entities lists
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

    // Track the insertion order.
    z_caches[layer].add(entity);

    // Update the list of entities by type.
    auto it = entities_by_type.find(type);
    if (it == entities_by_type.end()) {
      it = entities_by_type.emplace(type, std::vector<EntitySet>(num_layers)).first;
    }
    std::vector<EntitySet>& sets = it->second;
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

    // Clear the entity's name to allow users to create a new one with
    // the same name right now.
    if (!entity.get_name().empty()) {
      named_entities.erase(entity.get_name());
      entity.set_name("");
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

  // remove the marked entities
  for (const EntityPtr& entity: entities_to_remove) {

    const EntityType type = entity->get_type();
    const int layer = entity->get_layer();

    // remove it from the quadtree
    quadtree.remove(entity);

    // remove it from the sprite entities list if present
    if (entity->is_drawn_in_y_order()) {
      entities_drawn_y_order[layer].remove(entity);
    }
    else if (entity->can_be_drawn()) {
      entities_drawn_first[layer].remove(entity);
    }

    // remove it from the whole list
    all_entities.remove(entity);
    const std::string& name = entity->get_name();
    if (!name.empty()) {
      named_entities.erase(name);
    }

    // update the specific entities lists
    switch (type) {

      case EntityType::CAMERA:
        camera = nullptr;
        break;

      default:
      break;
    }

    // Track the insertion order.
    z_caches[layer].remove(entity);

    // Update the list of entities by type.
    const auto& it = entities_by_type.find(type);
    if (it != entities_by_type.end()) {
      std::vector<EntitySet>& sets = it->second;
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
  for (int layer = 0; layer < map.get_num_layers(); layer++) {

    // Sort the entities drawn in y order.
    std::list<EntityPtr>& entities = entities_drawn_y_order[layer];
    entities.sort(YOrderComparator());
  }

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

  // Remove the entities that have to be removed now.
  remove_marked_entities();
}

/**
 * \brief Draws the entities on the map surface.
 */
void Entities::draw() {

  for (int layer = 0; layer < map.get_num_layers(); ++layer) {

    // draw the animated tiles and the tiles that overlap them:
    // in other words, draw all regions containing animated tiles
    // (and maybe more, but we don't care because non-animated tiles
    // will be drawn later)
    for (unsigned int i = 0; i < tiles_in_animated_regions[layer].size(); i++) {
      tiles_in_animated_regions[layer][i]->draw_on_map();
    }

    // draw the non-animated tiles (with transparent rectangles on the regions of animated tiles
    // since they are already drawn)
    non_animated_regions[layer]->draw_on_map();

    // draw the first sprites
    for (const EntityPtr& entity: entities_drawn_first[layer]) {

      if (entity->is_enabled()) {
        entity->draw_on_map();
      }
    }

    // draw the sprites at the hero's level, in the order
    // defined by their y position (including the hero)
    for (const EntityPtr& entity: entities_drawn_y_order[layer]) {

      if (entity->is_enabled()) {
        entity->draw_on_map();
      }
    }
  }

  if (EntityTree::debug_quadtrees) {
    quadtree.draw(map.get_visible_surface(), -map.get_camera_position().get_top_left());
  }
}

/**
 * \brief Sets whether an entity is drawn in Y order or in creation order.
 * \param entity The entity to change.
 * \param drawn_in_y_order \c true to display it in Y order, \c false to
 * display it in Z order.
 */
void Entities::set_entity_drawn_in_y_order(
    Entity& entity, bool drawn_in_y_order) {

  const EntityPtr& shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());
  const int layer = entity.get_layer();
  if (drawn_in_y_order) {
    entities_drawn_first[layer].remove(shared_entity);
    entities_drawn_y_order[layer].push_back(shared_entity);
  }
  else {
    entities_drawn_y_order[layer].remove(shared_entity);
    entities_drawn_first[layer].push_back(shared_entity);
  }
}

/**
 * \brief Changes the layer of an entity.
 *
 * Only some specific entities should change their layer.
 *
 * \param entity an entity
 * \param layer the new layer
 */
void Entities::set_entity_layer(Entity& entity, int layer) {

  int old_layer = entity.get_layer();

  if (layer != old_layer) {

    const EntityPtr& shared_entity = std::static_pointer_cast<Entity>(entity.shared_from_this());

    // Update the sprites list.
    if (entity.is_drawn_in_y_order()) {
      entities_drawn_y_order[old_layer].remove(shared_entity);
      entities_drawn_y_order[layer].push_back(shared_entity);
    }
    else if (entity.can_be_drawn()) {
      entities_drawn_first[old_layer].remove(shared_entity);
      entities_drawn_first[layer].push_back(shared_entity);
    }

    // Track the insertion order.
    z_caches[old_layer].remove(shared_entity);
    z_caches[layer].add(shared_entity);

    // Update the list of entities by type and layer.
    const EntityType type = entity.get_type();
    const auto& it = entities_by_type.find(type);
    if (it != entities_by_type.end()) {
      std::vector<EntitySet>& sets = it->second;
      sets[old_layer].erase(shared_entity);
      sets[layer].insert(shared_entity);
    }

    // Update the entity after the lists because this function might be called again
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
bool Entities::overlaps_raised_blocks(int layer, const Rectangle& rectangle) const {

  std::set<std::shared_ptr<const CrystalBlock>> blocks =
      get_entities_by_type<CrystalBlock>(layer);

  EntityVector entities_nearby;
  get_entities_in_rectangle(rectangle, entities_nearby);
  for (const EntityPtr& entity : entities_nearby) {

    if (entity->get_type() != EntityType::CRYSTAL_BLOCK) {
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
int Entities::ZCache::get_z(const EntityPtr& entity) const {

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
 * It must not be present.
 */
void Entities::ZCache::add(const EntityPtr& entity) {

  ++max;
  const auto& inserted = z_values.insert(std::make_pair(entity, max));
  Debug::check_assertion(inserted.second, "Entity already in Z cache");
}

/**
 * \brief Removes an entity from the structure.
 *
 * It must be present.
 */
void Entities::ZCache::remove(const EntityPtr& entity) {

  int num_removed = z_values.erase(entity);
  Debug::check_assertion(num_removed == 1, "Entity not found in Z cache");

  // Z values remain unchanged: removing an entity does not break the order.
}

/**
 * \brief Puts an entity above all others.
 *
 * It will then have a Z order greater than all other entities in the structure.
 */
void Entities::ZCache::bring_to_front(const EntityPtr& entity) {

  remove(entity);
  add(entity);
}

/**
 * \brief Puts an entity behind of all others.
 *
 * It will then have a Z order lower than all other entities in the structure.
 */
void Entities::ZCache::bring_to_back(const EntityPtr& entity) {

  remove(entity);
  --min;
  const auto& inserted = z_values.insert(std::make_pair(entity, min));
  Debug::check_assertion(inserted.second, "Entity already in Z cache");
}

}

