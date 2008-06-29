#ifndef ZSDX_MAP_ENTITIES_H
#define ZSDX_MAP_ENTITIES_H

#include "Common.h"
#include "Layer.h"
#include "Obstacle.h"
#include "TransitionEffect.h"
#include "PickableItemType.h"
#include "MovementFallingHeight.h"
#include "TransportableItemType.h"
#include <vector>
#include <list>

/**
 * Stores all entities of a map: the tiles, Link and all other entities.
 */
class MapEntities {

  friend class MapLoader; // the map loader modifies the private fields of MapEntities

 private:

  /**
   * The map.
   */
  Map *map;

  /**
   * The tileset.
   */
  Tileset *tileset;

  /**
   * All tiles of the map (a vector for each layer).
   */
  vector<TileOnMap*> tiles[LAYER_NB];
  
  /**
   * Number of elements in the array obstacle_tiles.
   * obstacle_tiles_size = map->width8 * map->height8
   */
  int obstacle_tiles_size;
  
  /**
   * Array of Obstacle representing which tiles are obstacles and how.
   */
  Obstacle *obstacle_tiles[LAYER_NB];

  /**
   * All map entities execept the tiles.
   * This vector is used to delete the entities when the map is unloaded.
   */
  list<MapEntity*> all_entities;

  /**
   * List of entities to remove now.
   */
  list<MapEntity*> entities_to_remove;

  /**
   * All map entities that are displayed by a sprite (a vector for each layer).
   */
  list<MapEntity*> sprite_entities[LAYER_NB];

  /**
   * Vector of all possible entrances of the map.
   */
  vector<MapEntrance*> entrances;

  /**
   * All entity detectors of the map.
   */
  list<EntityDetector*> entity_detectors;
  
  /**
   * All obstacle entities of the map.
   */
  list<MapEntity*> obstacle_entities[LAYER_NB];

 public:

  // creation and destruction
  MapEntities(Map *map);
  ~MapEntities(void);

  // information about the entities
  unsigned int get_nb_entrances(void);
  MapEntrance * get_entrance(int index);
  Obstacle get_obstacle_tile(Layer layer, int x, int y);
  list<MapEntity*> * get_obstacle_entities(Layer layer);
  list<EntityDetector*> * get_entity_detectors(void);
 
  // add and remove entities
  void add_tile(int tile_id, Layer layer, int x, int y, int width, int height);
  void add_entrance(string entrance_name, Layer layer, int link_x, int link_y, int link_direction);
  void add_exit(string exit_name, Layer layer, int x, int y, int w, int h,
		TransitionType transition_type, MapId map_id, string entrance_name);

  void add_pickable_item(Layer layer, int x, int y, PickableItemType pickable_item_type,
			 int unique_id, MovementFallingHeight falling, bool will_disappear);
  void remove_pickable_item(PickableItem *item);

  void add_transportable_item(Layer layer, int x, int y, TransportableItemType transportable_item_type,
			      PickableItemType pickable_item_type);
  void remove_transportable_item(TransportableItem *item);

  void remove_marked_entities(void);

  // update and display
  void set_suspended(bool suspended);
  void update(void);
  void display(void);
};

#endif
