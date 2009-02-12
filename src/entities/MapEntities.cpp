#include "entities/MapEntities.h"
#include "entities/Tileset.h"
#include "entities/Tile.h"
#include "entities/TileOnMap.h"
#include "entities/Teletransporter.h"
#include "entities/PickableItem.h"
#include "entities/DestructibleItem.h"
#include "entities/DestinationPoint.h"
#include "entities/Hero.h"
#include "entities/Chest.h"
#include "entities/JumpSensor.h"
#include "Treasure.h"
#include "Map.h"
#include "ZSDX.h"
#include "Game.h"
using std::list;

/**
 * Constructor.
 */
MapEntities::MapEntities(Map *map) {
  this->map = map;
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
  for (int layer = 0; layer < MapEntity::LAYER_NB; layer++) {

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      delete tiles[layer][i];
    }

    tiles[layer].clear();
    delete[] obstacle_tiles[layer];

    sprite_entities[layer].clear();
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
MapEntity::Obstacle MapEntities::get_obstacle_tile(MapEntity::Layer layer, int x, int y) {

  int width8 = map->get_width8();
  return obstacle_tiles[layer][(y / 8) * width8 + (x / 8)];
}

/**
 * Returns the entities on a layer, such that the hero cannot walk on them
 * (except the tiles).
 * @param layer the layer
 * @return the obstacle entities
 */
list<MapEntity*> * MapEntities::get_obstacle_entities(MapEntity::Layer layer) {
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
 * Sets the obstacle property of an entity.
 * @param entity an entity
 * @param obstacle the obstacle property to set
 */
void MapEntities::set_obstacle(MapEntity *entity, MapEntity::Obstacle obstacle) {

  MapEntity::Layer layer = entity->get_layer();

  obstacle_entities[layer].remove(entity);
  if (obstacle != MapEntity::OBSTACLE_NONE) {
    obstacle_entities[layer].push_back(entity);
  }
}

/**
 * Sets the obstacle tile property of an 8*8 square of the map.
 * @param layer layer of the square
 * @param x8 x coordinate of the square (divided by 8)
 * @param y8 y coordinate of the square (divided by 8)
 * @param obstacle the obstacle property to set
 */
void MapEntities::set_obstacle(int layer, int x8, int y8, MapEntity::Obstacle obstacle) {

  int width8 = map->get_width8();
  int height8 = map->get_height8();
  if (x8 >= 0 && x8 < width8 && y8 >= 0 && y8 < height8) {
    int index = y8 * width8 + x8;
    obstacle_tiles[layer][index] = obstacle;
  }
}

/**
 * Brings to front an entity displayed as a sprite.
 * @param sprite_entity the entity to bring to front
 */
void MapEntities::bring_to_front(MapEntity *sprite_entity) {
  sprite_entities->remove(sprite_entity);
  sprite_entities->push_back(sprite_entity);
}

/**
 * Creates a tile on the map.
 * This function is called for each tile when loading the map.
 * The tiles on a map are not supposed to change during the game.
 * @param tile_id id of the tile in the tileset
 * @param layer layer of the tile to create
 * @param x x position of the tile on the map
 * @param y y position of the tile on the map
 * @param width width in pixels (the pattern will be repeated on x to fit this width)
 * @param height height in pixels (the pattern will be repeated on y to fit this height
 */
void MapEntities::add_tile(int tile_id, MapEntity::Layer layer, int x, int y, int width, int height) {

  // get the tile in the tileset
  Tile *tile = map->get_tileset()->get_tile(tile_id);
  MapEntity::Obstacle obstacle = tile->get_obstacle();

  int repeat_x = width / tile->get_width();
  int repeat_y = height / tile->get_height();

  // create the tile object
  TileOnMap *tile_on_map = new TileOnMap(tile, layer, x, y, repeat_x, repeat_y);

  // add it to the map
  tiles[layer].push_back(tile_on_map);
  tile_on_map->set_map(map);

  // update the collision list
  int tile_x8 = x / 8;
  int tile_y8 = y / 8;
  int tile_width8 = width / 8;
  int tile_height8 = height / 8;

  int i, j;
 
  for (i = 0; i < tile_height8; i++) {
    for (j = 0; j < tile_width8; j++) {
      set_obstacle(layer, tile_x8 + j, tile_y8 + i, MapEntity::OBSTACLE_NONE);
    }
  }

  switch (obstacle) {

    /* If the tile is entirely an obstacle or entirely no obstacle,
     * then all 8*8 squares of the tile have the same property.
     */
  case MapEntity::OBSTACLE_NONE:
  case MapEntity::OBSTACLE:
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
  case MapEntity::OBSTACLE_TOP_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + i, tile_y8 + i, MapEntity::OBSTACLE_TOP_RIGHT);

      // right part of the row: we are in the top-right corner
      for (j = i + 1; j < tile_width8; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, MapEntity::OBSTACLE);
      }
    }
    break;

  case MapEntity::OBSTACLE_TOP_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // left part of the row: we are in the top-left corner
      for (j = 0; j < tile_width8 - i - 1; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, MapEntity::OBSTACLE);
      }

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + j, tile_y8 + i, MapEntity::OBSTACLE_TOP_LEFT);
    }
    break;

  case MapEntity::OBSTACLE_BOTTOM_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // left part of the row: we are in the bottom-left corner
      for (j = 0; j < i; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, MapEntity::OBSTACLE);
      }

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + j, tile_y8 + i, MapEntity::OBSTACLE_BOTTOM_LEFT);
    }
    break;

  case MapEntity::OBSTACLE_BOTTOM_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      // 8*8 square on the diagonal
      set_obstacle(layer, tile_x8 + tile_width8 - i - 1, tile_y8 + i, MapEntity::OBSTACLE_BOTTOM_RIGHT);

      // right part of the row: we are in the bottom-right corner
      for (j = tile_width8 - i; j < tile_width8; j++) {
	set_obstacle(layer, tile_x8 + j, tile_y8 + i, MapEntity::OBSTACLE);
      }
    }
    break;
  }
}

