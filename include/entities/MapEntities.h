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
#ifndef ZSDX_MAP_ENTITIES_H
#define ZSDX_MAP_ENTITIES_H

#include "Common.h"
#include "Transition.h"
#include "entities/Obstacle.h"
#include "entities/Layer.h"
#include "entities/EntityType.h"
#include <vector>
#include <list>

/**
 * Stores all entities of a map: the tiles, the hero and all other entities.
 */
class MapEntities {

  friend class MapLoader; /**< the map loader sets the private fields of MapEntities */

 private:

  // map
  Map *map;                                               /**< the map */
  Tileset *tileset;                                       /**< the tileset */

  // tiles
  std::vector<Tile*> tiles[LAYER_NB];                     /**< all tiles of the map (a vector for each layer) */
  int obstacle_tiles_size;                                /**< number of elements in the array obstacle_tiles
							   * (obstacle_tiles_size = map->width8 * map->height8) */
  Obstacle *obstacle_tiles[LAYER_NB];                     /**< array of Obstacle representing which
					                   * tiles are obstacles and how */
  // dynamic entities

  std::list<MapEntity*> all_entities;                     /**< all map entities execept the tiles and the hero;
				                           * this vector is used to delete the entities 
				                           * when the map is unloaded */
  std::list<MapEntity*> entities_to_remove;               /**< list of entities that need to be removed now */

  std::list<MapEntity*>
    entities_displayed_first[LAYER_NB];                   /**< all map entities that are displayed in the normal order */

  std::list<MapEntity*>
    entities_displayed_y_order[LAYER_NB];                 /**< all map entities that are displayed in the order
							   * defined by their y position, including the hero */

  std::vector<DestinationPoint*> destination_points;      /**< all destination points of the map */
  std::list<Detector*> detectors;                         /**< all entities able to detect other entities
							   * on this map */
  
  std::list<MapEntity*>
    obstacle_entities[LAYER_NB];                          /**< all entities that might be obstacle for other
							   * entities on this map, including the hero */

  void set_obstacle(int layer, int x8, int y8, Obstacle obstacle);
  void remove_marked_entities(void);

 public:

  // creation and destruction
  MapEntities(Map *map);
  ~MapEntities(void);

  // information about the entities
  unsigned int get_nb_destination_points(void);
  DestinationPoint * get_destination_point(int index);
  Obstacle get_obstacle_tile(Layer layer, int x, int y);
  std::list<MapEntity*> * get_obstacle_entities(Layer layer);
  std::list<Detector*> * get_detectors(void);

  MapEntity *get_entity(EntityType type, std::string name);
  std::list<MapEntity*> * get_entities(EntityType type);

  // handle entities
  void add_tile(Tile *tile);
  void add_entity(MapEntity *entity);
  void remove_entity(MapEntity *entity);
  void remove_entity(EntityType type, std::string name);
  void bring_to_front(MapEntity *entity);
  void destroy_all_entities(void);
  static bool compare_y(MapEntity *first, MapEntity *second);

  // update and display
  void set_suspended(bool suspended);
  void update(void);
  void display(void);
};

#endif
