/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Map.h"
#include "MapLoader.h"
#include "Game.h"
#include "DialogBox.h"
#include "Sprite.h"
#include "Camera.h"
#include "lua/MapScript.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Music.h"
#include "entities/Ground.h"
#include "entities/Tileset.h"
#include "entities/TilePattern.h"
#include "entities/MapEntities.h"
#include "entities/DestinationPoint.h"
#include "entities/Detector.h"
#include "entities/Hero.h"

MapLoader Map::map_loader;

/**
 * @brief Creates a map.
 * @param id id of the map, used to determine the description file
 * and the script file of the map
 */
Map::Map(MapId id):
  game(NULL), id(id), started(false), destination_point_name(""),
  entities(NULL), suspended(false) {

}

/**
 * @brief Destructor.
 */
Map::~Map() {

  if (is_loaded()) {
    unload();
  }
}

/**
 * @brief Returns the id of the map.
 * @return the map id
 */
MapId Map::get_id() {
  return id;
}

/**
 * @brief Returns the tileset associated to this map.
 * @return the tileset
 */
Tileset& Map::get_tileset() {
  return *tileset;
}

/**
 * @brief Returns the script of this map.
 * @return the map script
 */
MapScript& Map::get_script() {
  return *script;
}

/**
 * @brief Returns the world where this map is.
 * @return 0 if this map is outside, -1 if it is inside, 1 to 20 if it is in a dungeon
 */
int Map::get_world_number() {
  return world;
}

/**
 * @brief Returns whether this map belongs to a dungeon.
 * @return true if this map is in a dungeon
 */
bool Map::is_in_dungeon() {
  return get_world_number() > 0;
}

/**
 * @brief Returns whether this map belongs to the outside world.
 * @return true if this map is in the oustide world
 */
bool Map::is_in_outside_world() {
  return get_world_number() == 0;
}

/**
 * @brief Returns the floor where this map is.
 *
 * The value returned can be:
 * - a floor number between -16 and 15,
 * - -100 to indicate that there is no floor,
 * - -99 to indicate an unknown floor (the '?' image will be displayed).
 *
 * @return the floor
 */
int Map::get_floor() {
  return floor;
}

/**
 * @brief Returns whether this map has a floor.
 *
 * This function returns true if get_floor() is not -100.
 *
 * @return true if there is a floor
 */
bool Map::has_floor() {
  return get_floor() != -100;
}

/**
 * @brief Returns the location of this map in its context.
 *
 * The location returned is:
 * - in the outside world: location of the map's top-left corner
 *   relative to the whole world map
 * - in the inside world: location of the map relative to the whole world map
 * - in a dungeon: location of the map's top-left corner relative to the whole floor
 * The width and height fields correspond to the map size.
 *
 * @return the location of this map in its context.
 */
const Rectangle& Map::get_location() {
  return location;
}

/**
 * @brief Returns the index of the variable where the number of small keys for this
 * map is saved.
 *
 * -1 indicates that the small keys are not enabled on this map.
 *
 * @return the small keys savegame variable
 */
int Map::get_small_keys_variable() {
  return small_keys_variable;
}

/**
 * @brief Returns whether the small keys are enabled in this map.
 *
 * This function returns true if get_small_keys_variable() is not -1. 
 *
 * @return true if the small keys are enabled in this map
 */
bool Map::has_small_keys() {
  return get_small_keys_variable() != -1;
}

/**
 * @brief Returns the map width in pixels.
 * @return the map width
 */
int Map::get_width() {
  return location.get_width();
}

/**
 * @brief Returns the map height in pixels.
 * @return the map height
 */
int Map::get_height() {
  return location.get_height();
}

/**
 * @brief Returns the map width in number of 8*8 squares.
 *
 * This is equivalent to get_width() / 8.
 *
 * @return the map width in number of 8*8 squares
 */
int Map::get_width8() {
  return width8;
}

/**
 * @brief Returns the map height in number of 8*8 squares.
 *
 * This is equivalent to get_height() / 8.
 *
 * @return the map height in number of 8*8 squares
 */
int Map::get_height8() {
  return height8;
}