/**
 * Adds any kind of entity except a tile.
 * @param entity the entity to add
 */
void MapEntities::add_entity(MapEntity *entity) {
  all_entities.push_back(entity);
  entity->set_map(map);
}

/**
 * Creates a destination point on the map.
 * This function is called for each destination point when loading the map.
 * @param destination_point_name a string identifying this new destination point
 * @param layer the layer of the hero's position
 * @param x x position of the destination point to create
 * @param y y position of the destination point to create
 * @param hero_direction initial direction of the hero in this state (0 to 3, or -1
 * to indicate that the hero's direction is not changed)
 * @param is_visible true to make the destination point visible
 */
void MapEntities::add_destination_point(string destination_point_name, MapEntity::Layer layer,
					int x, int y, int hero_direction, bool is_visible) {
  
  DestinationPoint *destination_point = new DestinationPoint(destination_point_name, layer,
							     x, y, hero_direction, is_visible);
  destination_points.push_back(destination_point);
  add_entity(destination_point);
}

/**
 * Creates a teletransporter on the map.
 * This function is called for each teletransporter when loading the map.
 * When the hero walks on the teletransporter, he he transported to a destination point
 * on the current map or another one.
 * @param teletransporter_name a string identifying this new teletransporter
 * @param layer layer of the teletransporter to create
 * @param x x position of the teletransporter rectangle
 * @param y y position of the teletransporter rectangle
 * @param w width of the teletransporter rectangle
 * @param h height of the teletransporter rectangle
 * @param subtype subtype of teletransporter
 * @param transition_style type of transition between the two maps
 * @param map_id id of the destination map (can be the current map)
 * @param destination_point_name name of the destination point on the destination map
 */
void MapEntities::add_teletransporter(string teletransporter_name, MapEntity::Layer layer, int x, int y, int w, int h,
				      Teletransporter::Subtype subtype, Transition::Style transition_style,
				      MapId destination_map_id, string destination_point_name) {
  
  Teletransporter *teletransporter = new Teletransporter(teletransporter_name, layer, x, y, w, h,
							 subtype, transition_style, destination_map_id,
							 destination_point_name);
  detectors.push_back(teletransporter);
  obstacle_entities[layer].push_back(teletransporter); // not obstacle for the hero but for enemies and NPCs 
  add_entity(teletransporter);
}

