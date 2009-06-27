/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "entities/Tile.h"
#include "entities/TilePattern.h"
#include "entities/Layer.h"
#include "entities/Obstacle.h"
#include "entities/CrystalSwitchBlock.h"
#include "entities/Boomerang.h"
#include "Map.h"
#include "ZSDX.h"
#include "Game.h"
using std::list;

/**
 * Constructor.
 */
MapEntities::MapEntities(Map *map):
  map(map), hero_on_raised_blocks(false) {

  Hero *hero = zsdx->game->get_hero();
  Layer layer = hero->get_layer();
  this->obstacle_entities[layer].push_back(hero);
  this->entities_displayed_y_order[layer].push_back(hero);
  // TODO update that when the layer changes, same thing for enemies
}

/**
 * Destructor.
 */
MapEntities::~MapEntities(void) {
  destroy_all_entities();
}

/**
 * Removes all entities from the map.
 * This function is called by the destructor and when the map
 * is unloaded.
 */
void MapEntities::destroy_all_entities(void) {

  // delete the entities sorted by layer
  for (int layer = 0; layer < LAYER_NB; layer++) {

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      delete tiles[layer][i];
    }

    tiles[layer].clear();
    delete[] obstacle_tiles[layer];

    entities_displayed_first[layer].clear();
    entities_displayed_y_order[layer].clear();
    obstacle_entities[layer].clear();
  }

  // delete the other entities

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {
    delete *i;
  }
  all_entities.clear();

  destination_points.clear();
  detectors.clear();
  entities_to_remove.clear();
}

/**
 * Returns the number of destination points of the map.
 * @return the number of destination points
 */
unsigned int MapEntities::get_nb_destination_points(void) {
  return destination_points.size();
}

/**
 * Returns a destination point.
 * @param index index of the destination point to get
 */
DestinationPoint * MapEntities::get_destination_point(int index) {
  return destination_points[index];
}

/**
 * Returns the obstacle property of the tile located
 * at a specified point.
 * @param layer of the tile to get
 * @param x x coordinate of the point
 * @param y y coordiate of the point
 * @return the obstacle property of this tile
 */
Obstacle MapEntities::get_obstacle_tile(Layer layer, int x, int y) {

  int width8 = map->get_width8();
  return obstacle_tiles[layer][(y / 8) * width8 + (x / 8)];
}

/**
 * Returns the entities on a layer, such that the hero cannot walk on them
 * (except the tiles).
 * @param layer the layer
 * @return the obstacle entities
 */
list<MapEntity*> * MapEntities::get_obstacle_entities(Layer layer) {
  return &obstacle_entities[layer];
}

/**
 * Returns all detectors of the map.
 * @return the detectors
 */
list<Detector*> * MapEntities::get_detectors(void) {
  return &detectors;
}

/**
 * Returns all crystal switch blocks on the specified layer.
 * @param layer the layer
 * @return the crystal switch blocks on this layer
 */
list<CrystalSwitchBlock*> * MapEntities::get_crystal_switch_blocks(Layer layer) {
  return &crystal_switch_blocks[layer];
}

/**
 * Sets the obstacle tile property of an 8*8 square of the map.
 * @param layer layer of the square
 * @param x8 x coordinate of the square (divided by 8)
 * @param y8 y coordinate of the square (divided by 8)
 * @param obstacle the obstacle property to set
 */
void MapEntities::set_obstacle(int layer, int x8, int y8, Obstacle obstacle) {

  int width8 = map->get_width8();
  int height8 = map->get_height8();
  if (x8 >= 0 && x8 < width8 && y8 >= 0 && y8 < height8) {
    int index = y8 * width8 + x8;
    obstacle_tiles[layer][index] = obstacle;
  }
}

/**
 * Returns the entity with the specified type and name.
 * The program stops if there is no such entity.
 * @param type type of entity
 * @param name name of the entity to get
 * @return the entity requested
 */
