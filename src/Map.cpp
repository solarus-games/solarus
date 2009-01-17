#include "Map.h"
#include "MapLoader.h"
#include "ZSDX.h"
#include "Game.h"
#include "Sprite.h"
#include "FileTools.h"
#include "MapScript.h"
#include "entities/Tileset.h"
#include "entities/Link.h"
#include "entities/MapEntities.h"
#include "entities/DestinationPoint.h"
#include "entities/Detector.h"

MapLoader Map::map_loader;

/**
 * Creates a map.
 * @param id id of the map, used to determine the description file
 * and the script file of the map
 */
Map::Map(MapId id):
id(id), started(false), entities(NULL), suspended(false), script(NULL) {

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
 * Returns this map's script.
 * @return the script
 */
MapScript * Map::get_script(void) {
  return script;
}

/**
 * Returns the world where this map is.
 * @return 0 if this map is outside, -1 if it is inside, 1 to 20 if it is in a dungeon
 */
int Map::get_world_number(void) {
  return world;
}

/**
 * Returns whether this map belongs to a dungeon.
 * @return true if this map is in a dungeon
 */
bool Map::is_in_dungeon(void) {
  return get_world_number() > 0;
}

/**
 * Returns whether this map belongs to the outside world.
 * @return true if this map is in the oustide world
 */
bool Map::is_in_outside_world(void) {
  return get_world_number() == 0;
}

/**
 * Returns the floor where this map is.
 * The value returned can be:
 * - a floor number between -16 and 15,
 * - -100 to indicate that there is no floor,
 * - -99 to indicate an unknown floor (the '?' image will be displayed)
 * @return the floor
 */
int Map::get_floor(void) {
  return floor;
}

/**
 * Returns whether this map has a floor, i.e.
 * whether get_floor() does not return -100.
 * @return true if there is a floor
 */
bool Map::has_floor(void) {
  return get_floor() != -100;
}

/**
 * Returns the location of this map in its context.
 * - in the outside world: location of the map's top-left corner
 *   relative to the whole world map
 * - in the inside world: location of the map relative to the whole world map
 * - in a dungeon: location of the map's top-left corner relative to the whole floor
 * The width and height fields correspond to the map size.
 */
const SDL_Rect * Map::get_location(void) {
  return &location;
}

/**
 * Returns the index of the variable where the number of small keys for this
 * map is saved.
 * -1 indicates that the small keys are not enabled on this map.
 * @return the small keys savegame variable
 */
int Map::get_small_keys_variable(void) {
  return small_keys_variable;
}

/**
 * Returns whether the small keys are enabled in this map, i.e. whether
 * get_small_keys_variable() does not return -1. 
 * @return true if the small keys are enabled in this map
 */
bool Map::has_small_keys(void) {
  return get_small_keys_variable() != -1;
}

/**
 * Returns the map width in pixels.
 * @return the map width
 */
int Map::get_width(void) {
  return location.w;
}

/**
 * Returns the map height in pixels.
 * @return the map height
 */
int Map::get_height(void) {
  return location.h;
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
  return this->entities != NULL;
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
  entities = NULL;

  if (script != NULL) {
    delete script;
  }
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
 * Sets the current destination point of the map.
 * @param destination_point_index index of the destination point you want to use
 */
void Map::set_destination_point(unsigned int destination_point_index) {

  if (destination_point_index < 0 || destination_point_index >= entities->get_nb_destination_points()) {
    DIE("Unknown destination point '" << destination_point_index << "' on map '" << id << '\'');
  }

  this->destination_point_index = destination_point_index;
}

/**
 * Sets the current destination point of the map.
 * @param destination_point_name name of the destination point you want to use,
 * or "_same" to keep Link's coordinates, or "_side0", "_side1", "_side2" or "_side3 to place Link on a side of the map
 */
void Map::set_destination_point(string destination_point_name) {

  if (destination_point_name == "_same") {
    this->destination_point_index = -1;
  }
  else if (destination_point_name.substr(0,5) == "_side") {
    this->destination_point_index = -2;
    this->destination_side = destination_point_name[5] - '0';
  }
  else {

    bool found = false;

    unsigned int i;
    for (i = 0; i < entities->get_nb_destination_points() && !found; i++) { 
      found = (entities->get_destination_point(i)->get_name() == destination_point_name);
    }

    if (found) {
      this->destination_point_index = i - 1;
    }
    else {
      DIE("Unknown destination point '" << destination_point_name << "' on map '" << id << '\'');
    }
  }
}

/**
 * Returns the SDL surface where the map is displayed.
 * This surface is only the visible part of the map, so the
 * coordinates on this surface are relative to the screen,
 * not to the map.
 * @return the surface where the map is displayed
 */
SDL_Surface * Map::get_visible_surface(void) {
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

  // detect whether the game has just been suspended or resumed
  bool game_suspended = zsdx->game->is_suspended();
  if (suspended != game_suspended) {
    set_suspended(game_suspended);
  }

  // update the entities
  entities->update();

  // update the script
  script->update();
}

/**
 * Displays the map with all its entities on the screen.
 */
void Map::display() {

  // screen
  Link *link = zsdx->game->get_link();
  screen_position.x = MIN(MAX(link->get_x() - 160, 0), location.w - 320);
  screen_position.y = MIN(MAX(link->get_y() - 120, 0), location.h - 240);  

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

  zsdx->game->play_music(music_id);
  SDL_SetAlpha(visible_surface, SDL_SRCALPHA, 255);

  // put Link
  if (destination_point_index >= 0) {
    DestinationPoint *destination_point = entities->get_destination_point(destination_point_index);
    destination_point->place_hero();
  }
  else if (destination_point_index == -1) {

    // Link's coordinates are the same as on previous map
    Link *link = zsdx->game->get_link();
    link->set_map(this);
  }
  else {

    // only one coordinate is changed
    Link *link = zsdx->game->get_link();
    link->set_map(this);

    switch (destination_side) {

    case 0: // right side
      link->set_x(get_width() - 8);
      break;

    case 1: // top side
      link->set_y(21);
      break;

    case 2: // left side
      link->set_x(8);
      break;

    case 3: // bottom side
      link->set_y(get_height() - 3);
      break;

    default:
      DIE("Invalid destination side: " << destination_side);
    }
  }

  started = true;

  script->event_map_started();
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
MapEntity::Obstacle Map::pixel_collision_with_tiles(MapEntity::Layer layer, int x, int y) {

  MapEntity::Obstacle obstacle_type;
  bool on_obstacle = false;
  int x_in_tile, y_in_tile;

  // if the point is outside the map, there is no obstacle (useful when Link walks on a teletransporter)
  if (x < 0 || x >= get_width()
      || y < 0 || y >= get_height()) {
    return MapEntity::OBSTACLE_NONE;
  }

  // get the obstacle property of the tile under that point
  obstacle_type = entities->get_obstacle_tile(layer, x, y);

  // test the obstacle property of this square
  switch (obstacle_type) {

  case MapEntity::OBSTACLE_NONE:
    // the square is not an obstacle
    on_obstacle = false;
    break;

  case MapEntity::OBSTACLE:
    // the square is entirely an obstacle
    on_obstacle = true;
    break;

  case MapEntity::OBSTACLE_TOP_RIGHT:
    // the upper right half of the square is an obstacle
    // so we have to test the position of the point
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < x_in_tile;
    break;

  case MapEntity::OBSTACLE_TOP_LEFT:
    // same thing
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < 8 - x_in_tile;
    break;

  case MapEntity::OBSTACLE_BOTTOM_LEFT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > x_in_tile;
    break;

  case MapEntity::OBSTACLE_BOTTOM_RIGHT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > 8 - x_in_tile;
    break;
  }

  return on_obstacle ? obstacle_type : MapEntity::OBSTACLE_NONE;
}

/**
 * Tests whether a rectangle overlaps an obstacle entity.
 * @param layer the layer
 * @param collision_box the rectangle to check
 * @return OBSTACLE if there is an obstacle entity at this point,
 * OBSTACLE_NONE otherwise.
 */
bool Map::collision_with_entities(MapEntity::Layer layer, SDL_Rect &collision_box) {

  std::list<MapEntity*> *obstacle_entities = entities->get_obstacle_entities(layer);

  bool collision = false;

  std::list<MapEntity*>::iterator i;
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
bool Map::collision_with_obstacles(MapEntity::Layer layer, SDL_Rect &collision_box) {
  int x1, y1;
  bool collision = false;

  // collisions with tiles
  // we check the 4 corners of each 8*8 square in the collision box
  for (y1 = collision_box.y; y1 < collision_box.y + collision_box.h && !collision; y1++) {
    for (x1 = collision_box.x; x1 < collision_box.x + collision_box.w && !collision; x1++) {
      collision = pixel_collision_with_tiles(layer, x1, y1);
    }
  }

  // collisions with entities
  if (!collision) {
    collision = collision_with_entities(layer, collision_box);
  }
  
  return collision;   
}

/**
 * Tests whether a point collides with the map obstacles
 * (tiles and active entities).
 * @param layer layer of the rectangle in the map
 * @param x x coordinate of the point to check
 * @param y y coordinate of the point to check
 * @return true if the point is overlapping an obstacle, false otherwise
 */
bool Map::collision_with_obstacles(MapEntity::Layer layer, int x, int y) {

  bool collision;

  // test the tiles
  collision = pixel_collision_with_tiles(layer, x, y);

  // test the entities
  if (!collision) {
    SDL_Rect collision_box = {x, y, 1, 1};
    collision = collision_with_entities(layer, collision_box);
  }

  return collision;   
}

/**
 * This function is called by an entity sensitive to the entity detectors
 * when this entity has just moved on the map, or when a detector
 * wants to check this entity.
 * We check whether or not the entity overlaps an entity detector.
 * @param entity the entity that has just moved (this entity should have
 * a movement sensible to the collisions)
 */
void Map::check_collision_with_detectors(MapEntity *entity) {

  std::list<Detector*> *detectors = entities->get_detectors();

  // check each detector
  std::list<Detector*>::iterator i;
  for (i = detectors->begin();
       i != detectors->end();
       i++) {

    if (!(*i)->is_being_removed()) {
      (*i)->check_collision(entity);
    }
  }
}

/**
 * This function is called by an entity
 * when the frame of one of its sprites has just changed.
 * We check whether or not the sprite overlaps the detector.
 * @param entity the sprite that has just changed
 * @param sprite the sprite that has just changed
 */
void Map::check_collision_with_detectors(MapEntity *entity, Sprite *sprite) {

  std::list<Detector*> *detectors = entities->get_detectors();

  // check each detector
  std::list<Detector*>::iterator i;
  for (i = detectors->begin();
       i != detectors->end();
       i++) {

    if (!(*i)->is_being_removed()) {
      (*i)->check_collision(entity, sprite);
    }
  }
}