/**
 * Creates a pickable item on the map.
 * This function is called when loading the map if it already contains pickable items (e.g. fairies
 * or rupees). It is also called when playing on the map, e.g. when the hero lifts a pot or kill an enemy.
 * When the hero walks on the item, he picks it.
 * @param layer layer of the pickable item
 * @param x x position of the pickable item
 * @param y y position of the pickable item
 * @param pickable_item_type type of pickable item to create
 * (can be a normal item, NONE or RANDOM)
 * @param savegame_variable index of the savegame boolean variable storing
 * the possession state of the pickable item,
 * for certain kinds of pickable items only (a key, a piece of heart...)
 * @param falling_height to make the item falling when it appears (ignored for a fairy)
 * @param will_disappear true to make the item disappear after an amout of time
 */
void MapEntities::add_pickable_item(MapEntity::Layer layer, int x, int y,
				    PickableItem::ItemType pickable_item_type, int savegame_variable,
				    FallingOnFloorMovement::Height falling_height, bool will_disappear) {

  PickableItem *item = PickableItem::create(layer, x, y, pickable_item_type,
					    savegame_variable, falling_height, will_disappear);

  // item can be NULL if the type was NONE or RANDOM
  if (item != NULL) {

    layer = item->get_layer(); // well, some items set their own layer

    sprite_entities[layer].push_back(item);
    detectors.push_back(item);
    add_entity(item);
  }
}

/**
 * Removes a pickable item from the map and destroys it.
 * @param item the pickable item to remove
 */
void MapEntities::remove_pickable_item(PickableItem *item) {

  entities_to_remove.push_back(item);
  item->set_being_removed();
}

/**
 * Creates a destructible item on the map.
 * @param layer layer of the destructible item
 * @param x x position of the destructible item
 * @param y y position of the destructible item
 * @param destructible_item_type type of destructible item to create
 * @param pickable_item_type type of pickable item that appears when the
 * destructible item is removed
 * @param savegame_variable index of the savegame boolean variable indicating
 * the possession state of the pickable item,
 * for certain kinds of pickable items only (a key, a piece of heart...)
 */
void MapEntities::add_destructible_item(MapEntity::Layer layer, int x, int y,
					 DestructibleItem::ItemType destructible_item_type,
					 PickableItem::ItemType pickable_item_type, int savegame_variable) {

  DestructibleItem *item = new DestructibleItem(layer, x, y, destructible_item_type,
						pickable_item_type, savegame_variable);
  
  sprite_entities[layer].push_back(item);
  detectors.push_back(item);
  obstacle_entities[layer].push_back(item);

  add_entity(item);
}

/**
 * Removes a destructible item from the map and destroys it.
 * @param item the destructible item to remove
 */
void MapEntities::remove_destructible_item(DestructibleItem *item) {
  entities_to_remove.push_back(item);
  item->set_being_removed();
}

/**
 * Creates a chest on the map.
 * @param name name identifying the chest
 * @param layer layer of the chest to create
 * @param x x coordinate of the chest to create
 * @param y y coordinate of the chest to create
 * @param big_chest true to make a big chest, false to make a normal chest
 * @param treasure_content content of the treasure in this chest, or -1 to make an empty chest
 * @param treasure_amount amout of the treasure (for some kinds of treasure only)
 * @param treasure_savegame_variable index of the savegame boolean variable where this treasure's
 * possession state is saved
 */
void MapEntities::add_chest(string chest_name, MapEntity::Layer layer, int x, int y,
			    bool big_chest, int treasure_content,
			    int treasure_amount, int treasure_savegame_variable) {
  
  Treasure *treasure = new Treasure((Treasure::Content) treasure_content, treasure_amount, treasure_savegame_variable);
  Chest *chest = new Chest(chest_name, layer, x, y, big_chest, treasure);

  sprite_entities[layer].push_back(chest);
  detectors.push_back(chest);
  obstacle_entities[layer].push_back(chest);
  add_entity(chest);
}