MapEntity * MapEntities::get_entity(EntityType type, const std::string &name) {

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {

    MapEntity *entity = *i;
    if (entity->get_type() == type && entity->get_name() == name) {
      return entity;
    }
  }

  DIE("Cannot find entity with type '" << type << "' and name '" << name << "'");
}

/**
 * Returns all entities of the map with the specified type.
 * Don't forget to delete the list when you don't need it anymore.
 * @param type type of entity
 * @return the entities of this type
 */
list<MapEntity*> * MapEntities::get_entities(EntityType type) {

  list<MapEntity*> *entities = new list<MapEntity*>();

  list<MapEntity*>::iterator i;
  for (i = all_entities.begin(); i != all_entities.end(); i++) {

    MapEntity *entity = *i;
    if (entity->get_type() == type) {
      entities->push_back(entity);
    }
  }

  return entities;
}

/**
 * Brings to front an entity that is displayed as a 
 * sprite in the normal order.
 * @param entity the entity to bring to front
 */
void MapEntities::bring_to_front(MapEntity *entity) {

  if (!entity->can_be_displayed()) {
    DIE("Cannot bring to front entity '" << entity->get_name()
	<< "' since it is not displayed");
  }

  if (entity->is_displayed_in_y_order()) {
    DIE("Cannot bring to front entity '" << entity->get_name()
	<< "' since it is displayed in the y order");
  }

  entities_displayed_first->remove(entity);
  entities_displayed_first->push_back(entity);
}

/**
 * Creates a tile on the map.
 * This function is called for each tile when loading the map.
 * The tiles cannot change during the game.
 * @param tile the tile to add
 */
void MapEntities::add_tile(Tile *tile) {

  Layer layer = tile->get_layer();

  // add the tile to the map
  tiles[layer].push_back(tile);
  tile->set_map(map);

  // update the collision list
  Obstacle obstacle = tile->get_tile_pattern()->get_obstacle();

  int tile_x8 = tile->get_x() / 8;
  int tile_y8 = tile->get_y() / 8;
  int tile_width8 = tile->get_width() / 8;
  int tile_height8 = tile->get_height() / 8;

  int i, j;
 
  for (i = 0; i < tile_height8; i++) {
    for (j = 0; j < tile_width8; j++) {
      set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE_NONE);
    }
  }

  switch (obstacle) {

    /* If the obstacle property is the same for all points inside the base tile,
     * then all 8*8 squares of the extended tile have the same property.
     */
  case OBSTACLE_NONE:
  case OBSTACLE_SHALLOW_WATER:
  case OBSTACLE_DEEP_WATER:
  case OBSTACLE_HOLE:
  case OBSTACLE:
    for (i = 0; i < tile_height8; i++) {
      for (j = 0; j < tile_width8; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, obstacle);
      }
    }
    break;

    /* If the top right corner of the tile is an obstacle,
     * then the top right 8*8 squares are OBSTACLE, the bottom left
     * 8*8 squares are NO_OBSTACLE and the 8*8 squares on the diagonal
     * are OBSTACLE_TOP_RIGHT.
     */
  case OBSTACLE_TOP_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + i, tile_y8 + i, OBSTACLE_TOP_RIGHT);

      // right part of the row: we are in the top-right corner
      for (j = i + 1; j < tile_width8; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE);
      }
    }
    break;

  case OBSTACLE_TOP_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // left part of the row: we are in the top-left corner
      for (j = 0; j < tile_width8 - i - 1; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE);
      }

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE_TOP_LEFT);
    }
    break;

  case OBSTACLE_BOTTOM_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // left part of the row: we are in the bottom-left corner
      for (j = 0; j < i; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE);
      }

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE_BOTTOM_LEFT);
    }
    break;

  case OBSTACLE_BOTTOM_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + tile_width8 - i - 1, tile_y8 + i, OBSTACLE_BOTTOM_RIGHT);

      // right part of the row: we are in the bottom-right corner
      for (j = tile_width8 - i; j < tile_width8; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, OBSTACLE);
      }
    }
    break;
  }
}