/**
 * @brief Returns whether the map is loaded.
 * @return true if the map is loaded, false otherwise
 */
bool Map::is_loaded() {
  return this->entities != NULL;
}

/**
 * @brief Unloads the map.
 *
 * Destroys all entities in the map to free some memory. This function
 * can be called when the player exists the map. If the player is likely to
 * come back on the map, you can keep the map loaded.
 */
void Map::unload() {

  delete tileset;
  delete visible_surface;
  delete entities;
  entities = NULL;

  delete script;
  delete camera;
}

/**
 * @brief Loads the map into a game.
 *
 * Reads the description file of the map.
 *
 * @param game the game
 */
void Map::load(Game &game) {

  this->visible_surface = new Surface(320, 240);
  entities = new MapEntities(game, *this);

  // read the map file
  map_loader.load_map(game, *this);
}

/**
 * @brief Returns the game that loaded this map.
 *
 * This function should not be called before the map is loaded into a game.
 *
 * @return the game
 */
Game& Map::get_game() {
  return *game;
}

/**
 * @brief Returns the entities of the map.
 *
 * This function should not be called before the map is loaded into a game.
 *
 * @return the entities of the map
 */
MapEntities& Map::get_entities() {
  return *entities;
}

/**
 * @brief Sets the current destination point of the map.
 * @param destination_point_name name of the destination point you want to use,
 * or "_same" to keep the hero's coordinates, or "_side0", "_side1", "_side2"
 * or "_side3" to place the hero on a side of the map
 */
void Map::set_destination_point(const std::string &destination_point_name) {
  this->destination_point_name = destination_point_name;
}

/**
 * @brief Returns the destination point index specified by the last call to set_destination_point().
 * @return the name of the destination point previously set
 */
const std::string& Map::get_destination_point_name() {
  return destination_point_name;
}

/**
 * @brief When the destination point is a side of the map,
 * returns this side.
 * @return the destination side (0 to 3), or -1 if the destination point is not a side
 */
int Map::get_destination_side() {

  if (destination_point_name.substr(0,5) == "_side") {
    int destination_side = destination_point_name[5] - '0';
    return destination_side;
  }
  return -1;
}

/**
 * @brief Returns the surface where the map is displayed.
 *
 * This surface is only the visible part of the map, so the
 * coordinates on this surface are relative to the screen,
 * not to the map.
 *
 * @return the surface where the map is displayed
 */
Surface* Map::get_visible_surface() {
  return visible_surface;
}

/**
 * @brief Returns the position of the visible area, relative to the map
 * top-left corner.
 * @return the position of the visible area
 */
const Rectangle& Map::get_camera_position() {
  return camera->get_position();
}

/**
 * @brief Makes the camera move towards a point.
 * @param x x coordinate of the target point
 * @param y y coordinate of the target point
 * @param speed speed of the movement
 */
void Map::move_camera(int x, int y, int speed) {
  camera->set_speed(speed);
  camera->move(x, y);
}

/**
 * @brief Makes the camera move back to the hero.
 */
void Map::restore_camera() {
  camera->restore();
}

/**
 * @brief Returns whether the camera is fixed on the hero.
 * @return true if the camera is following the hero
 */
bool Map::is_camera_fixed_on_hero() {
  return camera->is_fixed_on_hero();
}

/**
 * @brief Sets a subarea of the map where the next drawings will be restricted to.
 *
 * A zero-sized rectangle means that drawings are not restricted to a subarea of the map.
 *
 * @param clipping_rectangle a subarea of the map to restrict the display to
 */
void Map::set_clipping_rectangle(const Rectangle &clipping_rectangle) {

  this->clipping_rectangle = clipping_rectangle;

  const Rectangle &camera_position = camera->get_position();
  Rectangle surface_clipping_rectangle(clipping_rectangle);
  surface_clipping_rectangle.add_xy(-camera_position.get_x(), -camera_position.get_y());
  visible_surface->set_clipping_rectangle(surface_clipping_rectangle);
}

/**
 * @brief Suspends or resumes the movement and animations of the entities.
 *
 * This function is called when the game is being suspended
 * or resumed.
 *
 * @param suspended true to suspend the movement and the animations,
 * false to resume them
 */
