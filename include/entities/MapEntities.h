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
#include "movements/FallingOnFloorMovement.h"
#include <vector>
#include <list>

/**
 * Stores all entities of a map: the tiles, the hero and all other entities.
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
  std::vector<TileOnMap*> tiles[MapEntity::LAYER_NB];

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
  std::list<MapEntity*> all_entities;

  /**
   * List of entities to remove now.
   */
  std::list<MapEntity*> entities_to_remove;

  /**
   * All map entities that are displayed as a sprite (a vector for each layer).
   */
  std::list<MapEntity*> sprite_entities[MapEntity::LAYER_NB];

  /**
   * Vector of all possible destination points of the map.
   */
  std::vector<DestinationPoint*> destination_points;

  /**
   * All detectors of the map.
   */
  std::list<Detector*> detectors;
  
  /**
   * All obstacle entities of the map.
   * Note that the obstacle notion is relative, that is, there exists entities that
   * are obstacle for some targets and non obstacle for others. This list contains
   * all entities that might be obstacle for at least one entity.
   */
  std::list<MapEntity*> obstacle_entities[MapEntity::LAYER_NB];

  void add_entity(MapEntity *entity);

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
  void set_obstacle(MapEntity *entity, MapEntity::Obstacle obstacle);
  void set_obstacle(int layer, int x8, int y8, MapEntity::Obstacle obstacle);
  MapEntity *get_entity(MapEntity::EntityType type, string name);

  void bring_to_front(MapEntity *sprite_entity);

  // add and remove entities
  void add_tile(int tile_id, MapEntity::Layer layer, int x, int y, int width, int height);

  void add_destination_point(string destination_point_name, MapEntity::Layer layer,
			     int x, int y, int hero_direction, bool is_visible);

  void add_teletransporter(string teletransporter_name, MapEntity::Layer layer, int x, int y, int w, int h,
			   Teletransporter::Subtype subtype, Transition::Style transition_style,
			   MapId map_id, string destination_point_name);

  void add_pickable_item(MapEntity::Layer layer, int x, int y, PickableItem::ItemType pickable_item_type,
			 int savegame_variable, FallingOnFloorMovement::Height falling_height, bool will_disappear);
  void remove_pickable_item(PickableItem *item);

  void add_destructible_item(MapEntity::Layer layer, int x, int y, DestructibleItem::ItemType destructible_item_type,
			      PickableItem::ItemType pickable_item_type, int savegame_variable);
  void remove_destructible_item(DestructibleItem *item);

  void add_chest(string chest_name, MapEntity::Layer layer, int x, int y,
		 bool big_chest, int treasure_content,
		 int treasure_amount, int treasure_savegame_variable);

  void add_jump_sensor(string name, MapEntity::Layer layer,
		       int x, int y, int width, int height, int direction, int jump_length);

  void add_enemy(string name, MapEntity::Layer layer, int x, int y, int direction,
		 Enemy::EnemyType enemy_type, Enemy::Rank rank, int savegame_variable,
		 PickableItem::ItemType pickable_item_type, int pickable_item_savegame_variable);

  void add_interactive_entity(string name, MapEntity::Layer layer, int x, int y,
			      InteractiveEntity::SpecialInteraction special_interaction,
			      SpriteAnimationSetId sprite_name, int initial_direction,
			      MessageId message_to_show);

  void remove_enemy(Enemy *enemy);

  void remove_marked_entities(void);

  void destroy_all_entities(void);

  // update and display
  void set_suspended(bool suspended);
  void update(void);
  void display(void);
};

#endif