/**
 * Adds a dynamic entity to the map.
 * This function is called when loading the map. If the entity
 * specified is NULL (because some entity creation function
 * sometimes return NULL), nothing is done.
 * @param entity the entity to add (can be NULL)
 */
void MapEntities::add_entity(MapEntity *entity) {

  if (entity == NULL) {
    return;
  }

  if (entity->get_type() == TILE) {
    DIE("This entity is not dynamic");
  }

  Layer layer = entity->get_layer();

  // update the obstacle list
  if (entity->can_be_obstacle()) {
    obstacle_entities[layer].push_back(entity);
  }

  // update the detectors list
  if (entity->can_detect_entities()) {
    detectors.push_back((Detector*) entity);
  }

  // update the sprites list
  if (entity->is_displayed_in_y_order()) {
    entities_displayed_y_order[layer].push_back(entity);
  }
  else if (entity->can_be_displayed()) {
    entities_displayed_first[layer].push_back(entity);
  }

  // update the specific entities lists
  switch (entity->get_type()) {

    case DESTINATION_POINT:
      destination_points.push_back((DestinationPoint*) entity);
      break;

    case CRYSTAL_SWITCH_BLOCK:
      crystal_switch_blocks[layer].push_back((CrystalSwitchBlock*) entity);
      break;

    case BOOMERANG:
      this->boomerang = (Boomerang*) entity;
      break;

    default:
      break;
  }

  // update the list of all entities
  all_entities.push_back(entity);

  entity->set_map(map);
}

/**
 * Removes an entity from the map and schedules it to be destroyed.
 * @param entity the entity to remove
 */
void MapEntities::remove_entity(MapEntity *entity) {

  entities_to_remove.push_back(entity);
  entity->set_being_removed();

  if (entity == (MapEntity*) this->boomerang) {
    this->boomerang = NULL;
  }
}

/**
 * Removes an entity from the map and schedules it to be destroyed.
 * @param type type of the entity to remove
 * @param name name of the entity
 */
void MapEntities::remove_entity(EntityType type, const std::string &name) {
  remove_entity(get_entity(type, name));
}

/**
 * Removes and destroys the entities placed in the entities_to_remove list. 
 */
void MapEntities::remove_marked_entities(void) {

  list<MapEntity*>::iterator it;

  // remove the marked entities
  for (it = entities_to_remove.begin();
       it != entities_to_remove.end();
       it++) {

    MapEntity *entity = *it;
    Layer layer = entity->get_layer();

    // remove it from the obstacle entities list if present
    if (entity->can_be_obstacle()) {
      obstacle_entities[layer].remove(entity);
    }

    // remove it from the detectors list if present
    if (entity->can_detect_entities()) {
      detectors.remove((Detector*) entity);
    }

    // remove it from the sprite entities list if present
    if (entity->is_displayed_in_y_order()) {
      entities_displayed_y_order[layer].remove(entity);
    }
    else if (entity->can_be_displayed()) {
      entities_displayed_first[layer].remove(entity);
    }

    // remove it from the whole list
    all_entities.remove(entity);

    // destroy it
    delete entity;
  }
  entities_to_remove.clear();
}

/**
 * Suspends or resumes the movement and animations of the
 * entities.
 * This function is called by the map when the game
 * is being suspended or resumed.
 * @param suspended true to suspend the movement and the animations,
 * false to resume them
 */
void MapEntities::set_suspended(bool suspended) {

  // the hero first
  Hero *hero = zsdx->game->get_hero();
  hero->set_suspended(suspended);

  // other entities
  list<MapEntity*>::iterator i;
  for (i = all_entities.begin();
       i != all_entities.end();
       i++) {

    (*i)->set_suspended(suspended);
  }

  // note that we don't suspend the animated tiles
}

/**
 * Updates the position, movement and animation each entity.
 */