void Map::set_suspended(bool suspended) {

  this->suspended = suspended;

  entities->set_suspended(suspended);
  script->set_suspended(suspended);
}

/**
 * @brief Updates the animation and the position of each map elements, including the hero.
 */
void Map::update() {

  // detect whether the game has just been suspended or resumed
  check_suspended();

  // update the elements
  TilePattern::update();
  entities->update();
  script->update();
  camera->update(); /* update the camera after the entities since this might 
		       be the last update() call for this map */
  set_clipping_rectangle(clipping_rectangle);
}

/**
 * @brief Checks whether the game has just been suspended or resumed
 * and notifies the map elements when this is the case.
 *
 * This function is called at each cycle while this map is active,
 * but you may want to call it more often in specific situations if you cannot wait.
 */
void Map::check_suspended() {

  bool game_suspended = game->is_suspended();
  if (suspended != game_suspended) {
    set_suspended(game_suspended);
  }
}

/**
 * @brief Displays the map with all its entities on the screen.
 */
void Map::display() {

  // background
  visible_surface->fill_with_color(tileset->get_background_color());

  // display all entities (including the hero)
  entities->display();

  // foreground
/* TODO (in a separate function)
  if (light == 0) {

    static Surface* lamp_surface = new Surface("entities/dark0.png");
    const Rectangle& hero_position = get_entities().get_hero().get_center_point();
    const Rectangle& camera_position = camera->get_position();
    int x = 320 - hero_position.get_x() + camera_position.get_x();
    int y = 240 - hero_position.get_y() + camera_position.get_y();
    Rectangle src(x, y, 320, 240);
    lamp_surface->blit(src, visible_surface);

  }
  // TODO intermediate light levels
   */
}

/**
 * @brief Displays a sprite on the map surface.
 * @param sprite the sprite to display
 * @param xy coordinates of the sprite's origin point in the map
 * (the size of the rectangle is ignored)
 */
void Map::display_sprite(Sprite &sprite, const Rectangle &xy) {

  display_sprite(sprite, xy.get_x(), xy.get_y());
}

/**
 * @brief Displays a sprite on the map surface.
 * @param sprite the sprite to display
 * @param x x coordinate of the sprite's origin point in the map
 * @param y y coordinate of the sprite's origin point in the map
 */
void Map::display_sprite(Sprite &sprite, int x, int y) {

  // the position is given in the map coordinate system:
  // convert it to the visible surface coordinate system
  const Rectangle &camera_position = get_camera_position();
  sprite.display(visible_surface, x - camera_position.get_x(), y - camera_position.get_y());
}

/**
 * @brief Starts the map.
 *
 * The map must be loaded.
 * The background music starts and the map script is initialized.
 */
void Map::start() {

  this->started = true;
  this->visible_surface->set_opacity(255);
  Music::play(music_id);
  this->entities->notify_map_started();
  this->script->start(destination_point_name);
  get_game().get_equipment().set_map(*this);
}

/**
 * @brief Exits the map.
 *
 * This function is called when the hero leaves the map.
 */
void Map::leave() {
  started = false;
}

/**
 * @brief Returns whether the map is started.
 *
 * This function returns whether this is the current map and the hero is on it.
 *
 * @return true if the map is started
 */
bool Map::is_started() {
  return started;
}

/**
 * @brief This function is called when the map is started and 
 * the opening transition is finished.
 */
void Map::notify_opening_transition_finished() {

  visible_surface->set_opacity(255); // because the transition effect may have changed the opacity
  check_suspended();
  game->get_hero().notify_opening_transition_finished();
  get_script().event_map_opening_transition_finished(destination_point_name);
}

/**
 * @brief Tests whether a rectangle has overlaps the outside part of the map area.
 * @param collision_box the rectangle to check
 * @return true if a point of the rectangle is outside the map area
 */
bool Map::test_collision_with_border(const Rectangle &collision_box) {

  return collision_box.get_x() < 0 || collision_box.get_x() + collision_box.get_width() >= get_width()
    || collision_box.get_y() < 0 || collision_box.get_y() + collision_box.get_height() >= get_height();
}

