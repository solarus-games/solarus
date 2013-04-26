/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/DynamicTile.h"
#include "entities/Tileset.h"
#include "entities/TilePattern.h"
#include "entities/Hero.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "Game.h"
#include "Map.h"

/**
 * @brief Creates a new dynamic tile on the map.
 * @param name a name to identify this tile
 * @param layer layer of the tile
 * @param x x position of the tile on the map
 * @param y y position of the tile on the map
 * @param width width of the tile (the pattern can be repeated)
 * @param height height of the tile (the pattern can be repeated)
 * @param tile_pattern_id id of the tile pattern in the tileset 
 * @param enabled true to make the tile active on the map
 */
DynamicTile::DynamicTile(const std::string &name, Layer layer, int x, int y,
			 int width, int height, int tile_pattern_id, bool enabled):
  Detector(COLLISION_CUSTOM, name, layer, x, y, width, height),
  tile_pattern_id(tile_pattern_id), tile_pattern(NULL) {

  set_enabled(enabled);
}

/**
 * @brief Destructor.
 */
DynamicTile::~DynamicTile() {

}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType DynamicTile::get_type() {

  return DYNAMIC_TILE;
}

/**
 * @brief Sets the map of this entity.
 *
 * Warning: as this function is called when initializing the map,
 * the current map of the game is still the old one.
 *
 * @param map the map
 */
void DynamicTile::set_map(Map &map) {

  MapEntity::set_map(map);
  this->tile_pattern = &map.get_tileset().get_tile_pattern(tile_pattern_id);
}

/**
 * @brief Returns whether this tile is an obstacle for the specified entity.
 * @param other an entity
 * @return true if this tile is an obstacle for the entity
 */
bool DynamicTile::is_obstacle_for(MapEntity &other) {

  bool result = false;
  switch (tile_pattern->get_obstacle()) {

    case OBSTACLE:
    case OBSTACLE_TOP_RIGHT:
    case OBSTACLE_TOP_LEFT:
    case OBSTACLE_BOTTOM_LEFT:
    case OBSTACLE_BOTTOM_RIGHT:
    case OBSTACLE_TOP_RIGHT_WATER:
    case OBSTACLE_TOP_LEFT_WATER:
    case OBSTACLE_BOTTOM_LEFT_WATER:
    case OBSTACLE_BOTTOM_RIGHT_WATER:
      result = true;
      break;

    case OBSTACLE_EMPTY:
    case OBSTACLE_NONE:
    case OBSTACLE_SHALLOW_WATER:
    case OBSTACLE_DEEP_WATER:
    case OBSTACLE_HOLE:
    case OBSTACLE_LAVA:
    case OBSTACLE_PRICKLE:
    case OBSTACLE_LADDER:
      result = false;
      break;
  }
  return result;
}

/**
 * @brief Draws the tile on the map.
 */
void DynamicTile::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  const Rectangle& camera_position = get_map().get_camera_position();
  Rectangle dst(0, 0);

  Rectangle dst_position(get_top_left_x() - camera_position.get_x(),
      get_top_left_y() - camera_position.get_y(),
      get_width(), get_height());

  tile_pattern->fill_surface(get_map().get_visible_surface(), dst_position,
      get_map().get_tileset(), camera_position);
}

/**
 * @brief Returns whether an entity collides with this detector with respect to a custom rule.
 * @param entity the entity
 * @return true if the entity's collides with this detector with respect to the custom rule
 */
bool DynamicTile::test_collision_custom(MapEntity &entity) {

  // we must test the same coordinates as non-dynamic tiles (see Hero::get_ground_point())
  return overlaps(entity.get_x(), entity.get_y() - 2);
}

/**
 * @brief Notifies this detector that a collision was just detected with an entity.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision (useful if
 * the detector has several collision modes)
 */
void DynamicTile::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping.is_hero()) {
    // tell the hero that he is on the ground of this tile
    Hero& hero = (Hero&) entity_overlapping;
    Ground ground = Map::obstacle_to_ground(tile_pattern->get_obstacle());
    hero.set_ground(ground);
  }
}

/**
 * @brief Notifies this entity that it was just enabled or disabled.
 * @param enabled true if the entity is now enabled
 */
void DynamicTile::notify_enabled(bool enabled) {

  if (is_on_map()
      && !enabled
      && test_collision_custom(get_hero())) {
    // the tile under the hero is disabled: the hero's ground may have changed
    get_hero().check_position();
  }
}
