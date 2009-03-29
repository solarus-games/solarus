#ifndef ZSDX_MAP_ENTITIES_H
#define ZSDX_MAP_ENTITIES_H

#include "Common.h"
#include "MapEntity.h"
#include "Transition.h"
#include "entities/Teletransporter.h"
#include "entities/PickableItem.h"
#include "entities/DestructibleItem.h"
#include "entities/Enemy.h"
#include "entities/InteractiveEntity.h"
#include "entities/Block.h"
#include "entities/Switch.h"
#include "movements/FallingOnFloorMovement.h"
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
  std::vector<Tile*> tiles[MapEntity::LAYER_NB];          /**< all tiles of the map (a vector for each layer) */
  int obstacle_tiles_size;                                /**< number of elements in the array obstacle_tiles
							   * (obstacle_tiles_size = map->width8 * map->height8) */
  MapEntity::Obstacle
    *obstacle_tiles[MapEntity::LAYER_NB];                 /**< array of Obstacle representing which
					                   * tiles are obstacles and how */
  // dynamic entities

  std::list<MapEntity*> all_entities;                     /**< all map entities execept the tiles and the hero;
				                           * this vector is used to delete the entities 
				                           * when the map is unloaded */
  std::list<MapEntity*> entities_to_remove;               /**< list of entities that need to be removed now */

  std::list<MapEntity*>
    entities_displayed_first[MapEntity::LAYER_NB];        /**< all map entities that are displayed in the normal order */

  std::list<MapEntity*>
    entities_displayed_y_order[MapEntity::LAYER_NB];      /**< all map entities that are displayed in the order
							   * defined by their y position, including the hero */

  std::vector<DestinationPoint*> destination_points;      /**< all destination points of the map */
  std::list<Detector*> detectors;                         /**< all entities able to detect other entities
							   * on this map */
  
  std::list<MapEntity*>
    obstacle_entities[MapEntity::LAYER_NB];               /**< all entities that might be obstacle for other
							   * entities on this map, including the hero */

  void set_obstacle(int layer, int x8, int y8, MapEntity::Obstacle obstacle);
  void remove_marked_entities(void);

 public:

  // creation and destruction
  MapEntities(Map *map);
  ~MapEntities(void);

  // information about the entities
  unsigned int get_nb_destination_points(void);
  DestinationPoint * get_destination_point(int index);
  MapEntity::Obstacle get_obstacle_tile(MapEntity::Layer layer, int x, int y);
  std::list<MapEntity*> * get_obstacle_entities(MapEntity::Layer layer);
  std::list<Detector*> * get_detectors(void);

  MapEntity *get_entity(MapEntity::EntityType type, string name);
  std::list<MapEntity*> * get_entities(MapEntity::EntityType type);

  // handle entities
  void add_tile(Tile *tile);
  void add_entity(MapEntity *entity);
  void remove_entity(MapEntity *entity);
  void remove_entity(MapEntity::EntityType type, string name);
  void bring_to_front(MapEntity *entity);
  void set_y_order(MapEntity *entity);
  void destroy_all_entities(void);

  // update and display
  void set_suspended(bool suspended);
  void update(void);
  void display(void);
};

#endif