/**
 * @brief Tests whether a point collides with a map tile.
 *
 * This method also returns true if the point is outside the map.
 *
 * @param layer layer of the point
 * @param x x of the point in pixels
 * @param y y of the point in pixels
 * @param entity_to_check the entity to check (used to decide what tiles are considered as an obstacle)
 * @return true if this point is on an obstacle
 */
bool Map::test_collision_with_tiles(Layer layer, int x, int y, MapEntity &entity_to_check) {

  Obstacle obstacle_type;
  bool on_obstacle = false;
  int x_in_tile, y_in_tile;

  // if the point is outside the map, there is only obstacles
  if (test_collision_with_border(x, y)) {
    return true;
  }

  // get the obstacle property of the tile under that point
  obstacle_type = entities->get_obstacle_tile(layer, x, y);

  // test the obstacle property of this square
  switch (obstacle_type) {

  case OBSTACLE_NONE:
  case OBSTACLE_EMPTY:
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
    on_obstacle = y_in_tile <= x_in_tile;
    break;

  case OBSTACLE_TOP_LEFT:
    // same thing
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile <= 7 - x_in_tile;
    break;

  case OBSTACLE_BOTTOM_LEFT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile >= x_in_tile;
    break;

  case OBSTACLE_BOTTOM_RIGHT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile >= 7 - x_in_tile;
    break;

  case OBSTACLE_SHALLOW_WATER:
  case OBSTACLE_DEEP_WATER:
    on_obstacle = entity_to_check.is_water_obstacle();
    break;

  case OBSTACLE_HOLE:
    on_obstacle = entity_to_check.is_hole_obstacle();
    break;

  case OBSTACLE_LADDER:
    on_obstacle = entity_to_check.is_ladder_obstacle();
    break;
  }

  return on_obstacle;
}

/**
 * @brief Tests whether a rectangle overlaps an obstacle entity.
 * @param layer the layer
 * @param collision_box the rectangle to check
 * @param entity_to_check the entity to check (used to decide what is considered as an obstacle)
 * @return true if there is an obstacle entity at this point
 */
bool Map::test_collision_with_entities(Layer layer, const Rectangle &collision_box, MapEntity &entity_to_check) {

  std::list<MapEntity*> &obstacle_entities = entities->get_obstacle_entities(layer);

  bool collision = false;

  std::list<MapEntity*>::iterator i;
  for (i = obstacle_entities.begin();
       i != obstacle_entities.end() && !collision;
       i++) {

    MapEntity *entity = *i;
    collision = entity != &entity_to_check &&
      entity->is_obstacle_for(entity_to_check) && entity->overlaps(collision_box);
  }

  return collision;
}

/**
 * @brief Tests whether a rectangle collides with the map obstacles.
 * @param layer layer of the rectangle in the map
 * @param collision_box the rectangle to check (its dimensions should be multiples of 8)
 * @param entity_to_check the entity to check (used to decide what is considered as an obstacle)
 * @return true if the rectangle is overlapping an obstacle, false otherwise
 */
bool Map::test_collision_with_obstacles(Layer layer, const Rectangle &collision_box, MapEntity &entity_to_check) {

  int x, y, x1, x2, y1, y2;
  bool collision = false;

  // collisions with tiles: we just check the borders of the collision box
  y1 = collision_box.get_y();
  y2 = y1 + collision_box.get_height() - 1;
  x1 = collision_box.get_x();
  x2 = x1 + collision_box.get_width() - 1;

  for (x = x1; x <= x2 && !collision; x++) {
    collision = test_collision_with_tiles(layer, x, y1, entity_to_check) ||
      test_collision_with_tiles(layer, x, y2, entity_to_check);
  }

  for (y = y1; y <= y2 && !collision; y++) {
    collision = test_collision_with_tiles(layer, x1, y, entity_to_check) ||
      test_collision_with_tiles(layer, x2, y, entity_to_check);
  }

/*
  // slow version: check every pixel of the collision_box rectangle
  for (y1 = collision_box.y; y1 < collision_box.y + collision_box.h && !collision; y1++) {
    for (x1 = collision_box.x; x1 < collision_box.x + collision_box.w && !collision; x1++) {
      collision = test_collision_with_tiles(layer, x1, y1, entity_to_check);
    }
  }
*/

  // collisions with entities
  if (!collision) {
    collision = test_collision_with_entities(layer, collision_box, entity_to_check);
  }

  return collision;
}

