/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "lua/LuaContext.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Music.h"
#include "lowlevel/Debug.h"
#include "entities/Ground.h"
#include "entities/Tileset.h"
#include "entities/TilePattern.h"
#include "entities/MapEntities.h"
#include "entities/Destination.h"
#include "entities/Detector.h"
#include "entities/Hero.h"

MapLoader Map::map_loader;

/**
 * @brief Creates a map.
 * @param id id of the map, used to determine the description file
 * and the script file of the map
 */
Map::Map(const std::string& id):
  game(NULL),
  id(id),
  tileset(NULL),
  floor(NO_FLOOR),
  loaded(false),
  started(false),
  destination_name(""),
  entities(NULL),
  suspended(false),
  light(1) {

}

/**
 * @brief Destructor.
 */
Map::~Map() {

  Debug::check_assertion(!is_started(),
      "Deleting a map that is still running. Call Map::leave() before.");

  if (is_loaded()) {
    unload();
  }
}

/**
 * @brief Returns the id of the map.
 * @return the map id
 */
const std::string& Map::get_id() {
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
 * @brief Returns the id of the tileset associated to this map.
 * @return the id of the tileset
 */
const std::string& Map::get_tileset_id() {
  // note that if set_tileset() has been called, tileset_id != tileset->get_id()
  return tileset_id;
}

/**
 * @brief Changes the tileset used to draw this map.
 *
 * The new tileset must be compatible with the previous one,
 * i.e. every tile of the previous tileset must exist in the new one
 * and have the same properties.
 * This function keeps the tiles of the previous tileset and loads the
 * image of the new tileset.
 *
 * @param tileset_id Id of the new tileset.
 */
void Map::set_tileset(const std::string& tileset_id) {

  Tileset new_tileset(tileset_id);
  new_tileset.load();
  tileset->set_images(new_tileset);
  get_entities().notify_tileset_changed();
  this->tileset_id = tileset_id;
}

/**
 * @brief Returns the world where this map is.
 * @return The world name.
 */
const std::string& Map::get_world() {
  return world;
}

/**
 * @brief Returns the world where this map is.
 * @param world The world name.
 */
void Map::set_world(const std::string& world) {
  this->world = world;
}

/**
 * @brief Returns whether this map belongs to a dungeon.
 * @return true if this map is in a dungeon
 */
bool Map::is_in_dungeon() {
  return get_world().substr(0, 8) == "dungeon_";
}

/**
 * @brief Returns whether this map belongs to the outside world.
 * @return true if this map is in the oustide world
 */
bool Map::is_in_outside_world() {
  return get_world() == "outside_world";
}

/**
 * @brief Returns whether this map has a floor.
 *
 * This function returns true if the floor is not nil.
 *
 * @return true if there is a floor.
 */
bool Map::has_floor() {
  return get_floor() != NO_FLOOR;
}

/**
 * @brief Returns the floor where this map is.
 * @return The floor or FLOOR_NIL.
 */
int Map::get_floor() {
  return floor;
}

/**
 * @brief Sets the floor where this map is.
 * @param floor The floor or FLOOR_NIL.
 */
void Map::set_floor(int floor) {
  this->floor = floor;
}

/**
 * @brief Returns the location of this map in its context.
 *
 * The location returned is the location of the map's top-left corner
 * relative to its context (its floor or its world).
 * The width and height fields correspond to the map size.
 *
 * @return The location of this map in its context.
 */
const Rectangle& Map::get_location() {
  return location;
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
  return loaded;
}

/**
 * @brief Unloads the map.
 *
 * Destroys all entities in the map to free some memory. This function
 * can be called when the player exists the map. If the player is likely to
 * come back on the map, you can keep the map loaded.
 */
void Map::unload() {

  if (is_loaded()) {
    delete tileset;
    tileset = NULL;
    visible_surface->decrement_refcount();
    if (visible_surface->get_refcount() == 0) {
      delete visible_surface;
    }
    visible_surface = NULL;
    delete entities;
    entities = NULL;
    delete camera;
    camera = NULL;

    for (int i = 0; i < 4; i++) {
      delete dark_surfaces[i];
      dark_surfaces[i] = NULL;
    }
    loaded = false;
  }
}

/**
 * @brief Loads the map into a game.
 *
 * Reads the description file of the map.
 *
 * @param game the game
 */
void Map::load(Game &game) {

  this->visible_surface = new Surface(SOLARUS_SCREEN_WIDTH, SOLARUS_SCREEN_HEIGHT);
  this->visible_surface->increment_refcount();
  entities = new MapEntities(game, *this);

  // read the map file
  map_loader.load_map(game, *this);

  // initialize the light
  dark_surfaces[0] = new Surface("entities/dark0.png");
  dark_surfaces[1] = new Surface("entities/dark1.png");
  dark_surfaces[2] = new Surface("entities/dark2.png");
  dark_surfaces[3] = new Surface("entities/dark3.png");

  loaded = true;
}

/**
 * @brief Returns the shared Lua context.
 *
 * This function should not be called before the map is loaded into a game.
 *
 * @return The Lua context where all scripts are run.
 */
LuaContext& Map::get_lua_context() {
  return game->get_lua_context();
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
 * @param destination_name name of the destination point you want to use,
 * or "_same" to keep the hero's coordinates, or "_side0", "_side1", "_side2"
 * or "_side3" to place the hero on a side of the map
 */
void Map::set_destination(const std::string &destination_name) {
  this->destination_name = destination_name;
}

/**
 * @brief Returns the destination point index specified by the last call to set_destination().
 * @return the name of the destination point previously set
 */
const std::string& Map::get_destination_name() {
  return destination_name;
}

/**
 * @brief Returns the destination point specified by the last call to
 * set_destination().
 *
 * Returns NULL if the destination point was set to a special value ("_same",
 * "_side0", "_side1", "_side2" or "_side3")
 *
 * @return The destination point previously set, or NULL.
 */
Destination* Map::get_destination() {

  if (destination_name == "_same"
      || destination_name.substr(0,5) == "_side") {
    return NULL;
  }
  MapEntity* entity = get_entities().get_entity(destination_name);
  Debug::check_assertion(entity->get_type() == DESTINATION,
      "This entity is not a destination");
  return static_cast<Destination*>(entity);
}

/**
 * @brief When the destination point is a side of the map,
 * returns this side.
 * @return the destination side (0 to 3), or -1 if the destination point is not a side
 */
int Map::get_destination_side() {

  if (destination_name.substr(0,5) == "_side") {
    int destination_side = destination_name[5] - '0';
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
Surface& Map::get_visible_surface() {
  return *visible_surface;
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
 * @brief Returns the current level of light of the map.
 * @return the light level (0: no light, a positive value: full light)
 */
int Map::get_light() {
  return light;
}

/**
 * @brief Sets the level of light of the map.
 * @param light the new light level (0: no light, a positive value: full light)
 */
void Map::set_light(int light) {
  this->light = light;
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
  get_lua_context().notify_map_suspended(*this, suspended);
}

/**
 * @brief This function is called when a low-level input event occurs on this map.
 * @param event the event to handle
 * @return \c true if the event was handled and should stop being propagated.
 */
bool Map::notify_input(InputEvent& event) {

  bool handled = get_lua_context().map_on_input(*this, event);
  return handled;
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
  get_lua_context().map_on_update(*this);
  camera->update();  // update the camera after the entities since this might
                     // be the last update() call for this map */
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
 * @brief Draws the map with all its entities on the screen.
 */
void Map::draw() {

  if (is_loaded()) {
    // background
    draw_background();

    // draw all entities (including the hero)
    entities->draw();

    // foreground
    draw_foreground();

    // Lua
    get_lua_context().map_on_draw(*this, *visible_surface);
  }
}

/**
 * @brief Draws the background of the map.
 */
void Map::draw_background() {

  visible_surface->fill_with_color(tileset->get_background_color());
}

/**
 * @brief Draws the foreground of the map.
 */
void Map::draw_foreground() {

  if (light == 0) {
    // no light

    int hero_direction = get_entities().get_hero().get_animation_direction();
    const Rectangle& hero_position = get_entities().get_hero().get_center_point();
    const Rectangle& camera_position = camera->get_position();
    int x = 320 - hero_position.get_x() + camera_position.get_x();
    int y = 240 - hero_position.get_y() + camera_position.get_y();
    Rectangle src_position(x, y, SOLARUS_SCREEN_WIDTH, SOLARUS_SCREEN_HEIGHT);
    Surface& dark_surface = *dark_surfaces[hero_direction];
    dark_surface.draw_region(src_position, *visible_surface);

    // dark_surface may be too small if the screen size is greater
    // than 320*240: add black bars.
    if (x < 0) {
      visible_surface->fill_with_color(Color::get_black(),
          Rectangle(0, 0, -x, SOLARUS_SCREEN_HEIGHT));
    }
    if (y < 0) {
      visible_surface->fill_with_color(Color::get_black(),
          Rectangle(0, 0, SOLARUS_SCREEN_WIDTH, -y));
    }
    if (x > dark_surface.get_width() - SOLARUS_SCREEN_WIDTH) {
      visible_surface->fill_with_color(Color::get_black(),
          Rectangle(dark_surface.get_width() - x, 0,
              x - dark_surface.get_width() + SOLARUS_SCREEN_WIDTH, SOLARUS_SCREEN_HEIGHT));
    }
    if (y > dark_surface.get_height() - SOLARUS_SCREEN_HEIGHT) {
      visible_surface->fill_with_color(Color::get_black(),
          Rectangle(0, dark_surface.get_height() - y,
              SOLARUS_SCREEN_WIDTH, y - dark_surface.get_height() + SOLARUS_SCREEN_HEIGHT));
    }
  }
  // TODO intermediate light levels

  // If the map is too small for the screen, add black bars outside the map.
  const int map_width = get_width();
  if (map_width < SOLARUS_SCREEN_WIDTH) {
    int bar_width = (SOLARUS_SCREEN_WIDTH - map_width) / 2;
    Rectangle dst_position(0, 0, bar_width, SOLARUS_SCREEN_HEIGHT);
    visible_surface->fill_with_color(Color::get_black(), dst_position);
    dst_position.set_x(bar_width + map_width);
    visible_surface->fill_with_color(Color::get_black(), dst_position);
  }

  const int map_height = get_height();
  if (map_height < SOLARUS_SCREEN_HEIGHT) {
    int bar_height = (SOLARUS_SCREEN_HEIGHT - map_height) / 2;
    Rectangle dst_position(0, 0, SOLARUS_SCREEN_WIDTH, bar_height);
    visible_surface->fill_with_color(Color::get_black(), dst_position);
    dst_position.set_y(bar_height + map_height);
    visible_surface->fill_with_color(Color::get_black(), dst_position);
  }
}

/**
 * @brief Draws a sprite on the map surface.
 * @param sprite the sprite to draw
 * @param xy coordinates of the sprite's origin point in the map
 * (the size of the rectangle is ignored)
 */
void Map::draw_sprite(Sprite &sprite, const Rectangle &xy) {

  draw_sprite(sprite, xy.get_x(), xy.get_y());
}

/**
 * @brief Draws a sprite on the map surface.
 * @param sprite the sprite to draw
 * @param x x coordinate of the sprite's origin point in the map
 * @param y y coordinate of the sprite's origin point in the map
 */
void Map::draw_sprite(Sprite& sprite, int x, int y) {

  // the position is given in the map coordinate system:
  // convert it to the visible surface coordinate system
  const Rectangle& camera_position = get_camera_position();
  sprite.draw(*visible_surface,
      x - camera_position.get_x(),
      y - camera_position.get_y()
  );
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
  get_lua_context().run_map(*this, get_destination());
}

/**
 * @brief Exits the map.
 *
 * This function is called when the hero leaves the map.
 */
void Map::leave() {
  started = false;
  get_lua_context().map_on_finished(*this);
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
  entities->notify_map_opening_transition_finished();
  get_lua_context().map_on_opening_transition_finished(*this, get_destination());
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
  case OBSTACLE_TOP_RIGHT_WATER:
    // the upper right half of the square is an obstacle
    // so we have to test the position of the point
    x_in_tile = x & 7;
    y_in_tile = y & 7;
    on_obstacle = y_in_tile <= x_in_tile;
    break;

  case OBSTACLE_TOP_LEFT:
  case OBSTACLE_TOP_LEFT_WATER:
    // same thing
    x_in_tile = x & 7;
    y_in_tile = y & 7;
    on_obstacle = y_in_tile <= 7 - x_in_tile;
    break;

  case OBSTACLE_BOTTOM_LEFT:
  case OBSTACLE_BOTTOM_LEFT_WATER:
    x_in_tile = x & 7;
    y_in_tile = y & 7;
    on_obstacle = y_in_tile >= x_in_tile;
    break;

  case OBSTACLE_BOTTOM_RIGHT:
  case OBSTACLE_BOTTOM_RIGHT_WATER:
    x_in_tile = x & 7;
    y_in_tile = y & 7;
    on_obstacle = y_in_tile >= 7 - x_in_tile;
    break;

  case OBSTACLE_SHALLOW_WATER:
    on_obstacle = entity_to_check.is_shallow_water_obstacle();
    break;

  case OBSTACLE_DEEP_WATER:
    on_obstacle = entity_to_check.is_deep_water_obstacle();
    break;

  case OBSTACLE_HOLE:
    on_obstacle = entity_to_check.is_hole_obstacle();
    break;

  case OBSTACLE_LAVA:
    on_obstacle = entity_to_check.is_lava_obstacle();
    break;

  case OBSTACLE_PRICKLE:
    on_obstacle = entity_to_check.is_prickle_obstacle();
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
    collision =
	entity != &entity_to_check
	&& entity->is_enabled()
	&& entity->is_obstacle_for(entity_to_check)
	&& entity->overlaps(collision_box);
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

  return obstacle_to_ground(entities->get_obstacle_tile(layer, x, y));
}

/**
 * @brief Returns the value of ground equivalent to a value of obstacle.
 * TODO: remove Obstacle, only use Ground
 * @param obstacle an obstacle property
 * @return the corresponding ground
 */
Ground Map::obstacle_to_ground(Obstacle obstacle) {

  Ground ground = GROUND_EMPTY;
  switch (obstacle) {

    case OBSTACLE_SHALLOW_WATER:
      ground = GROUND_SHALLOW_WATER;
      break;

    case OBSTACLE_DEEP_WATER:
    case OBSTACLE_TOP_RIGHT_WATER:
    case OBSTACLE_TOP_LEFT_WATER:
    case OBSTACLE_BOTTOM_RIGHT_WATER:
    case OBSTACLE_BOTTOM_LEFT_WATER:
      ground = GROUND_DEEP_WATER;
      break;

    case OBSTACLE_HOLE:
      ground = GROUND_HOLE;
      break;

    case OBSTACLE_LAVA:
      ground = GROUND_LAVA;
      break;

    case OBSTACLE_PRICKLE:
      ground = GROUND_PRICKLE;
      break;

    case OBSTACLE_LADDER:
      ground = GROUND_LADDER;
      break;

    case OBSTACLE_EMPTY:
      ground = GROUND_EMPTY;
      break;

    case OBSTACLE:
    case OBSTACLE_NONE:
    case OBSTACLE_TOP_RIGHT:
    case OBSTACLE_TOP_LEFT:
    case OBSTACLE_BOTTOM_RIGHT:
    case OBSTACLE_BOTTOM_LEFT:
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
 * @brief Returns whether there is at least one empty tile in the specified rectangle.
 *
 * Only the borders of the rectangle are checked.
 *
 * @param layer the layer
 * @param collision_box the rectangle to test
 * @return true if there is at least one empty tile in this rectangle
 */
bool Map::has_empty_tiles(Layer layer, const Rectangle& collision_box) {

  bool empty_tile = false;

  // we just check the borders of the collision box
  int y1 = collision_box.get_y();
  int y2 = y1 + collision_box.get_height() - 1;
  int x1 = collision_box.get_x();
  int x2 = x1 + collision_box.get_width() - 1;

  for (int x = x1; x <= x2 && !empty_tile; x++) {
    empty_tile = entities->get_obstacle_tile(layer, x, y1) == OBSTACLE_EMPTY
        || entities->get_obstacle_tile(layer, x, y2) == OBSTACLE_EMPTY;
  }

  for (int y = y1; y <= y2 && !empty_tile; y++) {
    empty_tile = entities->get_obstacle_tile(layer, x1, y) == OBSTACLE_EMPTY
        || entities->get_obstacle_tile(layer, x2, y) == OBSTACLE_EMPTY;
  }

  return empty_tile;
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

    if (!(*i)->is_being_removed() && (*i)->is_enabled()) {
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

    if (!(*i)->is_being_removed()
        && (*i)->is_enabled()) {
      (*i)->check_collision(entity, sprite);
    }
  }
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return The name identifying this type in Lua.
 */
const std::string& Map::get_lua_type_name() const {
  return LuaContext::map_module_name;
}

