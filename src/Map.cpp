#include "Map.h"
#include "MapLoader.h"
#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include "Sprite.h"
#include "Link.h"
#include "Tileset.h"
#include "Tile.h"
#include "TileOnMap.h"
#include "MapEntrance.h"
#include "MapExit.h"
#include "PickableItem.h"
#include "FileTools.h"

MapLoader Map::map_loader;

/**
 * Creates a map.
 * @param id id of the map, used to determine the description file
 * and the script file of the map
 */
Map::Map(MapId id):
id(id), started(false), width(0), suspended(true) {

}

/**
 * Destructor.
 */
Map::~Map(void) {

  if (is_loaded()) {
    unload();
  }
}

/**
 * Returns the id of the map.
 * @return the map id
 */
MapId Map::get_id(void) {
  return id;
}

/**
 * Returns the tileset associated to this map.
 * @return the tileset
 */
Tileset * Map::get_tileset(void) {
  return tileset;
}

/**
 * Returns whether the map is loaded.
 * @return true if the map is loaded, false otherwise
 */
bool Map::is_loaded(void) {
  return this->width != 0;
}

/**
 * Unloads the map.
 * Destroys all entities in the map to free some memory. This function
 * can be called when the player exists the map. If the player is likely to
 * come back on the map, you can keep the map loaded.
 */
void Map::unload(void) {

  SDL_FreeSurface(visible_surface);

  // delete the tiles
  for (int layer = 0; layer < LAYER_NB; layer++) {

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      delete tiles[layer][i];
    }

    tiles[layer].clear();
    delete[] obstacle_tiles[layer];

    sprite_entities[layer].clear();
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

  width = 0;
}

/**
 * Loads the map.
 * Reads the description file of the map.
 */
void Map::load() {

  this->visible_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);

  // read the map file
  map_loader.load_map(this);
}

/**
 * Creates a tile on the map.
 * This function is called for each tile when loading the map.
 * The tiles on a map are not supposed to change during the game.
 * @param tile_id id of the tile in the tileset
 * @param layer layer of the tile to create
 * @param position_in_map x position of the tile on the map
 * @param position_in_map y position of the tile on the map
 * @param width width in pixels (the pattern will be repeated on x to fit this width)
 * @param height height in pixels (the pattern will be repeated on y to fit this height
 */