/**
 * @brief Tests whether a point collides with the map obstacles.
 * @param layer layer of point to check
 * @param x x coordinate of the point to check
 * @param y y coordinate of the point to check
 * @param entity_to_check the entity to check (used to decide what is considered as an obstacle)
 * @return true if the point is overlapping an obstacle, false otherwise
 */
bool Map::test_collision_with_obstacles(Layer layer, int x, int y, MapEntity &entity_to_check) {

  bool collision;

  // test the tiles
  collision = test_collision_with_tiles(layer, x, y, entity_to_check);

  // test the entities
  if (!collision) {
    Rectangle collision_box(x, y, 1, 1);
    collision = test_collision_with_entities(layer, collision_box, entity_to_check);
  }

  return collision;   
}

/**
 * @brief Returns the kind of ground that is under the specified point.
 *
 * Only the tiles are considered here (not the dynamic entities).
 *
 * @param layer layer of point to check
 * @param x x coordinate of the point to check
 * @param y y coordinate of the point to check
 * @return the ground at this place
 */
Ground Map::get_tile_ground(Layer layer, int x, int y) {

  Obstacle obstacle = entities->get_obstacle_tile(layer, x, y);
  Ground ground;

  switch (obstacle) {
    case OBSTACLE_SHALLOW_WATER:
      ground = GROUND_SHALLOW_WATER;
      break;

    case OBSTACLE_DEEP_WATER:
      ground = GROUND_DEEP_WATER;
      break;

    case OBSTACLE_HOLE:
      ground = GROUND_HOLE;
      break;

    case OBSTACLE_LADDER:
      ground = GROUND_LADDER;
      break;

    default:
      ground = GROUND_NORMAL;
      break;
  }

  return ground;
}

/**
 * @brief Returns the kind of ground that is under the specified point.
 *
 * Only the tiles are considered here (not the dynamic entities).
 *
 * @param layer layer of point to check
 * @param coordinates coordinates of the point to check
 * @return the ground at this place
 */
Ground Map::get_tile_ground(Layer layer, const Rectangle &coordinates) {
  return get_tile_ground(layer, coordinates.get_x(), coordinates.get_y());
}

/**
 * @brief Checks the collisions between an entity and the detectors of the map.
 *
 * This function is called by an entity sensitive to the entity detectors
 * when this entity has just moved on the map, or when a detector
 * wants to check this entity.
 * We check whether or not the entity overlaps an entity detector.
 * If the map is suspended, this function does nothing.
 *
 * @param entity the entity that has just moved (this entity should have
 * a movement sensible to the collisions)
 */
void Map::check_collision_with_detectors(MapEntity &entity) {

  if (suspended) {
    return;
  }

  std::list<Detector*> &detectors = entities->get_detectors();

  // check each detector
  std::list<Detector*>::iterator i;

  for (i = detectors.begin();
       i != detectors.end();
       i++) {

    if (!(*i)->is_being_removed()) {
      (*i)->check_collision(entity);
    }
  }
}

/**
 * @brief Checks the pixel-perfect collisions between an entity and the detectors of the map.
 *
 * This function is called by an entity
 * when the frame of one of its sprites has just changed.
 * We check whether or not the sprite overlaps the detector.
 * If the map is suspended, this function does nothing.
 *
 * @param entity the sprite that has just changed
 * @param sprite the sprite that has just changed
 */
void Map::check_collision_with_detectors(MapEntity &entity, Sprite &sprite) {

  if (suspended) {
    return;
  }

  std::list<Detector*> &detectors = entities->get_detectors();
  // check each detector
  std::list<Detector*>::iterator i;
  for (i = detectors.begin();
       i != detectors.end();
       i++) {

    if (!(*i)->is_being_removed()) {
      (*i)->check_collision(entity, sprite);
    }
  }
}

