#include "entities/MapEntities.h"
#include "entities/Tileset.h"
#include "entities/Tile.h"
#include "entities/TileOnMap.h"
#include "entities/Exit.h"
#include "entities/PickableItem.h"
#include "entities/TransportableItem.h"
#include "entities/Entrance.h"
#include "entities/Link.h"
#include "entities/Chest.h"
#include "Treasure.h"
#include "Map.h"
#include "ZSDX.h"
#include "Game.h"

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

  entrances.clear();
  entity_detectors.clear();
  entities_to_remove.clear();
}

/**
 * Returns the number of entrances of the map.
 * @return the number of entrances
 */
unsigned int MapEntities::get_nb_entrances(void) {
  return entrances.size();
}

/**
 * Returns an entrance.
 * @param index index of the entrance to get
 */
Entrance * MapEntities::get_entrance(int index) {
  return entrances[index];
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
 * Returns the entities on a layer, such that Link cannot walk on them
 * (except the tiles).
 * @param layer the layer
 * @return the obstacle entities
 */
list<MapEntity*> * MapEntities::get_obstacle_entities(MapEntity::Layer layer) {
  return &obstacle_entities[layer];
}

/**
 * Returns all entity detectors of the map.
 * @return the entity detectors
 */
list<EntityDetector*> * MapEntities::get_entity_detectors(void) {
  return &entity_detectors;
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
  TileOnMap *tileOnMap = new TileOnMap(tile, layer, x, y, repeat_x, repeat_y);

  // add it to the map
  tiles[layer].push_back(tileOnMap);

  // update the collision list
  int tile_x8 = x / 8;
  int tile_y8 = y / 8;
  int tile_width8 = (tile->get_width() / 8) * repeat_x;
  int tile_height8 = (tile->get_height() / 8) * repeat_y;

  int width8 = map->get_width8();

  int index, i, j;
  

  switch (obstacle) {

    /* If the tile is entirely an obstacle or entirely no obstacle,
     * then all 8*8 squares of the tile have the same property.
     */
  case MapEntity::OBSTACLE_NONE:
  case MapEntity::OBSTACLE:
    for (i = 0; i < tile_height8; i++) {
      index = (tile_y8 + i) * width8 + tile_x8;
      for (j = 0; j < tile_width8; j++) {
	obstacle_tiles[layer][index++] = obstacle;
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

      index = (tile_y8 + i) * width8 + tile_x8;

      // 8*8 square on the diagonal
      index += i;
      obstacle_tiles[layer][index++] = MapEntity::OBSTACLE_TOP_RIGHT;

      // right part of the row: we are in the top-right corner
      for (j = i + 1; j < tile_width8; j++) {
	obstacle_tiles[layer][index++] = MapEntity::OBSTACLE;
      }
    }
    break;
    
  case MapEntity::OBSTACLE_TOP_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // left part of the row: we are in the top-left corner
      for (j = 0; j < tile_width8 - i - 1; j++) {
	obstacle_tiles[layer][index++] = MapEntity::OBSTACLE;
      }

      // 8*8 square on the diagonal
      obstacle_tiles[layer][index] = MapEntity::OBSTACLE_TOP_LEFT;
    }
    break;
    
  case MapEntity::OBSTACLE_BOTTOM_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // left part of the row: we are in the bottom-left corner
      for (j = 0; j < i; j++) {
	obstacle_tiles[layer][index++] = MapEntity::OBSTACLE;
      }

      // 8*8 square on the diagonal
      obstacle_tiles[layer][index] = MapEntity::OBSTACLE_BOTTOM_LEFT;
    }
    break;
    
  case MapEntity::OBSTACLE_BOTTOM_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // 8*8 square on the diagonal
      index += tile_height8 - i - 1;
      obstacle_tiles[layer][index++] = MapEntity::OBSTACLE_BOTTOM_RIGHT;

      // right part of the row: we are in the bottom-right corner
      for (j = tile_width8 - i - 1; j < tile_width8; j++) {
	obstacle_tiles[layer][index++] = MapEntity::OBSTACLE;
      }
    }
    break;
  }
}

/**
 * Creates an entrance on the map.
 * This function is called for each entrance when loading the map.
 * @param entrance_name a string identifying this new entrance
 * @param layer the layer of Link's position
 * @param link_x x initial position of link in this state
 * (set -1 to indicate that the x coordinate is kept the same from the previous map)
 * @param link_y y initial position of link in this state
 * (set -1 to indicate that the y coordinate is kept the same from the previous map)
 * @param link_direction initial direction of link in this state (0 to 3)
 */
