/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Destination.h"
#include "solarus/entities/Ground.h"
#include "solarus/entities/GroundInfo.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/Savegame.h"
#include "solarus/Sprite.h"

namespace Solarus {

/**
 * \brief Creates a map.
 * \param id Id of the map, used to determine the data file and
 * and the script file of the map. The data file must exist.
 */
Map::Map(const std::string& id):
  game(nullptr),
  id(id),
  width8(0),
  height8(0),
  min_layer(0),
  max_layer(0),
  tileset(nullptr),
  floor(MapData::NO_FLOOR),
  visible_surface(nullptr),
  background_surface(nullptr),
  foreground_surface(nullptr),
  loaded(false),
  started(false),
  destination_name(""),
  entities(nullptr),
  suspended(false) {

}

/**
 * \brief Returns the id of the map.
 * \return the map id
 */
const std::string& Map::get_id() const {
  return id;
}

/**
 * \brief Returns the id of the tileset associated to this map.
 * \return the id of the tileset
 */
const std::string& Map::get_tileset_id() const {
  // note that if set_tileset() has been called, tileset_id != tileset->get_id()
  return tileset_id;
}

/**
 * \brief Changes the tileset used to draw this map.
 *
 * The new tileset must be compatible with the previous one,
 * i.e. every tile of the previous tileset must exist in the new one
 * and have the same properties.
 * This function keeps the tiles of the previous tileset and loads the
 * image of the new tileset.
 *
 * \param tileset_id Id of the new tileset.
 */
void Map::set_tileset(const std::string& tileset_id) {

  tileset->set_images(tileset_id);
  get_entities().notify_tileset_changed();
  this->tileset_id = tileset_id;
  build_background_surface();
}

/**
 * \brief Returns the id of the music associated to this map.
 * \return The id of the music, possibly Music::none or Music::unchanged.
 */
const std::string& Map::get_music_id() const {
  return music_id;
}

/**
 * \brief Returns whether this map is in a world.
 *
 * This function returns \c true if the world is not an empty string.
 *
 * \return \c true if there is a world.
 */
bool Map::has_world() const {
  return !get_world().empty();
}

/**
 * \brief Returns the world where this map is.
 * \return The world name or an empty string.
 */
const std::string& Map::get_world() const {
  return world;
}

/**
 * \brief Returns the world where this map is.
 * \param world The world name or an empty string.
 */
void Map::set_world(const std::string& world) {
  this->world = world;
}

/**
 * \brief Returns whether this map has a floor.
 *
 * This function returns true if the floor is not MapData::NO_FLOOR.
 *
 * \return true if there is a floor.
 */
bool Map::has_floor() const {
  return get_floor() != MapData::NO_FLOOR;
}

/**
 * \brief Returns the floor where this map is.
 * \return The floor or FLOOR_NIL.
 */
int Map::get_floor() const {
  return floor;
}

/**
 * \brief Sets the floor where this map is.
 * \param floor The floor or FLOOR_NIL.
 */
void Map::set_floor(int floor) {
  this->floor = floor;
}

/**
 * \brief Returns the location of this map in its context.
 *
 * The location returned is the location of the map's top-left corner
 * relative to its context (its floor or its world).
 * The width and height fields correspond to the map size.
 *
 * \return The location of this map in its context.
 */
const Rectangle& Map::get_location() const {
  return location;
}

/**
 * \brief Returns the size of this map.
 * \return The size of this map in pixels.
 */
Size Map::get_size() const {
  return { location.get_width(), location.get_height() };
}

/**
 * \brief Returns the map width in pixels.
 * \return the map width
 */
int Map::get_width() const {
  return location.get_width();
}

/**
 * \brief Returns the map height in pixels.
 * \return the map height
 */
int Map::get_height() const {
  return location.get_height();
}

/**
 * \brief Returns the map width in number of 8*8 squares.
 *
 * This is equivalent to get_width() / 8.
 *
 * \return the map width in number of 8*8 squares
 */
int Map::get_width8() const {
  return width8;
}

/**
 * \brief Returns the map height in number of 8*8 squares.
 *
 * This is equivalent to get_height() / 8.
 *
 * \return the map height in number of 8*8 squares
 */
int Map::get_height8() const {
  return height8;
}

/**
 * \brief Returns the index of the first layer of the map.
 * \return The first (lowest) layer (0 or less).
 */
int Map::get_min_layer() const {

  return min_layer;
}

/**
 * \brief Returns the index of the last layer of the map.
 * \return The last (highest) layer (0 or more).
 */
int Map::get_max_layer() const {

  return max_layer;
}

/**
 * \brief Returns whether the specified layer exists in this map.
 * \param layer The layer to check.
 * \return \c true if the layer is between \c get_lowest_layer()
 * and \c get_highest_layer().
 */
bool Map::is_valid_layer(int layer) const {

  return layer >= get_min_layer() && layer <= get_max_layer();
}

/**
 * \brief Returns whether the map is loaded.
 * \return true if the map is loaded, false otherwise
 */
bool Map::is_loaded() const {
  return loaded;
}

/**
 * \brief Unloads the map.
 *
 * Destroys all entities in the map to free some memory. This function
 * can be called when the player exists the map. If the player is likely to
 * come back on the map, you can keep the map loaded.
 */
void Map::unload() {

  if (is_loaded()) {
    tileset = nullptr;
    visible_surface = nullptr;
    background_surface = nullptr;
    foreground_surface = nullptr;
    entities = nullptr;

    loaded = false;
  }
}

/**
 * \brief Loads the map into a game.
 *
 * Reads the description file of the map.
 *
 * \param game the game
 */
void Map::load(Game& game) {

  visible_surface = Surface::create(
      Video::get_quest_size()
  );
  visible_surface->set_software_destination(false);

  background_surface = Surface::create(
      Video::get_quest_size()
  );
  background_surface->set_software_destination(false);

  // Read the map data file.
  MapData data;
  const std::string& file_name = std::string("maps/") + get_id() + ".dat";
  bool success = data.import_from_quest_file(file_name);

  if (!success) {
    Debug::die("Failed to load map data file '" + file_name + "'");
  }

  // Initialize the map from the data just read.
  this->game = &game;
  location.set_xy(data.get_location());
  location.set_size(data.get_size());
  width8 = data.get_size().width / 8;
  height8 = data.get_size().height / 8;
  min_layer = data.get_min_layer();
  max_layer = data.get_max_layer();
  music_id = data.get_music_id();
  set_world(data.get_world());
  set_floor(data.get_floor());
  tileset_id = data.get_tileset_id();
  tileset = std::unique_ptr<Tileset>(new Tileset(data.get_tileset_id()));
  tileset->load();
  entities = std::unique_ptr<Entities>(new Entities(game, *this));
  entities->create_entities(data);

  build_background_surface();
  build_foreground_surface();

  loaded = true;
}

/**
 * \brief Returns the shared Lua context.
 *
 * This function should not be called before the map is loaded into a game.
 *
 * \return The Lua context where all scripts are run.
 */
LuaContext& Map::get_lua_context() {
  return game->get_lua_context();
}

/**
 * \brief Returns the game that loaded this map.
 *
 * This function should not be called before the map is loaded into a game.
 *
 * \return the game
 */
Game& Map::get_game() {
  return *game;
}

/**
 * \brief Sets the current destination point of the map.
 * \param destination_name Name of the destination point you want to use.
 * An empty string means the default one.
 * You can also use "_same" to keep the hero's coordinates, or
 * "_side0", "_side1", "_side2" or "_side3"
 * to place the hero on a side of the map.
 */
void Map::set_destination(const std::string& destination_name) {

  this->destination_name = destination_name;
}

/**
 * \brief Returns the destination point name set by the last call to
 * set_destination().
 * \return The name of the destination point previously set,
 * possibly an empty string (meaning the default one).
 */
const std::string& Map::get_destination_name() const {
  return destination_name;
}

/**
 * \brief Returns the destination point specified by the last call to
 * set_destination().
 *
 * If the destination point was set to a special value
 * ("_same", "_side0", "_side1", "_side2" or "_side3"), returns nullptr.
 *
 * If the destination name is empty, returns the default destination if any,
 * or nullptr.
 *
 * Otherwise, if there is no destination entity with this name on the map,
 * prints an error message and returns the default destination if any or nullptr.
 *
 * \return The destination point previously set, or nullptr.
 */
Destination* Map::get_destination() {

  if (destination_name == "_same"
      || destination_name.substr(0,5) == "_side") {
    return nullptr;
  }

  Debug::check_assertion(is_loaded(), "This map is not loaded");

  std::shared_ptr<Destination> destination;
  if (!destination_name.empty()) {
    // Use the destination whose name was specified.
    const EntityPtr& entity = get_entities().find_entity(destination_name);

    if (entity == nullptr || entity->get_type() != EntityType::DESTINATION) {
      Debug::error(
          std::string("Map '") + get_id() + "': No such destination: '"
          + destination_name + "'"
      );
      // Perhaps the game was saved with a destination that no longer exists
      // or whose name was changed during the development of the quest.
      // This is not a fatal error: we will use the default destination
      // instead. It is up to quest makers to avoid this situation once their
      // quest is released.
    }
    else {
      destination = std::static_pointer_cast<Destination>(entity);
    }
  }

  if (destination == nullptr) {
    // No valid destination name was set: use the default one if any.
    destination = get_entities().get_default_destination();
  }

  return destination.get();
}

/**
 * \brief When the destination point is a side of the map,
 * returns this side.
 * \return the destination side (0 to 3), or -1 if the destination point is not a side
 */
int Map::get_destination_side() const {

  if (destination_name.substr(0,5) == "_side") {
    int destination_side = destination_name[5] - '0';
    return destination_side;
  }
  return -1;
}

/**
 * \brief Returns the surface where the map is displayed.
 *
 * This surface is only the visible part of the map, so the
 * coordinates on this surface are relative to the screen,
 * not to the map.
 *
 * \return the surface where the map is displayed
 */
const SurfacePtr& Map::get_visible_surface() {
  return visible_surface;
}

/**
 * \brief Makes the camera move towards a point.
 * \param x x coordinate of the target point
 * \param y y coordinate of the target point
 * \param speed speed of the movement
 */
void Map::move_camera(int x, int y, int speed) {

  Camera& camera = get_camera();
  camera.set_speed(speed);
  camera.move(x, y);
}

/**
 * \brief Makes the camera move back to the hero.
 */
void Map::restore_camera() {

  Camera& camera = get_camera();
  camera.restore();
}

/**
 * \brief Returns whether there is a camera movement.
 *
 * It may be a movement towards a point or a scrolling movement due to a
 * separator.
 *
 * \return \c true if the camera is moving.
 */
bool Map::is_camera_moving() const {

  const Camera& camera = get_camera();
  return camera.is_moving();
}

/**
 * \brief Makes the camera traverse a separator.
 * \param separator The separator to traverse.
 */
void Map::traverse_separator(Separator* separator) {

  Camera& camera = get_camera();
  camera.traverse_separator(separator);
}

/**
 * \brief Suspends or resumes the movement and animations of the entities.
 *
 * This function is called when the game is being suspended
 * or resumed.
 *
 * \param suspended true to suspend the movement and the animations,
 * false to resume them
 */
void Map::set_suspended(bool suspended) {

  this->suspended = suspended;

  entities->set_suspended(suspended);
  get_lua_context().notify_map_suspended(*this, suspended);
}

/**
 * \brief This function is called when a low-level input event occurs on this map.
 * \param event the event to handle
 * \return \c true if the event was handled and should stop being propagated.
 */
bool Map::notify_input(const InputEvent& event) {

  bool handled = get_lua_context().map_on_input(*this, event);
  return handled;
}

/**
 * \brief Updates the animation and the position of each map elements, including the hero.
 */
void Map::update() {

  // detect whether the game has just been suspended or resumed
  check_suspended();

  // update the elements
  TilePattern::update();
  entities->update();
  get_lua_context().map_on_update(*this);
}

/**
 * \brief Returns whether the map is currently suspended.
 * \return true if the map is suspended.
 */
bool Map::is_suspended() const {
  return suspended;
}

/**
 * \brief Checks whether the game has just been suspended or resumed
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
 * \brief Draws the map with all its entities on the screen.
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
    get_lua_context().map_on_draw(*this, visible_surface);
  }
}

/**
 * \brief Builds or rebuilds the surface corresponding to the background of
 * the tileset.
 */
void Map::build_background_surface() {

  if (tileset != nullptr) {
    background_surface->clear();
    background_surface->fill_with_color(tileset->get_background_color());
  }
}

/**
 * \brief Draws the background of the map.
 */
void Map::draw_background() {

  background_surface->draw(visible_surface);
}

/**
 * \brief Builds a surface with black bars when the map is smaller than the
 * quest size.
 */
void Map::build_foreground_surface() {

  foreground_surface = nullptr;

  const int screen_width = visible_surface->get_width();
  const int screen_height = visible_surface->get_height();

  // If the map is too small for the screen, add black bars outside the map.
  const int map_width = get_width();
  const int map_height = get_height();

  if (map_width >= screen_width
      && map_height >= screen_height) {
    // Nothing to do.
    return;
  }

  foreground_surface = Surface::create(visible_surface->get_size());
  // Keep this surface as software destination because it is built only once.

  if (map_width < screen_width) {
    int bar_width = (screen_width - map_width) / 2;
    Rectangle dst_position(0, 0, bar_width, screen_height);
    foreground_surface->fill_with_color(Color::black, dst_position);
    dst_position.set_x(bar_width + map_width);
    foreground_surface->fill_with_color(Color::black, dst_position);
  }

  if (map_height < screen_height) {
    int bar_height = (screen_height - map_height) / 2;
    Rectangle dst_position(0, 0, screen_width, bar_height);
    foreground_surface->fill_with_color(Color::black, dst_position);
    dst_position.set_y(bar_height + map_height);
    foreground_surface->fill_with_color(Color::black, dst_position);
  }
}

/**
 * \brief Draws the foreground of the map.
 */
void Map::draw_foreground() {

  if (foreground_surface != nullptr) {
    foreground_surface->draw(visible_surface);
  }
}

/**
 * \brief Draws a sprite on the map surface.
 * \param sprite the sprite to draw
 * \param xy coordinates of the sprite's origin point in the map
 */
void Map::draw_sprite(Sprite& sprite, const Point &xy) {

  draw_sprite(sprite, xy.x, xy.y);
}

/**
 * \brief Draws a sprite on the map surface.
 * \param sprite the sprite to draw
 * \param x x coordinate of the sprite's origin point in the map
 * \param y y coordinate of the sprite's origin point in the map
 */
void Map::draw_sprite(Sprite& sprite, int x, int y) {

  // the position is given in the map coordinate system:
  // convert it to the visible surface coordinate system
  const Camera& camera = get_camera();
  sprite.draw(visible_surface,
      x - camera.get_top_left_x(),
      y - camera.get_top_left_y()
  );
}

/**
 * \brief Draws a sprite on a restricted area of the map surface.
 * \param sprite The sprite to draw.
 * \param x X coordinate of the sprite's origin point in the map.
 * \param y Y coordinate of the sprite's origin point in the map.
 * \param clipping_area Rectangle of the map where the drawing will be
 * restricted. A flat rectangle means no restriction.
 */
void Map::draw_sprite(Sprite& sprite, int x, int y,
    const Rectangle& clipping_area) {

  if (clipping_area.is_flat()) {
    // No clipping area.
    draw_sprite(sprite, x, y);
    return;
  }

  const Camera& camera = get_camera();
  const Rectangle region_in_frame(
      clipping_area.get_x() - x,
      clipping_area.get_y() - y,
      clipping_area.get_width(),
      clipping_area.get_height()
  );
  const Point dst_position = {
      x - camera.get_top_left_x(),
      y - camera.get_top_left_y()
  };
  sprite.draw_region(
      region_in_frame,
      visible_surface,
      dst_position
  );
}

/**
 * \brief Starts the map.
 *
 * The map must be loaded.
 * The background music starts and the map script is initialized.
 */
void Map::start() {

  this->started = true;
  this->visible_surface->set_opacity(255);

  Music::play(music_id, true);
  this->entities->notify_map_started();
  get_lua_context().run_map(*this, get_destination());
}

/**
 * \brief Exits the map.
 *
 * This function is called when the hero leaves the map.
 */
void Map::leave() {
  started = false;
  get_lua_context().map_on_finished(*this);
  this->entities->notify_map_finished();
}

/**
 * \brief Returns whether the map is started.
 *
 * This function returns whether this is the current map and the hero is on it.
 *
 * \return true if the map is started
 */
bool Map::is_started() const {
  return started;
}

/**
 * \brief This function is called when the map is started and
 * the opening transition is finished.
 */
void Map::notify_opening_transition_finished() {

  visible_surface->set_opacity(255); // because the transition effect may have changed the opacity
  check_suspended();
  entities->notify_map_opening_transition_finished();
  get_lua_context().map_on_opening_transition_finished(*this, get_destination());
}

/**
 * \brief Tests whether a rectangle has overlaps the outside part of the map area.
 * \param collision_box the rectangle to check
 * \return true if a point of the rectangle is outside the map area
 */
bool Map::test_collision_with_border(const Rectangle& collision_box) const {

  return collision_box.get_x() < 0 || collision_box.get_x() + collision_box.get_width() >= get_width()
    || collision_box.get_y() < 0 || collision_box.get_y() + collision_box.get_height() >= get_height();
}

/**
 * \brief Tests whether a point collides with the ground of the map.
 *
 * The ground is the terrain of the point. It is defined by the tiles and
 * by the presence of entities that may change it
 * (like dynamic tiles and destructible items).
 *
 * This method also returns true if the point is outside the map.
 *
 * \param layer Layer of the point.
 * \param x X of the point in pixels.
 * \param y Y of the point in pixels.
 * \param entity_to_check The entity to check (used to decide what grounds are
 * considered as obstacle).
 * \param [out] found_diagonal_wall \c true if the ground under this point was
 * a diagonal wall, unchanged otherwise.
 * Your algorithm may decide to check more points if there is a diagonal wall.
 * \return \c true if this point is on an obstacle.
 */
bool Map::test_collision_with_ground(
    int layer,
    int x,
    int y,
    const Entity& entity_to_check,
    bool& found_diagonal_wall) const {

  bool on_obstacle = false;
  int x_in_tile, y_in_tile;

  // If the point is outside the map, this is an obstacle.
  if (test_collision_with_border(x, y)) {
    return true;
  }

  // Get the ground property under this point.
  Ground ground = get_ground(layer, x, y, &entity_to_check);
  switch (ground) {

  case Ground::EMPTY:
  case Ground::TRAVERSABLE:
  case Ground::GRASS:
  case Ground::ICE:
  case Ground::WALL:
  case Ground::LOW_WALL:
  case Ground::SHALLOW_WATER:
  case Ground::DEEP_WATER:
  case Ground::HOLE:
  case Ground::LAVA:
  case Ground::PRICKLE:
  case Ground::LADDER:
    on_obstacle = entity_to_check.is_ground_obstacle(ground);
    break;

  case Ground::WALL_TOP_RIGHT:
  case Ground::WALL_TOP_RIGHT_WATER:
    // The upper right half of the square is an obstacle
    // so we have to test the position of the point in the square.
    x_in_tile = x & 7;
    y_in_tile = y & 7;
    on_obstacle = y_in_tile <= x_in_tile;
    found_diagonal_wall = true;
    break;

  case Ground::WALL_TOP_LEFT:
  case Ground::WALL_TOP_LEFT_WATER:
    // Same thing.
    x_in_tile = x & 7;
    y_in_tile = y & 7;
    on_obstacle = y_in_tile <= 7 - x_in_tile;
    found_diagonal_wall = true;
    break;

  case Ground::WALL_BOTTOM_LEFT:
  case Ground::WALL_BOTTOM_LEFT_WATER:
    x_in_tile = x & 7;
    y_in_tile = y & 7;
    on_obstacle = y_in_tile >= x_in_tile;
    found_diagonal_wall = true;
    break;

  case Ground::WALL_BOTTOM_RIGHT:
  case Ground::WALL_BOTTOM_RIGHT_WATER:
    x_in_tile = x & 7;
    y_in_tile = y & 7;
    on_obstacle = y_in_tile >= 7 - x_in_tile;
    found_diagonal_wall = true;
    break;

  }

  return on_obstacle;
}

/**
 * \brief Tests whether a rectangle overlaps an obstacle dynamic entity.
 * \param layer The layer.
 * \param collision_box The rectangle to check.
 * \param entity_to_check The entity to check (used to decide what is
 * considered as obstacle).
 * \return \c true if there is an obstacle entity at this point.
 */
bool Map::test_collision_with_entities(
    int layer,
    const Rectangle& collision_box,
    Entity& entity_to_check) {

  EntityVector entities_nearby;
  get_entities().get_entities_in_rectangle(collision_box, entities_nearby);
  for (const EntityPtr& entity_nearby: entities_nearby) {

    if (entity_nearby->overlaps(collision_box) &&
        (entity_nearby->get_layer() == layer || entity_nearby->has_layer_independent_collisions()) &&
        entity_nearby->is_obstacle_for(entity_to_check, collision_box) &&
        entity_nearby->is_enabled() &&
        !entity_nearby->is_being_removed() &&
        entity_nearby.get() != &entity_to_check) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Tests whether a rectangle collides with the map obstacles.
 * \param layer Layer of the rectangle in the map.
 * \param collision_box The rectangle to check (its dimensions should be
 * multiples of 8).
 * \param entity_to_check The entity to check (used to decide what is
 * considered as obstacle).
 * \return \c true if the rectangle is overlapping an obstacle.
 */
bool Map::test_collision_with_obstacles(
    int layer,
    const Rectangle& collision_box,
    Entity& entity_to_check) {

  // This function is called very often.
  // For performance reasons, we only check the border of the of the collision box.

  // Collisions with the terrain
  // (i.e., tiles and dynamic entities that may change it).
  const int x1 = collision_box.get_x();
  const int x2 = x1 + collision_box.get_width() - 1;
  const int y1 = collision_box.get_y();
  const int y2 = y1 + collision_box.get_height() - 1;

  // First, only check the terrain of both extremities of each 8-pixel
  // segment of the border.
  // This is enough for all terrains (except diagonal ones, see below)
  // because the tested collision box makes at least 8x8 pixels.
  bool found_diagonal_wall = false;
  for (int x = x1; x <= x2; x += 8) {
    if (test_collision_with_ground(layer, x, y1, entity_to_check, found_diagonal_wall)
        || test_collision_with_ground(layer, x, y2, entity_to_check, found_diagonal_wall)
        || test_collision_with_ground(layer, x + 7, y1, entity_to_check, found_diagonal_wall)
        || test_collision_with_ground(layer, x + 7, y2, entity_to_check, found_diagonal_wall)) {
      return true;
    }
  }

  for (int y = y1; y <= y2; y += 8) {
    if (test_collision_with_ground(layer, x1, y, entity_to_check, found_diagonal_wall)
        || test_collision_with_ground(layer, x2, y, entity_to_check, found_diagonal_wall)
        || test_collision_with_ground(layer, x1, y + 7, entity_to_check, found_diagonal_wall)
        || test_collision_with_ground(layer, x2, y + 7, entity_to_check, found_diagonal_wall)) {
      return true;
    }
  }

  // Is a full check of the border needed?
  // This is costly, but hopefully, we seldom need it.
  if (found_diagonal_wall) {
    // A diagonal wall ground was involved in the terrain check.
    // In this case, we need to test all points of the border of the collision
    // box. Otherwise, walls with sharp angles like 'V' become
    // partially traversable.
    for (int x = x1; x <= x2; ++x) {
      if (test_collision_with_ground(layer, x, y1, entity_to_check, found_diagonal_wall)
          || test_collision_with_ground(layer, x, y2, entity_to_check, found_diagonal_wall)) {
        return true;
      }
    }

    for (int y = y1; y <= y2; ++y) {
      if (test_collision_with_ground(layer, x1, y, entity_to_check, found_diagonal_wall)
          || test_collision_with_ground(layer, x2, y, entity_to_check, found_diagonal_wall)) {
        return true;
      }
    }
  }

  // No collision with the terrain: check collisions with dynamic entities.
  return test_collision_with_entities(layer, collision_box, entity_to_check);
}

/**
 * \brief Tests whether a point collides with the map obstacles.
 * \param layer Layer of point to check.
 * \param x X coordinate of the point to check.
 * \param y Y coordinate of the point to check.
 * \param entity_to_check The entity to check (used to decide what is
 * considered as obstacle)
 * \return \c true if the point is overlapping an obstacle.
 */
bool Map::test_collision_with_obstacles(
    int layer,
    int x,
    int y,
    Entity& entity_to_check
) {

  bool is_diagonal_wall = false;

  // Test the terrain.
  bool collision = test_collision_with_ground(layer, x, y, entity_to_check, is_diagonal_wall);

  // Test dynamic entities.
  if (!collision) {
    Rectangle collision_box(x, y, 1, 1);
    collision = test_collision_with_entities(layer, collision_box, entity_to_check);
  }

  return collision;
}

/**
 * \brief Tests whether a point collides with the map obstacles.
 * \param layer Layer of point to check.
 * \param point Point to check.
 * \param entity_to_check The entity to check (used to decide what is
 * considered as obstacle)
 * \return \c true if the point is overlapping an obstacle.
 */
bool Map::test_collision_with_obstacles(
    int layer,
    const Point& point,
    Entity& entity_to_check) {

  return test_collision_with_obstacles(layer, point.x, point.y, entity_to_check);
}

/**
 * \brief Returns whether there is empty ground in the specified rectangle.
 *
 * Only the borders of the rectangle are checked.
 *
 * \param layer The layer.
 * \param collision_box The rectangle to test.
 * \return \c true if there is at least one empty tile in this rectangle.
 */
bool Map::has_empty_ground(int layer, const Rectangle& collision_box) const {

  bool empty_tile = false;

  // We just check the borders of the collision box.
  int y1 = collision_box.get_y();
  int y2 = y1 + collision_box.get_height() - 1;
  int x1 = collision_box.get_x();
  int x2 = x1 + collision_box.get_width() - 1;

  for (int x = x1; x <= x2 && !empty_tile; x++) {
    empty_tile = get_ground(layer, x, y1, nullptr) == Ground::EMPTY
        || get_ground(layer, x, y2, nullptr) == Ground::EMPTY;
  }

  for (int y = y1; y <= y2 && !empty_tile; y++) {
    empty_tile = get_ground(layer, x1, y, nullptr) == Ground::EMPTY
        || get_ground(layer, x2, y, nullptr) == Ground::EMPTY;
  }

  return empty_tile;
}


/**
 * \brief Returns the ground at the specified point.
 *
 * Static tiles and dynamic entities are all taken into account here.
 *
 * When an entity has a diagonal ground, only 8x8 squares on the diagonal of
 * that entity are diagonal grounds, other ones are resolved Ground::WALL on a
 * side and Ground::TRAVERSABLE or Ground::DEEP_WATER on the other side.
 *
 * A current limitation is that entities with diagonal grounds must be aligned
 * to the 8x8 grid to work correctly.
 *
 * \param layer Layer of the point.
 * \param x X coordinate of the point.
 * \param y Y coordinate of the point.
 * \param entity_to_check The entity you want to know the ground of (if any).
 * Used to make sure that the entity's own modified ground does not count.
 * \return The ground at this place.
 */
Ground Map::get_ground(
    int layer,
    int x,
    int y,
    const Entity* entity_to_check
) const {
  return get_ground(layer, Point(x, y), entity_to_check);
}

/**
 * \brief Returns the ground at the specified point.
 *
 * Static tiles and dynamic entities are all taken into account here.
 *
 * \param layer Layer of the point.
 * \param xy Coordinates of the point.
 * \param entity_to_check The entity you want to know the ground of (if any).
 * Used to make sure that the entity's own modified ground does not count.
 * \return The ground at this place.
 */
Ground Map::get_ground(
    int layer,
    const Point& xy,
    const Entity* entity_to_check
) const {

  if (test_collision_with_border(xy)) {
    // Outside the map bounds.
    return Ground::EMPTY;
  }

  // See if a dynamic entity changes the ground.
  const Rectangle box(xy, Size(1, 1));
  ConstEntityVector entities_nearby;
  get_entities().get_entities_in_rectangle_sorted(box, entities_nearby);

  const auto& rend = entities_nearby.rend();
  for (auto it = entities_nearby.rbegin(); it != rend; ++it) {
    const Entity& entity_nearby = *(*it);

    const Ground ground = entity_nearby.get_modified_ground();

    if (&entity_nearby == entity_to_check) {
      // Skip the entity itself.
      continue;
    }
    // TODO also skip entities above?

    if (ground == Ground::EMPTY) {
      // The entity has no influence on the ground.
      continue;
    }

    if (entity_nearby.overlaps(xy) &&
        entity_nearby.get_layer() == layer &&
        entity_nearby.is_enabled() &&
        !entity_nearby.is_being_removed()
    ) {
      return get_ground_from_entity(entity_nearby, xy);
    }
  }

  // Otherwise, return the ground defined by static tiles (this is very fast).
  return entities->get_tile_ground(layer, xy.x, xy.y);
}

/**
 * \brief Returns the modified ground of an entity at the specified point.
 *
 * The point is assumed to overlap the entity.
 *
 * The point only matters if the ground is diagonal.
 * When an entity has a diagonal ground, only 8x8 squares on the diagonal of
 * that entity are diagonal grounds, other ones are resolved Ground::WALL on a
 * side and Ground::TRAVERSABLE or Ground::DEEP_WATER on the other side.
 *
 * A current limitation is that entities with diagonal grounds must be aligned
 * to the 8x8 grid to work correctly.
 *
 * \param entity Entity whose modified ground to get.
 * \param x X coordinate of the point.
 * \param y Y coordinate of the point.
 * \return The ground at this place.
 */
Ground Map::get_ground_from_entity(const Entity& entity, int x, int y) const {
  return get_ground_from_entity(entity, Point(x, y));
}

/**
 * \brief Returns the modified ground of an entity at the specified point.
 *
 * The point is assumed to overlap the entity.
 *
 * The point only matters if the ground is diagonal.
 * When an entity has a diagonal ground, only 8x8 squares on the diagonal of
 * that entity are diagonal grounds, other ones are resolved Ground::WALL on a
 * side and Ground::TRAVERSABLE or Ground::DEEP_WATER on the other side.
 *
 * A current limitation is that entities with diagonal grounds must be aligned
 * to the 8x8 grid to work correctly.
 *
 * \param entity Entity whose modified ground to get.
 * \param xy Coordinates of the point.
 * \return The ground at this place.
 */
Ground Map::get_ground_from_entity(const Entity& entity, const Point& xy) const {

  const Ground ground = entity.get_modified_ground();
  if (!GroundInfo::is_ground_diagonal(ground)) {
    // Easy case.
    return ground;
  }

  const bool square = entity.get_width() == entity.get_height();
  if (!square) {
    // Malformed entity.
    return Ground::TRAVERSABLE;
  }
  if (!entity.is_aligned_to_grid() || entity.get_width() % 8 != 0) {
    // Not supported yet.
    return Ground::TRAVERSABLE;
  }

  if (entity.get_width() == 8) {
    // The entity only occupies one 8x8 square: the ground is then fully diagonal.
    return ground;
  }

  const Point point_in_entity = xy - entity.get_top_left_xy();
  const Point square_in_entity = point_in_entity / 8;

  const int num_squares = entity.get_width() / 8;
  int sum = 0;

  switch (ground) {

    // If the top right corner of the tile is an obstacle,
    // then the top right 8x8 squares are Ground::WALL, the bottom left 8x8
    // squares are Ground::TRAVERSABLE or Ground::DEEP_WATER and the 8x8 squares
    // on the diagonal are Ground::WALL_TOP_RIGHT.
    case Ground::WALL_TOP_RIGHT:
    case Ground::WALL_TOP_RIGHT_WATER:

      if (square_in_entity.x == square_in_entity.y) {
        // 8x8 square on the diagonal.
        return ground;
      }
      if (square_in_entity.x > square_in_entity.y) {
        // 8x8 square in the wall part.
        return Ground::WALL;
      }
      // 8x8 square in the non-wall part.
      return ground == Ground::WALL_TOP_RIGHT ? Ground::TRAVERSABLE : Ground::DEEP_WATER;

    case Ground::WALL_TOP_LEFT:
    case Ground::WALL_TOP_LEFT_WATER:

      sum = square_in_entity.x + square_in_entity.y;
      if (sum == num_squares - 1) {
        // 8x8 square on the diagonal.
        return ground;
      }
      if (sum < num_squares - 1) {
        // 8x8 square in the wall part.
        return Ground::WALL;
      }
      // 8x8 square in the non-wall part.
      return ground == Ground::WALL_TOP_LEFT ? Ground::TRAVERSABLE : Ground::DEEP_WATER;

    case Ground::WALL_BOTTOM_LEFT:
    case Ground::WALL_BOTTOM_LEFT_WATER:

      if (square_in_entity.x == square_in_entity.y) {
        // 8x8 square on the diagonal.
        return ground;
      }
      if (square_in_entity.x < square_in_entity.y) {
        // 8x8 square in the wall part.
        return Ground::WALL;
      }
      // 8x8 square in the non-wall part.
      return ground == Ground::WALL_BOTTOM_LEFT ? Ground::TRAVERSABLE : Ground::DEEP_WATER;

    case Ground::WALL_BOTTOM_RIGHT:
    case Ground::WALL_BOTTOM_RIGHT_WATER:

      sum = square_in_entity.x + square_in_entity.y;
      if (sum == num_squares - 1) {
        // 8x8 square on the diagonal.
        return ground;
      }
      if (sum > num_squares - 1) {
        // 8x8 square in the wall part.
        return Ground::WALL;
      }
      // 8x8 square in the non-wall part.
      return ground == Ground::WALL_BOTTOM_RIGHT ? Ground::TRAVERSABLE : Ground::DEEP_WATER;

    default:
      return ground;
  }
}

/**
 * \brief Checks the collisions between an entity and the detectors of the map.
 *
 * This function is called by an entity sensitive to the entity detectors
 * when this entity has just moved on the map, or when a detector
 * wants to check this entity.
 * We check whether or not the entity overlaps an entity detector.
 * If the map is suspended, this function does nothing.
 *
 * \param entity The entity that has just moved (this entity should have
 * a movement sensible to the collisions).
 */
void Map::check_collision_with_detectors(Entity& entity) {

  if (suspended) {
    return;
  }

  if (entity.is_being_removed()) {
    return;
  }

  // Check this entity with each detector.

  // Extend the box because some collision tests work without overlapping.
  Rectangle box = entity.get_extended_bounding_box(8);
  std::vector<EntityPtr> entities_nearby;
  entities->get_entities_in_rectangle(box, entities_nearby);
  for (const EntityPtr& entity_nearby: entities_nearby) {

    if (!entity_nearby->is_detector()) {
      // Most entities are detectors anyway.
      continue;
    }

    if (entity_nearby->is_enabled()
        && !entity_nearby->is_suspended()
        && !entity_nearby->is_being_removed()) {
      entity_nearby->check_collision(entity);
    }
  }
}

/**
 * \brief Checks the collisions between all entities and a detector.
 *
 * This function is called when a detector wants to check entities,
 * typically when the detector has just moved.
 * If the map is suspended, this function does nothing.
 *
 * \param detector A detector.
 */
void Map::check_collision_from_detector(Entity& detector) {

  if (suspended) {
    return;
  }

  if (detector.is_being_removed()) {
    return;
  }

  // First check the hero.
  detector.check_collision(get_entities().get_hero());

  // Check each entity with this detector.
  Rectangle box = detector.get_extended_bounding_box(8);
  std::vector<EntityPtr> entities_nearby;
  entities->get_entities_in_rectangle(box, entities_nearby);
  for (const EntityPtr& entity_nearby: entities_nearby) {

    if (entity_nearby->is_enabled()
        && !entity_nearby->is_suspended()
        && !entity_nearby->is_being_removed()
        && entity_nearby.get() != &detector
    ) {
      detector.check_collision(*entity_nearby);
    }
  }
}

/**
 * \brief Checks the pixel-precise collisions between an entity and the
 * detectors of the map.
 *
 * This function is called by an entity
 * when the frame of one of its sprites has just changed.
 * We check whether or not the sprite overlaps the detector.
 * If the map is suspended, this function does nothing.
 *
 * \param entity A map entity.
 * \param sprite The sprite of this entity to check.
 */
void Map::check_collision_with_detectors(Entity& entity, Sprite& sprite) {

  if (suspended) {
    return;
  }

  // Check each detector.
  Rectangle box = entity.get_max_bounding_box();
  std::vector<EntityPtr> entities_nearby;
  entities->get_entities_in_rectangle(box, entities_nearby);
  for (const EntityPtr& entity_nearby: entities_nearby) {

    if (!entity_nearby->is_detector()) {
      continue;
    }

    if (!entity_nearby->is_being_removed()
        && !entity_nearby->is_suspended()
        && entity_nearby->is_enabled()) {
      entity_nearby->check_collision(entity, sprite);
    }
  }
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& Map::get_lua_type_name() const {
  return LuaContext::map_module_name;
}

}

