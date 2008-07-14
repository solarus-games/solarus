#include "Map.h"
#include "MapLoader.h"
#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include "Sprite.h"
#include "Tileset.h"
#include "Link.h"
#include "FileTools.h"
#include "MapEntities.h"
#include "MapEntrance.h"
#include "EntityDetector.h"

MapLoader Map::map_loader;

/**
 * Creates a map.
 * @param id id of the map, used to determine the description file
 * and the script file of the map
 */
Map::Map(MapId id):
id(id), started(false), width(0), entities(NULL), suspended(true) {

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
 * Returns the map width in pixels.
 * @return the map width
 */
int Map::get_width(void) {
  return width;
}

/**
 * Returns the map height in pixels.
 * @return the map height
 */
int Map::get_height(void) {
  return height;
}

/**
 * Returns the map width in number of 8*8 squares.
 * This is equivalent to get_width() / 8.
 * @return the map width in number of 8*8 squares
 */
int Map::get_width8(void) {
  return width8;
}

/**
 * Returns the map height in number of 8*8 squares.
 * This is equivalent to get_height() / 8.
 * @return the map height in number of 8*8 squares
 */
int Map::get_height8(void) {
  return height8;
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
  delete entities;
  width = 0;
}

/**
 * Loads the map.
 * Reads the description file of the map.
 */
void Map::load() {

  this->visible_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
  entities = new MapEntities(this);

  // read the map file
  map_loader.load_map(this);
}

/**
 * Return the entities on the map.
 */
MapEntities * Map::get_entities(void) {
  return entities;
}

/**
 * Sets the current entrance of the map.
 * @param entrance_index index of the entrance you want to use
 */
void Map::set_entrance(unsigned int entrance_index) {

  if (entrance_index < 0 || entrance_index >= entities->get_nb_entrances()) {
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
  for (i = 0; i < entities->get_nb_entrances() && !found; i++) { 
    found = (entities->get_entrance(i)->get_name() == entrance_name);
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
  entities->set_suspended(suspended);
}

/**
 * Updates the animation and the position of each entity, including Link.
 */
void Map::update(void) {
  
  // update the entities
  entities->update();

  // detect whether the game has just been suspended or resumed
  bool game_suspended = zsdx->game->is_suspended();
  if (suspended != game_suspended) {
    set_suspended(game_suspended);
  }
}

/**
 * Displays the map with all its entities on the screen.
 */
void Map::display() {

  // screen
  Link *link = zsdx->game_resource->get_link();
  screen_position.x = MIN(MAX(link->get_x() - 160, 0), width - 320);
  screen_position.y = MIN(MAX(link->get_y() - 120, 0), height - 240);  

  // background color
  SDL_FillRect(visible_surface, NULL, tileset->get_background_color());

  // display all entities (including Link)
  entities->display();
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

  MapEntrance *entrance = entities->get_entrance(entrance_index);

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
 * @return the obstacle property of this tile
 */
Obstacle Map::pixel_collision_with_tiles(Layer layer, int x, int y) {

  Obstacle obstacle_type;
  bool on_obstacle = false;
  int x_in_tile, y_in_tile;

  // if the point is outside the map, there is no obstacle (useful when Link goes on a map exit)
  if (x < 0 || x >= width
      || y < 0 || y >= height) {
    return OBSTACLE_NONE;
  }

  // get the obstacle property of the tile under that point
  obstacle_type = entities->get_obstacle_tile(layer, x, y);

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
 * Tests whether a rectangle overlaps an obstacle entity.
 * @param layer the layer
 * @param collision_box the rectangle to check
 * @return OBSTACLE if there is an obstacle entity at this point,
 * OBSTACLE_NONE otherwise.
 */
bool Map::collision_with_entities(Layer layer, SDL_Rect &collision_box) {

  list<MapEntity*> *obstacle_entities = entities->get_obstacle_entities(layer);

  bool collision = false;

  list<MapEntity*>::iterator i;
  for (i = obstacle_entities->begin();
       i != obstacle_entities->end() && !collision;
       i++) {
    
    collision = (*i)->overlaps(&collision_box);
  }

  return collision;
}

/**
 * Tests whether a rectangle collides with the map obstacles
 * (tiles and active entities).
 * @param layer layer of the rectangle in the map
 * @param collision_box the rectangle to check
 * (its dimensions should be multiples of 8)
 * @return true if the rectangle is overlapping an obstacle, false otherwise
 */
bool Map::collision_with_obstacles(Layer layer, SDL_Rect &collision_box) {
  int x1, x2, y1, y2;
  bool collision = false;

  // collisions with tiles
  // we check the 4 corners of each 8*8 square in the collision box
  for (y1 = collision_box.y; y1 < collision_box.y + collision_box.h && !collision; y1 += 8) {
    y2 = y1 + 7;
    for (x1 = collision_box.x; x1 < collision_box.x + collision_box.w && !collision; x1 += 8) {
      x2 = x1 + 7;
      collision = pixel_collision_with_tiles(layer, x1, y1)
	|| pixel_collision_with_tiles(layer, x2, y1)
	|| pixel_collision_with_tiles(layer, x1, y2)
	|| pixel_collision_with_tiles(layer, x2, y2);
    }
  }

  // collisions with entities
  if (!collision) {
    collision = collision_with_entities(layer, collision_box);
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

  list<EntityDetector*> *entity_detectors = entities->get_entity_detectors();

  // check each detector
  list<EntityDetector*>::iterator i;
  for (i = entity_detectors->begin();
       i != entity_detectors->end();
       i++) {
    
    (*i)->check_entity_collision(entity);
  }

  // some detectors might have to be removed now
  // because of a collision
  entities->remove_marked_entities();
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