void Map::add_new_tile(int tile_id, Layer layer, int x, int y, int width, int height) {

  // get the tile in the tileset
  Tile *tile = tileset->get_tile(tile_id);
  Obstacle obstacle = tile->get_obstacle();

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

  int index, i, j;

  switch (obstacle) {

    /* If the tile is entirely an obstacle or entirely no obstacle,
     * then all 8*8 squares of the tile have the same property.
     */
  case OBSTACLE_NONE:
  case OBSTACLE:
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
  case OBSTACLE_TOP_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // 8*8 square on the diagonal
      index += i;
      obstacle_tiles[layer][index++] = OBSTACLE_TOP_RIGHT;

      // right part of the row: we are in the top-right corner
      for (j = i + 1; j < tile_width8; j++) {
	obstacle_tiles[layer][index++] = OBSTACLE;
      }
    }
    break;
    
  case OBSTACLE_TOP_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // left part of the row: we are in the top-left corner
      for (j = 0; j < tile_width8 - i - 1; j++) {
	obstacle_tiles[layer][index++] = OBSTACLE;
      }

      // 8*8 square on the diagonal
      obstacle_tiles[layer][index] = OBSTACLE_TOP_LEFT;
    }
    break;
    
  case OBSTACLE_BOTTOM_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // left part of the row: we are in the bottom-left corner
      for (j = 0; j < i; j++) {
	obstacle_tiles[layer][index++] = OBSTACLE;
      }

      // 8*8 square on the diagonal
      obstacle_tiles[layer][index] = OBSTACLE_BOTTOM_LEFT;
    }
    break;
    
  case OBSTACLE_BOTTOM_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // 8*8 square on the diagonal
      index += tile_height8 - i - 1;
      obstacle_tiles[layer][index++] = OBSTACLE_BOTTOM_RIGHT;

      // right part of the row: we are in the bottom-right corner
      for (j = tile_width8 - i - 1; j < tile_width8; j++) {
	obstacle_tiles[layer][index++] = OBSTACLE;
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
void Map::add_entrance(string entrance_name, Layer layer, int link_x, int link_y, int link_direction) {
  
  MapEntrance *entrance = new MapEntrance(entrance_name, layer, link_x, link_y, link_direction);
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
 * @param transition_type type of transition between the two maps
 * @param map_id id of the next map
 * @param entrance_name name of the entrance of the next map
 */
void Map::add_exit(string exit_name, Layer layer, int x, int y, int w, int h,
		   TransitionType transition_type, MapId map_id, string entrance_name) {
  
  MapExit *exit = new MapExit(exit_name, layer, x, y, w, h, transition_type, map_id, entrance_name);
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
 * (can be a normal item, PICKABLE_ITEM_NONE or PICKABLE_ITEM_RANDOM)
 * @param falling true to make the item falling when it appears (ignored for a fairy)
 * @param will_disappear true to make the item disappear after an amout of time
 */
void Map::add_pickable_item(Layer layer, int x, int y, PickableItemType pickable_item_type,
			    bool falling, bool will_disappear) {

  PickableItem *item = PickableItem::create(this, layer, x, y, pickable_item_type, falling, will_disappear);

  // item can be NULL if the type was PICKABLE_NONE or PICKABLE_RANDOM
  if (item != NULL) {    
    sprite_entities[layer].push_back(item);
    entity_detectors.push_back(item);
    all_entities.push_back(item);
  }
}

/**
 * Removes a pickable item from the map and destroys it.
 * @param pickable_item the item to remove
 */
void Map::remove_pickable_item(PickableItem *item) {

  entities_to_remove.push_back(item);

  // erf... cannot remove it from the lists while the lists are being traversed
  /*
  sprite_entities[item->get_layer()].remove(item);
  entity_detectors.remove(item);
  */
  // keep it in all_entities to have a reference and delete it when unloading the map
}

/**
 * Removes and destroys the entities placed in the entities_to_remove list. 
 */
void Map::remove_marked_entities(void) {

  list<MapEntity*>::iterator it;

  // remove the marked entities
  for (it = entities_to_remove.begin();
       it != entities_to_remove.end();
       it++) {

    // remove it from the entity detectors list
    // (the cast may be invalid but this is just a pointer comparison)
    // okay this is awful... so:
    // TODO:
    // - the entity tells the map it wants to kill itself: map->schedule_remove_entity(this)
    // - which adds the entity to a list of entities to be removed
    // - right here, the map calls (*it)->remove_from_map()
    // - which calls for example remove_pickable_item
    // - which removes the entity from the appropriate lists, and adds the entity to a list of entities to destroy
    // - then we destroy the entities to destroy
    // or: each entity has a state: active or to be removed... well it's complicated

    entity_detectors.remove((EntityDetector*) (*it));

    // remove it from the sprite entities list
    sprite_entities[(*it)->get_layer()].remove(*it);

    // remove it from the whole list
    all_entities.remove(*it);

    // destroy it
    delete *it;
  }
  entities_to_remove.clear();
}

/**
 * Sets the current entrance of the map.
 * @param entrance_index index of the entrance you want to use
 */
void Map::set_entrance(unsigned int entrance_index) {

  if (entrance_index < 0 || entrance_index >= entrances.size()) {
    DIE("Unknown entrance '" << entrance_index << "' on map '" << id << '\'');
  }

  this->entrance_index = entrance_index;
}

/**
 * Sets the current entrance of the map.
 * @param entrance_name name of the entrance you want to use
 */
void Map::set_entrance(string entrance_name) {

  bool found = false;

  unsigned int i;
  for (i = 0; i < entrances.size() && !found; i++) { 
    found = (entrances[i]->get_name() == entrance_name);
  }

  if (found) {
    this->entrance_index = i - 1;
  }
  else {
    DIE("Unknown entrance '" << entrance_name << "' on map '" << id << '\'');
  }
}

/**
 * Returns the SDL surface where the map is displayed.
 * This surface is only the visible part of the map, so the
 * coordinates on this surface are relative to the screen,
 * not to the map.
 * @return the surface where the map is displayed
 */
SDL_Surface *Map::get_visible_surface(void) {
  return visible_surface;
}

/**
 * Returns the position of the screen, relative to the map
 * top-left corner.
 * @return the screen position
 */
SDL_Rect * Map::get_screen_position(void) {
  return &screen_position;
}

/**
 * Suspends or resumes the movement and animations of the entities.
 * This function is called when the game is being suspended
 * or resumed.
 * @param suspended true to suspend the movement and the animations,
 * false to resume them
 */
void Map::set_suspended(bool suspended) {

  this->suspended = suspended;

  // Link
  Link *link = zsdx->game_resource->get_link();
  link->set_suspended(suspended);

  // other entities
  list<MapEntity*>::iterator i;
  for (int layer = 0; layer < LAYER_NB; layer++) {

    for (i = sprite_entities[layer].begin();
	 i != sprite_entities[layer].end();
	 i++) {
      (*i)->set_suspended(suspended);
    }

    // note that we don't suspend the animated tiles
  }
}

/**
 * Updates the animation and the position of each entity, including Link.
 */
void Map::update_entities(void) {
  
  list<MapEntity*>::iterator it;
  
  // update Link's position, movement and animation
  Link *link = zsdx->game_resource->get_link();
  link->update();

  // update the animated tiles and sprites
  for (int layer = 0; layer < LAYER_NB; layer++) {

    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      tiles[layer][i]->update();
    }

    for (it = sprite_entities[layer].begin();
	 it != sprite_entities[layer].end();
	 it++) {
      (*it)->update();
    }
  }

  // remove the entities that have to be removed now
  remove_marked_entities();

  // game suspended
  bool game_suspended = zsdx->game->is_suspended();
  if (suspended != game_suspended) {
    set_suspended(game_suspended);
  }
}

/**
 * Displays the map with all its entities on the screen.
 */
void Map::display() {

  // Link
  Link* link = zsdx->game_resource->get_link();

  // screen
  screen_position.x = MIN(MAX(link->get_x() - 160, 0), width - 320);
  screen_position.y = MIN(MAX(link->get_y() - 120, 0), height - 240);  

  // background color
  SDL_FillRect(visible_surface, NULL, tileset->get_background_color());

  // map entities
  for (int layer = 0; layer < LAYER_NB; layer++) {

    // put the tiles
    for (unsigned int i = 0; i < tiles[layer].size(); i++) {
      tiles[layer][i]->display_on_map(this);
    }

    // put the visible entities
    list<MapEntity*>::iterator i;
    for (i = sprite_entities[layer].begin();
	 i != sprite_entities[layer].end();
	 i++) {
      (*i)->display_on_map(this);
    }


    // put Link if he is in this layer
    if (link->get_layer() == layer) {
      link->display_on_map(this);
    }
  }
}

/**
 * Displays a sprite on the map surface.
 * @param sprite the sprite to display
 * @param x x coordinate of the sprite's origin point in the map
 * @param y y coordinate of the sprite's origin point in the map
 */
void Map::display_sprite(Sprite *sprite, int x, int y) {
  
  // the position is given in the map coordinate system:
  // convert it to the visible surface coordinate system
  sprite->display(visible_surface, x - screen_position.x, y - screen_position.y);
}

/**
 * Starts the map. The map must be loaded.
 * Link is placed on the map and the background music starts.
 */
void Map::start(void) {

  MapEntrance *entrance = entrances[entrance_index];

  zsdx->game->play_music(music_id);

  // put Link
  Link *link = zsdx->game_resource->get_link();
  link->set_map(this, entrance->get_direction());

  int x = entrance->get_x();
  int y = entrance->get_y();

  if (x != -1) {
    link->set_x(x);
  }

  if (y != -1) {
    link->set_y(y);
  }

  SDL_SetAlpha(visible_surface, SDL_SRCALPHA, 255);

  started = true;
}

/**
 * Exits the map.
 * This function is called when Link leaves the map.
 */
void Map::leave(void) {
  started = false;
}

/**
 * Returns whether the map is started, i.e. whether it is the current
 * map and Link is on it.
 * @return true if the map is started
 */
bool Map::is_started(void) {
  return started;
}

/**
 * Tests whether a point collides with a map tile.
 * @param layer layer of the point
 * @param x x of the point in pixels
 * @param y y of the point in pixels
 * @return the obstacle property of this point
 */
Obstacle Map::pixel_collision(int layer, int x, int y) {
  Obstacle obstacle_type;
  bool on_obstacle = false;
  int x_in_tile, y_in_tile;

  // if the point is outside the map, there is no obstacle (useful when Link goes on a map exit)
  if (x < 0 || x >= width
      || y < 0 || y >= height) {
    return OBSTACLE_NONE;
  }

  // get the obstacle property of this point's 8*8 square
  obstacle_type = obstacle_tiles[layer][(y / 8) * width8 + (x / 8)];

  // test the obstacle property of this square
  switch (obstacle_type) {

  case OBSTACLE_NONE:
    // the square is not an obstacle
    on_obstacle = false;
    break;

  case OBSTACLE:
    // the square is entirely an obstacle
    on_obstacle = true;
    break;

  case OBSTACLE_TOP_RIGHT:
    // the upper right half of the square is an obstacle
    // so we have to test the position of the point
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < x_in_tile;
    break;

  case OBSTACLE_TOP_LEFT:
    // same thing
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < 8 - x_in_tile;
    break;
    
  case OBSTACLE_BOTTOM_LEFT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > x_in_tile;
    break;
    
  case OBSTACLE_BOTTOM_RIGHT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > 8 - x_in_tile;
    break;
  }

  return on_obstacle ? obstacle_type : OBSTACLE_NONE;
}

/**
 * Tests whether a rectangle collides with the map tiles.
 * @param layer layer of the rectangle in the map
 * @param collision_box the rectangle to check
 * (its dimensions should be multiples of 8)
 * @return true if the rectangle is overlapping an obstacle, false otherwise
 */
bool Map::collision_with_tiles(int layer, SDL_Rect &collision_box) {
  int x1, x2, y1, y2;
  bool collision = false;

  // we check the 4 corners of each 8*8 square in the collision box
  for (y1 = collision_box.y; y1 < collision_box.y + collision_box.h && !collision; y1 += 8) {
    y2 = y1 + 7;
    for (x1 = collision_box.x; x1 < collision_box.x + collision_box.w && !collision; x1 += 8) {
      x2 = x1 + 7;
      collision = pixel_collision(layer, x1, y1) || pixel_collision(layer, x2, y1)
	|| pixel_collision(layer, x1, y2) || pixel_collision(layer, x2, y2);
    }
  }
  
  return collision;   
}

/**
 * This function is called by an entity sensible to the obstacles
 * when this entity has just moved on the map.
 * We check whether or not the entity overlaps an entity detector.
 * @param entity the entity that has just moved (this entity should have
 * a movement sensible to the collisions)
 */
void Map::entity_just_moved(MapEntity *entity) {

  // check each detector
  list<EntityDetector*>::iterator i;
  for (i = entity_detectors.begin();
       i != entity_detectors.end();
       i++) {
    
    (*i)->check_entity_collision(entity);
  }

  // some detectors might have to be removed now
  // because of a collision
  remove_marked_entities();
}

/**
 * This function is called by a detector when an entity is overlapping it.
 * I think think function (as all "events") should call the LUA script of the map.
 * Note that all detectors don't notify the map (i.e. they don't call this function)
 * because they may have their own behavior.
 * @param detector the detector
 * @param entity the entity
 */
void Map::event_entity_on_detector(EntityDetector *detector, MapEntity *entity) {

}
