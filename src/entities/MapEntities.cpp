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
#include "solarus/entities/MapEntities.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Tile.h"
#include "solarus/entities/Tileset.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Layer.h"
#include "solarus/entities/CrystalBlock.h"
#include "solarus/entities/Boomerang.h"
#include "solarus/entities/Stairs.h"
#include "solarus/entities/Separator.h"
#include "solarus/entities/Destination.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include "solarus/Map.h"
#include "solarus/Game.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/Debug.h"
#include <sstream>

namespace Solarus {

/**
 * \brief Constructor.
 * \param game The game.
 * \param map The map (not loaded yet).
 */
MapEntities::MapEntities(Game& game, Map& map):
  game(game),
  map(map),
  map_width8(0),
  map_height8(0),
  tiles_grid_size(0),
  hero(*game.get_hero()),
  default_destination(nullptr),
  boomerang(nullptr) {

  Layer hero_layer = hero.get_layer();
  this->obstacle_entities[hero_layer].push_back(&hero);
  this->entities_drawn_y_order[hero_layer].push_back(&hero);
  this->ground_observers[hero_layer].push_back(&hero);
  this->named_entities[hero.get_name()] = &hero;
}

/**
 * \brief Notifies an entity that it is being removed.
 * \param entity The entity being removed.
 */
void MapEntities::notify_entity_removed(MapEntity* entity) {

  if (!entity->is_being_removed()) {
    entity->notify_being_removed();
  }
}

/**
 * \brief Returns the hero.
 * \return the hero
 */
Hero& MapEntities::get_hero() {
  return hero;
}

/**
 * \brief Returns all entities expect tiles and the hero.
 * \return The entities except tiles and the hero.
 */
const std::list<MapEntityPtr>& MapEntities::get_entities() {
  return all_entities;
}

/**
 * \brief Returns the entities (other that tiles) such that the hero cannot walk on them.
 * \param layer The layer.
 * \return The obstacle entities on that layer.
 */
const std::list<MapEntity*>& MapEntities::get_obstacle_entities(Layer layer) {
  return obstacle_entities[layer];
}

/**
 * \brief Returns the entities that are sensible to the ground below them.
 * \param layer The layer.
 * \return The ground observers on that layer.
 */
const std::list<MapEntity*>& MapEntities::get_ground_observers(Layer layer) {
  return ground_observers[layer];
}

/**
 * \brief Returns the entities that are sensible to the ground below them.
 * \param layer The layer.
 * \return The ground observers on that layer.
 */
const std::list<MapEntity*>& MapEntities::get_ground_modifiers(Layer layer) {
  return ground_modifiers[layer];
}

/**
 * \brief Returns all detectors on the map.
 * \return the detectors
 */
const std::list<Detector*>& MapEntities::get_detectors() {
  return detectors;
}

/**
 * \brief Returns the default destination of the map.
 * \return The default destination, or nullptr if there exists no destination
 * on this map.
 */
Destination* MapEntities::get_default_destination() {
  return default_destination;
}

/**
 * \brief Returns all stairs on the specified layer.
 * \param layer the layer
 * \return the stairs on this layer
 */
const std::list<Stairs*>& MapEntities::get_stairs(Layer layer) {
  return stairs[layer];
}

/**
 * \brief Returns all crystal blocks on the specified layer.
 * \param layer the layer
 * \return the crystal blocks on this layer
 */
const std::list<CrystalBlock*>& MapEntities::get_crystal_blocks(Layer layer) {
  return crystal_blocks[layer];
}

/**
 * \brief Returns all separators of the map.
 * \return The separators.
 */
const std::list<const Separator*>& MapEntities::get_separators() const {
  return separators;
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
void MapEntities::set_tile_ground(Layer layer, int x8, int y8, Ground ground) {

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
MapEntity* MapEntities::get_entity(const std::string& name) {

  MapEntity* entity = find_entity(name);

  if (entity == nullptr) {
    Debug::die(std::string("Map '") + map.get_id()
        + "': Cannot find entity with name '" + name + "'");
  }

  return entity;
}

/**
 * \brief Returns the entity with the specified name, or nullptr if it doesn't exist.
 * \param name Name of the entity to find.
 * \return The entity requested, or nullptr if there is no entity with the specified name.
 */
MapEntity* MapEntities::find_entity(const std::string& name) {

  auto it = named_entities.find(name);
  if (it == named_entities.end()) {
    return nullptr;
  }

  MapEntity* entity = it->second;

  if (entity->is_being_removed()) {
    return nullptr;
  }

  return entity;
}

/**
 * \brief Returns the entities of the map having the specified name prefix.
 * \param prefix Prefix of the name.
 * \return The entities of this type and having this prefix in their name.
 */
std::list<MapEntity*> MapEntities::get_entities_with_prefix(const std::string& prefix) {

  std::list<MapEntity*> entities;

  for (const MapEntityPtr& entity: all_entities) {
    if (entity->has_prefix(prefix) && !entity->is_being_removed()) {
      entities.push_back(entity.get());
    }
  }

  return entities;
}

/**
 * \brief Returns the entities of the map with the specified type and having
 * the specified name prefix.
 * \param type Type of entity.
 * \param prefix Prefix of the name.
 * \return The entities of this type and having this prefix in their name.
 */
std::list<MapEntity*> MapEntities::get_entities_with_prefix(
    EntityType type, const std::string& prefix) {

  std::list<MapEntity*> entities;

  for (const MapEntityPtr& entity: all_entities) {
    if (entity->get_type() == type && entity->has_prefix(prefix) && !entity->is_being_removed()) {
      entities.push_back(entity.get());
    }
  }

  return entities;
}

/**
 * \brief Returns whether there exists at least one entity with the specified
 * name prefix on the map.
 * \param prefix Prefix of the name.
 * \return \c true if there exists an entity with this prefix.
 */
bool MapEntities::has_entity_with_prefix(const std::string& prefix) const {

  for (const MapEntityPtr& entity: all_entities) {
    if (entity->has_prefix(prefix) && !entity->is_being_removed()) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Brings to front an entity in its layer.
 * \param entity The entity to bring to front.
 */
void MapEntities::bring_to_front(MapEntity& entity) {

  Layer layer = entity.get_layer();
  if (entity.can_be_drawn() && !entity.is_drawn_in_y_order()) {
    entities_drawn_first[layer].remove(&entity);
    entities_drawn_first[layer].push_back(&entity);  // Displayed last.
  }

  if (entity.can_be_obstacle()) {
    if (entity.has_layer_independent_collisions()) {
      obstacle_entities[LAYER_LOW].remove(&entity);
      obstacle_entities[LAYER_LOW].push_back(&entity);
      obstacle_entities[LAYER_INTERMEDIATE].remove(&entity);
      obstacle_entities[LAYER_INTERMEDIATE].push_back(&entity);
      obstacle_entities[LAYER_HIGH].remove(&entity);
      obstacle_entities[LAYER_HIGH].push_back(&entity);
    }
    else {
      obstacle_entities[layer].remove(&entity);
      obstacle_entities[layer].push_back(&entity);
    }
  }

  // TODO also update ground_modifiers and ground_observers.
}

/**
 * \brief Brings to back an entity in its layer.
 * \param entity The entity to bring to back.
 */
void MapEntities::bring_to_back(MapEntity& entity) {

  Layer layer = entity.get_layer();
  if (entity.can_be_drawn() && !entity.is_drawn_in_y_order()) {
    entities_drawn_first[layer].remove(&entity);
    entities_drawn_first[layer].push_front(&entity);  // Displayed first.
  }

  if (entity.can_be_obstacle()) {
    if (entity.has_layer_independent_collisions()) {
      obstacle_entities[LAYER_LOW].remove(&entity);
      obstacle_entities[LAYER_LOW].push_front(&entity);
      obstacle_entities[LAYER_INTERMEDIATE].remove(&entity);
      obstacle_entities[LAYER_INTERMEDIATE].push_front(&entity);
      obstacle_entities[LAYER_HIGH].remove(&entity);
      obstacle_entities[LAYER_HIGH].push_front(&entity);
    }
    else {
      obstacle_entities[layer].remove(&entity);
      obstacle_entities[layer].push_front(&entity);
    }
  }

  // TODO also update ground_modifiers and ground_observers.
}

/**
 * \brief Notifies all entities of the map that the map has just become active.
 */
void MapEntities::notify_map_started() {

  // Put the hero in the the quadtree.
  quadtree.add(std::static_pointer_cast<Hero>(hero.shared_from_this()));

  // Notify entities.
  for (const MapEntityPtr& entity: all_entities) {
    entity->notify_map_started();
    entity->notify_tileset_changed();
  }
  hero.notify_map_started();
  hero.notify_tileset_changed();

  // Setup non-animated tiles pre-drawing.
  for (int layer = 0; layer < LAYER_NB; layer++) {
    non_animated_regions[layer]->build(tiles_in_animated_regions[layer]);
    // Now, tiles_in_animated_regions contains the tiles that won't be optimized.
  }
}

/**
 * \brief Notifies all entities that the opening transition
 * of the map is finished.
 */
void MapEntities::notify_map_opening_transition_finished() {

  for (const MapEntityPtr& entity: all_entities) {
    entity->notify_map_opening_transition_finished();
  }
  hero.notify_map_opening_transition_finished();
}

/**
 * \brief Notifies this entity manager that the tileset of the map has
 * changed.
 */
void MapEntities::notify_tileset_changed() {

  // Redraw optimized tiles (i.e. non animated ones).
  for (int layer = 0; layer < LAYER_NB; layer++) {
    non_animated_regions[layer]->notify_tileset_changed();
  }

  for (const MapEntityPtr& entity: all_entities) {
    entity->notify_tileset_changed();
  }
  hero.notify_tileset_changed();
}

/**
 * \brief Notifies all entities that the map is being stopped.
 */
void MapEntities::notify_map_finished() {

  for (const MapEntityPtr& entity: all_entities) {
    notify_entity_removed(entity.get());
  }
}

/**
 * \brief Adds a tile on the map.
 *
 * This function is called for each tile when loading the map.
 * The tiles cannot change during the game.
 *
 * \param tile The tile to add.
 */
void MapEntities::add_tile(const TilePtr& tile) {

  const Layer layer = tile->get_layer();

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
 * This function is called when loading the map. If the entity
 * specified is nullptr (because some entity creation functions
 * may return nullptr), nothing is done.
 *
 * \param entity The entity to add (can be an empty pointer).
 */
void MapEntities::add_entity(const MapEntityPtr& entity) {

  if (entity == nullptr) {
    return;
  }

  if (entity->get_type() == EntityType::TILE) {
    // Tiles are optimized specifically for obstacle checks and rendering.
    add_tile(std::static_pointer_cast<Tile>(entity));
  }
  else {
    Layer layer = entity->get_layer();

    // update the quadtree
    quadtree.add(entity);

    // update the detectors list
    if (entity->is_detector()) {
      detectors.push_back(static_cast<Detector*>(entity.get()));
    }

    // update the obstacle list
    if (entity->can_be_obstacle()) {

      if (entity->has_layer_independent_collisions()) {
        // some entities handle collisions on any layer (e.g. stairs inside a single floor)
        obstacle_entities[LAYER_LOW].push_back(entity.get());
        obstacle_entities[LAYER_INTERMEDIATE].push_back(entity.get());
        obstacle_entities[LAYER_HIGH].push_back(entity.get());
      }
      else {
        // but usually, an entity collides with only one layer
        obstacle_entities[layer].push_back(entity.get());
      }
    }

    // update the ground observers list
    if (entity->is_ground_observer()) {
      ground_observers[layer].push_back(entity.get());
    }

    // update the ground modifiers list
    if (entity->is_ground_modifier()) {
      ground_modifiers[layer].push_back(entity.get());
    }

    // update the sprites list
    if (entity->is_drawn_in_y_order()) {
      entities_drawn_y_order[layer].push_back(entity.get());
    }
    else if (entity->can_be_drawn()) {
      entities_drawn_first[layer].push_back(entity.get());
    }

    // update the specific entities lists
    switch (entity->get_type()) {

      case EntityType::STAIRS:
        stairs[layer].push_back(static_cast<Stairs*>(entity.get()));
        break;

      case EntityType::CRYSTAL_BLOCK:
        crystal_blocks[layer].push_back(static_cast<CrystalBlock*>(entity.get()));
        break;

      case EntityType::SEPARATOR:
        separators.push_back(static_cast<Separator*>(entity.get()));
        break;

      case EntityType::BOOMERANG:
        this->boomerang = static_cast<Boomerang*>(entity.get());
        break;

      case EntityType::DESTINATION:
        {
          Destination* destination = static_cast<Destination*>(entity.get());
          if (this->default_destination == nullptr || destination->is_default()) {
            this->default_destination = destination;
          }
        }
        break;

      default:
      break;
    }

    // Update the list of all entities.
    all_entities.push_back(entity);
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
    named_entities[name] = entity.get();
  }

  // Notify the entity.
  entity->set_map(map);
}

/**
 * \brief Removes an entity from the map and schedules it to be destroyed.
 * \param entity the entity to remove
 */
void MapEntities::remove_entity(MapEntity* entity) {

  if (!entity->is_being_removed()) {
    entities_to_remove.push_back(entity);
    entity->notify_being_removed();

    if (entity == this->boomerang) {
      this->boomerang = nullptr;
    }
  }
}

/**
 * \brief Removes an entity from the map.
 * \param name Name of the entity.
 */
void MapEntities::remove_entity(const std::string& name) {

  MapEntity* entity = find_entity(name);
  if (entity != nullptr) {
    remove_entity(entity);
  }
}

/**
 * \brief Removes all entities of a type whose name starts with the specified prefix.
 * \param prefix Prefix of the name of the entities to remove.
 */
void MapEntities::remove_entities_with_prefix(const std::string& prefix) {

  std::list<MapEntity*> entities = get_entities_with_prefix(prefix);
  for (MapEntity* entity: entities) {
    remove_entity(entity);
  }
}

/**
 * \brief Removes and destroys the entities placed in the entities_to_remove list.
 */
void MapEntities::remove_marked_entities() {

  // remove the marked entities
  for (MapEntity* entity: entities_to_remove) {

    MapEntityPtr shared_entity = std::static_pointer_cast<MapEntity>(entity->shared_from_this());
    Layer layer = entity->get_layer();

    // remove it from the quadtree
    quadtree.remove(shared_entity);

    // remove it from the obstacle entities list if present
    if (entity->can_be_obstacle()) {

      if (entity->has_layer_independent_collisions()) {
        for (int i = 0; i < LAYER_NB; i++) {
          obstacle_entities[i].remove(entity);
        }
      }
      else {
        obstacle_entities[layer].remove(entity);
      }
    }

    // remove it from the detectors list if present
    if (entity->is_detector()) {
      detectors.remove(static_cast<Detector*>(entity));
    }

    // remove it from the ground observers list if present
    if (entity->is_ground_observer()) {
      ground_observers[layer].remove(entity);
    }

    // remove it from the ground modifiers list if present
    if (entity->is_ground_modifier()) {
      ground_modifiers[layer].remove(entity);
    }

    // remove it from the sprite entities list if present
    if (entity->is_drawn_in_y_order()) {
      entities_drawn_y_order[layer].remove(entity);
    }
    else if (entity->can_be_drawn()) {
      entities_drawn_first[layer].remove(entity);
    }

    // remove it from the whole list
    all_entities.remove(shared_entity);
    const std::string& name = entity->get_name();
    if (!name.empty()) {
      named_entities.erase(name);
    }

    // update the specific entities lists
    switch (entity->get_type()) {

      case EntityType::STAIRS:
        stairs[layer].remove(static_cast<Stairs*>(entity));
        break;

      case EntityType::CRYSTAL_BLOCK:
        crystal_blocks[layer].remove(static_cast<CrystalBlock*>(entity));
        break;

      case EntityType::SEPARATOR:
        separators.remove(static_cast<Separator*>(entity));
        break;

      case EntityType::BOOMERANG:
        this->boomerang = nullptr;
        break;

      default:
      break;
    }

    // destroy it
    notify_entity_removed(shared_entity.get());
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
void MapEntities::set_suspended(bool suspended) {

  // the hero first
  hero.set_suspended(suspended);

  // other entities
  for (const MapEntityPtr& entity: all_entities) {
    entity->set_suspended(suspended);
  }

  // note that we don't suspend the tiles
}

/**
 * \brief Updates the position, movement and animation each entity.
 */
void MapEntities::update() {

  Debug::check_assertion(map.is_started(), "The map is not started");

  // First update the hero.
  hero.update();

  // Update the dynamic entities.
  for (int layer = 0; layer < LAYER_NB; layer++) {

    // Sort the entities drawn in y order.
    entities_drawn_y_order[layer].sort(compare_y);
  }

  for (const MapEntityPtr& entity: all_entities) {

    if (!entity->is_being_removed()) {
      entity->update();
    }
  }

  // remove the entities that have to be removed now
  remove_marked_entities();
}

/**
 * \brief Draws the entities on the map surface.
 */
void MapEntities::draw() {

  for (int layer = 0; layer < LAYER_NB; ++layer) {

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
    for (MapEntity* entity: entities_drawn_first[layer]) {

      if (entity->is_enabled()) {
        entity->draw_on_map();
      }
    }

    // draw the sprites at the hero's level, in the order
    // defined by their y position (including the hero)
    for (MapEntity* entity: entities_drawn_y_order[layer]) {

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
 * \brief Compares the y position of two entities.
 * \param first an entity
 * \param second another entity
 * \return true if the y position of the first entity is lower
 * than the second one
 */
bool MapEntities::compare_y(MapEntity* first, MapEntity* second) {

  // before was: first->get_top_left_y() < second->get_top_left_y(); but doesn't work for bosses
  return first->get_top_left_y() + first->get_height() < second->get_top_left_y() + second->get_height();
}

/**
 * \brief Sets whether an entity is drawn in Y order or in creation order.
 * \param entity The entity to change.
 * \param drawn_in_y_order \c true to display it in Y order, \c false to
 * display it in Z order.
 */
void MapEntities::set_entity_drawn_in_y_order(
    MapEntity& entity, bool drawn_in_y_order) {

  const Layer layer = entity.get_layer();
  if (drawn_in_y_order) {
    entities_drawn_first[layer].remove(&entity);
    entities_drawn_y_order[layer].push_back(&entity);
  }
  else {
    entities_drawn_y_order[layer].remove(&entity);
    entities_drawn_first[layer].push_back(&entity);
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
void MapEntities::set_entity_layer(MapEntity& entity, Layer layer) {

  Layer old_layer = entity.get_layer();

  if (layer != old_layer) {

    // update the obstacle list
    if (entity.can_be_obstacle() && !entity.has_layer_independent_collisions()) {
      obstacle_entities[old_layer].remove(&entity);
      obstacle_entities[layer].push_back(&entity);
    }

    // update the ground observers list
    if (entity.is_ground_observer()) {
      ground_observers[old_layer].remove(&entity);
      ground_observers[layer].push_back(&entity);
    }

    // update the ground modifiers list
    if (entity.is_ground_modifier()) {
      ground_modifiers[old_layer].remove(&entity);
      ground_modifiers[layer].push_back(&entity);
    }

    // update the sprites list
    if (entity.is_drawn_in_y_order()) {
      entities_drawn_y_order[old_layer].remove(&entity);
      entities_drawn_y_order[layer].push_back(&entity);
    }
    else if (entity.can_be_drawn()) {
      entities_drawn_first[old_layer].remove(&entity);
      entities_drawn_first[layer].push_back(&entity);
    }

    // update the entity after the lists because this function might be called again
    entity.set_layer(layer);
  }
}

/**
 * \brief This function should be called when an entity becomes a ground
 * observer or when it stops being one.
 * \param entity The entity whose ground observer property has changed.
 */
void MapEntities::notify_entity_ground_observer_changed(MapEntity& entity) {

  Layer layer = entity.get_layer();
  ground_observers[layer].remove(&entity);
  if (entity.is_ground_observer()) {
    ground_observers[layer].push_back(&entity);
  }
}

/**
 * \brief This function should be called when an entity becomes a ground
 * modifier or when it stops being one.
 * \param entity The entity whose ground modifier property has changed.
 */
void MapEntities::notify_entity_ground_modifier_changed(MapEntity& entity) {

  Layer layer = entity.get_layer();
  ground_modifiers[layer].remove(&entity);
  if (entity.is_ground_modifier()) {
    ground_modifiers[layer].push_back(&entity);
  }
}

/**
 * \brief Returns whether a rectangle overlaps with a raised crystal block.
 * \param layer the layer to check
 * \param rectangle a rectangle
 * \return true if this rectangle overlaps a raised crystal block
 */
bool MapEntities::overlaps_raised_blocks(Layer layer, const Rectangle& rectangle) {

  std::list<CrystalBlock*> blocks = get_crystal_blocks(layer);

  for (const CrystalBlock* block: blocks) {
    if (block->overlaps(rectangle) && block->is_raised()) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Returns true if the player has thrown the boomerang.
 * \return true if the boomerang is present on the map
 */
bool MapEntities::is_boomerang_present() {
  return boomerang != nullptr;
}

/**
 * \brief Removes the boomerang from the map, if it is present.
 */
void MapEntities::remove_boomerang() {

  if (boomerang != nullptr) {
    remove_entity(boomerang);
    boomerang = nullptr;
  }
}

/**
 * \brief Removes any arrow from the map.
 */
void MapEntities::remove_arrows() {

  // TODO this function may be slow if there are a lot of entities: store the arrows?
  for (const MapEntityPtr& entity: all_entities) {
    if (entity->get_type() == EntityType::ARROW) {
      remove_entity(entity.get());
    }
  }
}

}