void MapEntities::update(void) {

  // first update the hero
  Hero *hero = zsdx->game->get_hero();
  hero->update();
  update_crystal_switch_blocks();

  // update the tiles and the dynamic entities
  list<MapEntity*>::iterator it;
  for (int layer = 0; layer < LAYER_NB; layer++) {

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      tiles[layer][i]->update();
    }

    // sort the entities displayed in y order
    entities_displayed_y_order[layer].sort(compare_y);
  }

  for (it = all_entities.begin();
       it != all_entities.end();
       it++) {

    if (!(*it)->is_being_removed()) {
      (*it)->update();
    }
  }

  // remove the entities that have to be removed now
  remove_marked_entities();
}

/**
 * Displays the entities on the map surface.
 */
void MapEntities::display() {

  for (int layer = 0; layer < LAYER_NB; layer++) {

    // put the tiles
    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      tiles[layer][i]->display_on_map();
    }

    // put the first sprites
    list<MapEntity*>::iterator i;
    for (i = entities_displayed_first[layer].begin();
	 i != entities_displayed_first[layer].end();
	 i++) {
      (*i)->display_on_map();
    }

    // put the sprites displayed at the hero's level, in the order
    // defined by their y position (including the hero)
    for (i = entities_displayed_y_order[layer].begin();
	 i != entities_displayed_y_order[layer].end();
	 i++) {
      (*i)->display_on_map();
    }
  }
}

/**
 * Compares the y position of two entities.
 * @param first an entity
 * @param second another entity
 * @return true if the y position of the first entity is lower
 * than the second one
 */
bool MapEntities::compare_y(MapEntity *first, MapEntity *second) {
  return first->get_y() < second->get_y();
}

/**
 * Changes the layer of the hero.
 * @param layer the new layer
 */
void MapEntities::set_hero_layer(Layer layer) {

  Hero *hero = zsdx->game->get_hero();
  Layer old_layer = hero->get_layer();

  if (layer != old_layer) {

    this->obstacle_entities[old_layer].remove(hero);
    this->entities_displayed_y_order[old_layer].remove(hero);

    hero->set_layer(layer);

    this->obstacle_entities[layer].push_back(hero);
    this->entities_displayed_y_order[layer].push_back(hero);
  }
}

/**
 * Updates the crystal switch blocks.
 */
void MapEntities::update_crystal_switch_blocks(void) {

  Hero *hero = zsdx->game->get_hero();
  hero_on_raised_blocks = overlaps_raised_blocks(hero->get_layer(), hero->get_position_in_map());
}

/**
 * Returns whether a rectangle overlaps with a raised crystal switch block.
 * @param layer the layer to check
 * @param rectangle a rectangle
 * @return true if this rectangle overlaps a raised crystal switch block
 */
bool MapEntities::overlaps_raised_blocks(Layer layer, const SDL_Rect &rectangle) {

  bool overlaps = false;
  std::list<CrystalSwitchBlock*> *blocks = get_crystal_switch_blocks(layer);

  std::list<CrystalSwitchBlock*>::iterator it;
  for (it = blocks->begin(); it != blocks->end() && !overlaps; it++) {
    overlaps = (*it)->overlaps(rectangle) && (*it)->is_raised();
  }

  return overlaps;
}

/**
 * Returns whether the hero is currently on raised crystal switch blocks.
 * @return true if the hero is currently on raised crystal switch blocks
 */
bool MapEntities::is_hero_on_raised_blocks(void) {
  return hero_on_raised_blocks;
}

/**
 * Returns true if the player has thrown the boomerang.
 * @return true if the boomerang is present on the map
 */
bool MapEntities::is_boomerang_present(void) {
  return boomerang != NULL;
}

/**
 * Removes the boomerang from the map, if it is present.
 */
void MapEntities::remove_boomerang(void) {

  if (boomerang != NULL) {
    remove_entity(boomerang);
    boomerang = NULL;
  }
}
