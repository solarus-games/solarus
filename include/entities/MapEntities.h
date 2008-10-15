#ifndef ZSDX_MAP_ENTITIES_H
#define ZSDX_MAP_ENTITIES_H

#include "Common.h"
#include "MapEntity.h"
#include "Transition.h"
#include "entities/PickableItem.h"
#include "entities/TransportableItem.h"
#include "movements/MovementFalling.h"
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
  vector<TileOnMap*> tiles[MapEntity::LAYER_NB];
  
  /**
   * Number of elements in the array obstacle_tiles.
   * obstacle_tiles_size = map->width8 * map->height8
   */
  int obstacle_tiles_size;
  
  /**
   * Array of Obstacle representing which tiles are obstacles and how.
   */
  MapEntity::Obstacle *obstacle_tiles[MapEntity::LAYER_NB];

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
   * All map entities that are displayed as a sprite (a vector for each layer).
   */
  list<MapEntity*> sprite_entities[MapEntity::LAYER_NB];

  /**
   * Vector of all possible entrances of the map.
   */
  vector<Entrance*> entrances;

  /**
   * All detectors of the map.
   */
  list<Detector*> detectors;
  
  /**
   * All obstacle entities of the map.
   */
  list<MapEntity*> obstacle_entities[MapEntity::LAYER_NB];

 public:

  // creation and destruction
  MapEntities(Map *map);
  ~MapEntities(void);

  // information about the entities
  unsigned int get_nb_entrances(void);
  Entrance * get_entrance(int index);
  MapEntity::Obstacle get_obstacle_tile(MapEntity::Layer layer, int x, int y);
  list<MapEntity*> * get_obstacle_entities(MapEntity::Layer layer);
  list<Detector*> * get_detectors(void);
 
  // add and remove entities
  void add_tile(int tile_id, MapEntity::Layer layer, int x, int y, int width, int height);
  void add_entrance(string entrance_name, MapEntity::Layer layer, int link_x, int link_y, int link_direction);
  void add_exit(string exit_name, MapEntity::Layer layer, int x, int y, int w, int h,
		Transition::Style transition_style, MapId map_id, string entrance_name);

  void add_pickable_item(MapEntity::Layer layer, int x, int y, PickableItem::ItemType pickable_item_type,
			 int savegame_variable, MovementFalling::FallingHeight falling, bool will_disappear);
  void remove_pickable_item(PickableItem *item);

  void add_transportable_item(MapEntity::Layer layer, int x, int y, TransportableItem::ItemType transportable_item_type,
			      PickableItem::ItemType pickable_item_type, int savegame_variable);
  void remove_transportable_item(TransportableItem *item);

  void add_chest(string chest_name, MapEntity::Layer layer, int x, int y,
		 bool big_chest, int treasure_content,
		 int treasure_amount, int treasure_savegame_variable);

  void remove_marked_entities(void);

  void destroy_all_entities(void);

  // update and display
  void set_suspended(bool suspended);
  void update(void);
  void display(void);
};

#endif