/**
 * Creates a jump sensor on the map.
 * This function is called for each jump sensor when loading the map.
 * @param name a string identifying this new jump sensor
 * @param layer the layer of this sensor
 * @param x x position of the jump sensor to create
 * @param y y position of the jump sensor to create
 * @param width width of the jump sensor to create
 * @param height height of the jump sensor to create
 * @param direction direction of the jump (0 to 7)
 * @param jump length length of the jump in pixels
 */
void MapEntities::add_jump_sensor(string name, MapEntity::Layer layer,
				  int x, int y, int width, int height, int direction, int jump_length) {

  JumpSensor *jump_sensor = new JumpSensor(name, layer, x, y, width, height, direction, jump_length);

  detectors.push_back(jump_sensor);
  add_entity(jump_sensor);
}

/**
 * Creates an enemy on the map.
 * See the documentation of class Enemy for the meaning of each parameter.
 */
void MapEntities::add_enemy(string name, MapEntity::Layer layer, int x, int y, int direction,
			    Enemy::EnemyType enemy_type, Enemy::Rank rank, int savegame_variable,
			    PickableItem::ItemType pickable_item_type, int pickable_item_savegame_variable) {

  Enemy *enemy = Enemy::create(enemy_type, rank, savegame_variable, name, layer, x, y, direction,
			       pickable_item_type, pickable_item_savegame_variable);

  if (enemy != NULL) {

    sprite_entities[layer].push_back(enemy);
    detectors.push_back(enemy);
    add_entity(enemy);
  }
}

/**
 * Removes an enemy from the map.
 * @param enemy the enemy to remove
 */
void MapEntities::remove_enemy(Enemy *enemy) {
  entities_to_remove.push_back(enemy);
  enemy->set_being_removed();
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

    MapEntity::Layer layer = (*it)->get_layer();

    // remove it from the detectors list if present
    // (the cast may be invalid but this is just a pointer comparison)
    detectors.remove((Detector*) (*it));

    // remove it from the obstacle entities list if present
    obstacle_entities[layer].remove(*it);

    // remove it from the sprite entities list if present
    sprite_entities[layer].remove(*it);

    // remove it from the whole list
    all_entities.remove(*it);

    // destroy it
    delete *it;
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

  // the hero
  Hero *hero = zsdx->game->get_hero();
  hero->set_suspended(suspended);

  // other entities
  list<MapEntity*>::iterator i;
  for (int layer = 0; layer < MapEntity::LAYER_NB; layer++) {

    for (i = sprite_entities[layer].begin();
	 i != sprite_entities[layer].end();
	 i++) {
      (*i)->set_suspended(suspended);
    }

    // note that we don't suspend the animated tiles
  }
}

/**
 * Updates the animation and position of each entity.
 */
void MapEntities::update(void) {
  
  // update the hero's position, movement and animation
  Hero *hero = zsdx->game->get_hero();
  hero->update();

  // update the animated tiles and sprites
  list<MapEntity*>::iterator it;
  for (int layer = 0; layer < MapEntity::LAYER_NB; layer++) {

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      tiles[layer][i]->update();
    }

    for (it = sprite_entities[layer].begin();
	 it != sprite_entities[layer].end();
	 it++) {

      if (!(*it)->is_being_removed()) {
	(*it)->update();
      }
    }
  }

  // remove the entities that have to be removed now
  remove_marked_entities();
}

/**
 * Displays the map with all its entities on the screen.
 */
void MapEntities::display() {

  Hero *hero = zsdx->game->get_hero();

  // map entities
  for (int layer = 0; layer < MapEntity::LAYER_NB; layer++) {

    // put the tiles
    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      tiles[layer][i]->display_on_map();
    }

    // put the animated sprites
    list<MapEntity*>::iterator i;
    for (i = sprite_entities[layer].begin();
	 i != sprite_entities[layer].end();
	 i++) {
      (*i)->display_on_map();
    }

    // put the hero if he is in this layer
    if (hero->get_layer() == layer) {
      hero->display_on_map();
    }
  }
}