void MapEntities::add_entrance(string entrance_name, MapEntity::Layer layer, int link_x, int link_y, int link_direction) {
  
  Entrance *entrance = new Entrance(entrance_name, layer, link_x, link_y, link_direction);
  entrances.push_back(entrance);
  all_entities.push_back(entrance);
}

/**
 * Creates an exit on the map.
 * This function is called for each exit when loading the map.
 * When Link walks on the exit, he leaves the map and enters another one.
 * @param exit_name a string identifying this new exit
 * @param layer layer of the exit to create
 * @param x x position of the exit rectangle
 * @param y y position of the exit rectangle
 * @param w width of the exit rectangle
 * @param h height of the exit rectangle
 * @param transition_style type of transition between the two maps
 * @param map_id id of the next map
 * @param entrance_name name of the entrance of the next map
 */
void MapEntities::add_exit(string exit_name, MapEntity::Layer layer, int x, int y, int w, int h,
			   Transition::Style transition_style, MapId map_id, string entrance_name) {
  
  Exit *exit = new Exit(exit_name, layer, x, y, w, h, transition_style, map_id, entrance_name);
  entity_detectors.push_back(exit);
  all_entities.push_back(exit);
}

/**
 * Creates a pickable item on the map.
 * This function is called when loading the map if it already contains pickable items (e.g. fairies
 * or rupees). It is also called when playing on the map, e.g. when Link lifts a pot or kill an enemy.
 * When Link walks on the item, he picks it.
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
				    MovementFalling::FallingHeight falling_height, bool will_disappear) {

  PickableItem *item = PickableItem::create(map, layer, x, y, pickable_item_type,
					    savegame_variable, falling_height, will_disappear);

  // item can be NULL if the type was NONE or RANDOM
  if (item != NULL) {

    layer = item->get_layer(); // well, some items set their own layer

    sprite_entities[layer].push_back(item);
    entity_detectors.push_back(item);
    all_entities.push_back(item);
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
 * Creates a transportable item on the map.
 * @param layer layer of the transportable item
 * @param x x position of the transportable item
 * @param y y position of the transportable item
 * @param transportable_item_type type of transportable item to create
 * @param pickable_item_type type of pickable item that appears when the
 * transportable item is lifted
 * @param savegame_variable index of the savegame boolean variable indicating
 * the possession state of the pickable item,
 * for certain kinds of pickable items only (a key, a piece of heart...)
 */
void MapEntities::add_transportable_item(MapEntity::Layer layer, int x, int y,
					 TransportableItem::ItemType transportable_item_type,
					 PickableItem::ItemType pickable_item_type, int savegame_variable) {

  TransportableItem *item = new TransportableItem(map, layer, x, y, transportable_item_type,
						  pickable_item_type, savegame_variable);
  
  sprite_entities[layer].push_back(item);
  entity_detectors.push_back(item);
  obstacle_entities[layer].push_back(item);
  all_entities.push_back(item);
}

/**
 * Removes a transportable item from the map and destroys it.
 * @param item the transportable item to remove
 */
void MapEntities::remove_transportable_item(TransportableItem *item) {
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
  entity_detectors.push_back(chest);
  obstacle_entities[layer].push_back(chest);
  all_entities.push_back(chest);
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

    // remove it from the entity detectors list if present
    // (the cast may be invalid but this is just a pointer comparison)
    entity_detectors.remove((EntityDetector*) (*it));

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

  // Link
  Link *link = zsdx->game->get_link();
  link->set_suspended(suspended);

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
  
  // update Link's position, movement and animation
  Link *link = zsdx->game->get_link();
  link->update();

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

  Link* link = zsdx->game->get_link();

  // map entities
  for (int layer = 0; layer < MapEntity::LAYER_NB; layer++) {

    // put the tiles
    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      tiles[layer][i]->display_on_map(map);
    }

    // put the animated sprites
    list<MapEntity*>::iterator i;
    for (i = sprite_entities[layer].begin();
	 i != sprite_entities[layer].end();
	 i++) {
      (*i)->display_on_map(map);
    }

    // put Link if he is in this layer
    if (link->get_layer() == layer) {
      link->display_on_map(map);
    }
  }
}
