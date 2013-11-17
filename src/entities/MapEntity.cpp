/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/MapEntity.h"
#include "entities/MapEntities.h"
#include "entities/Tileset.h"
#include "entities/Switch.h"
#include "entities/Destructible.h"
#include "entities/Separator.h"
#include "entities/Hero.h"
#include "movements/Movement.h"
#include "lua/LuaContext.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "MainLoop.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"

const Rectangle MapEntity::directions_to_xy_moves[] = {
  Rectangle( 1, 0),
  Rectangle( 1,-1),
  Rectangle( 0,-1),
  Rectangle(-1,-1),
  Rectangle(-1, 0),
  Rectangle(-1, 1),
  Rectangle( 0, 1),
  Rectangle( 1, 1)
};

const std::string MapEntity::entity_type_names[ENTITY_NUMBER + 1] = {
    "tile",
    "destination",
    "teletransporter",
    "pickable",
    "destructible",
    "chest",
    "jumper",
    "enemy",
    "npc",
    "block",
    "dynamic_tile",
    "switch",
    "wall",
    "sensor",
    "crystal",
    "crystal_block",
    "shop_treasure",
    "conveyor_belt",
    "door",
    "stairs",
    "separator",
    "custom",
    "hero",
    "carried_object",
    "boomerang",
    "explosion",
    "arrow",
    "bomb",
    "fire",
    "hookshot",
    ""  // Sentinel.
};

/**
 * \brief Creates an entity, specifying its position, its name and its direction.
 * \param name Unique name identifying the entity on the map or an empty string.
 * \param direction direction of the entity
 * \param layer layer of the entity
 * \param x x position of the entity
 * \param y y position of the entity
 * \param width width of the entity
 * \param height height of the entity
 */
MapEntity::MapEntity(
    const std::string& name,
    int direction,
    Layer layer,
    int x,
    int y,
    int width,
    int height):
  main_loop(NULL),
  map(NULL),
  layer(layer),
  bounding_box(x, y, width, height),
  ground_below(GROUND_EMPTY),
  origin(0, 0),
  name(name),
  direction(direction),
  visible(true),
  movement(NULL),
  movement_events_enabled(true),
  facing_entity(NULL),
  being_removed(false),
  enabled(true),
  waiting_enabled(false),
  suspended(false),
  when_suspended(0),
  optimization_distance(default_optimization_distance),
  optimization_distance2(default_optimization_distance * default_optimization_distance) {

}

/**
 * \brief Destructor.
 *
 * The sprite and the movement of the entity are deleted (if any).
 */
MapEntity::~MapEntity() {

  clear_sprites();
  clear_old_sprites();
  clear_movement();
  clear_old_movements();
}

/**
 * \brief Returns whether this entity is the hero controlled by the player.
 * \return true if this entity is the hero
 */
bool MapEntity::is_hero() const {
  return get_type() == ENTITY_HERO;
}

/**
 * \brief Returns whether entities of this type have detection capabilities.
 *
 * This function returns whether entities of this type can detect the presence
 * of the hero or other entities. If yes, the function
 * notify_collision() will be called when a collision is detected.
 *
 * \return \c true if this type of entity can detect other entities.
 */
bool MapEntity::is_detector() const {
  return false;
}

/**
 * \brief Returns whether entities of this type can be obstacles for other entities.
 *
 * This function returns \c true by default.
 * If this function returns \c true, the entity is added to the list of
 * potential obstacles when it is added to a map.
 * If your type of entity can never be an obstacle, you should redefine this
 * function and return \c false to avoid useless collision checks.
 *
 * \return \c true if this type of entity can be obstacle for other entities.
 */
bool MapEntity::can_be_obstacle() const {
  return true;
}

/**
 * \brief Returns whether this entity is sensible to the ground below it.
 *
 * This function returns \c false by default.
 * If this function returns \c true when it is added to a map,
 * get_ground_below() will then return the ground below it
 * and notify_ground_below_changed() will be called when it changes.
 *
 * \return \c true if this entity is sensible to its ground.
 */
bool MapEntity::is_ground_observer() const {
  return false;
}

/**
 * \brief Returns whether this entity can override the ground
 * of where they are placed.
 *
 * The ground of a point is computed as the ground of the tile below it,
 * possibly modified by entities overlapping the point and who redefine
 * can_change_ground() to \c true.
 *
 * This function returns \c false by default.
 * If this function returns \c true, the entity is added to the list of
 * potential ground modifiers when it is added to a map.
 *
 * \return \c true if this entity can change the ground.
 */
bool MapEntity::is_ground_modifier() const {
  return false;
}

/**
 * \brief When is_ground_modifier() is \c true, returns the ground defined
 * by this entity.
 *
 * Entities overlapping it should take it into account.
 *
 * \return The ground defined by this entity.
 */
Ground MapEntity::get_modified_ground() const {
  return GROUND_EMPTY;
}

/**
 * \brief Inform entities sensible to their ground that it may have just
 * changed because of this entity.
 */
void MapEntity::update_ground_observers() {

  if (!is_ground_modifier()) {
    // Nothing to do.
    return;
  }

  // Update overlapping entities sensible to their ground.
  std::list<MapEntity*>::const_iterator it;
  const std::list<MapEntity*>& ground_observers =
      get_entities().get_ground_observers(get_layer());
  for (it = ground_observers.begin(); it != ground_observers.end(); ++it) {
    MapEntity& ground_observer = *(*it);
    if (overlaps(ground_observer.get_ground_point())) {
      ground_observer.update_ground_below();
    }
  }
}

/**
 * \brief Returns the ground where this entity is.
 *
 * If the entity is not sensible to its ground, always returns GROUND_EMPTY.
 *
 * \return The ground under this entity.
 */
Ground MapEntity::get_ground_below() const {
  return ground_below;
}

/**
 * \brief Recomputes the kind of ground below this entity.
 *
 * This function does nothing if the entity is not sensible to its ground.
 */
void MapEntity::update_ground_below() {

  if (!is_ground_observer()) {
    // This entity does not care about the ground below it.
    return;
  }

  if (!is_enabled() || is_being_removed()) {
    return;
  }

  if (map->test_collision_with_border(get_ground_point())) {
    // If the entity is outside the map, which is legal during a scrolling
    // transition, don't try to determine any ground.
    return;
  }

  Ground previous_ground = this->ground_below;
  this->ground_below = get_map().get_ground(
      get_layer(), get_ground_point());
  if (this->ground_below != previous_ground) {
    notify_ground_below_changed();
  }
}

/**
 * \brief Returns whether entities of this type can be drawn.
 *
 * This function returns \c true by default. Redefine it to return
 * \c false if your type of entity has nothing to display.
 *
 * \return true if this type of entity can be drawn
 */
bool MapEntity::can_be_drawn() const {
  return true;
}

/**
 * \brief Returns whether this entity has to be drawn in y order.
 *
 * This function returns whether an entity of this type should be drawn above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are drawn in the normal order (i.e. in the order of their creation),
 * and before the entities with this feature.
 *
 * Returns \c false by default.
 *
 * \return \c true if this type of entity should be drawn at the same level
 * as the hero.
 */
bool MapEntity::is_drawn_in_y_order() const {
  return false;
}

/**
 * \brief Returns whether this entity is on a map, that is,
 * whether set_map() has been called.
 * \return true if the entity is on a map
 */
bool MapEntity::is_on_map() const {
  return map != NULL;
}

/**
 * \brief Sets the map where this entity is.
 *
 * Warning: when this function is called during the initialization of a new map,
 * the current map of the game is still the old one.
 *
 * TODO make this function non-virtual and make a virtual function notify_added_to_map(Map& map).
 *
 * \param map The map.
 */
void MapEntity::set_map(Map& map) {

  this->main_loop = &map.get_game().get_main_loop();
  this->map = &map;
  if (&get_game().get_current_map() == &map) {
    notify_tileset_changed();
  }

  this->ground_below = GROUND_EMPTY;
}

/**
 * \brief Notifies this entity that its map has just become active.
 */
void MapEntity::notify_map_started() {
}

/**
 * \brief Notifies this entity that the opening transition
 * of the map is finished.
 */
void MapEntity::notify_map_opening_transition_finished() {

  if (is_ground_observer()) {
    update_ground_below();
  }
}

/**
 * \brief Notifies this entity that the tileset of the map has just changed.
 *
 * This is useful for tileset-dependent sprites such as doors and blocks.
 */
void MapEntity::notify_tileset_changed() {

  std::vector<Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {

    Sprite& sprite = *(*it);
    sprite.set_tileset(get_map().get_tileset());
  }
}

/**
 * \brief Returns the map where this entity is.
 * \return the map
 */
Map& MapEntity::get_map() const {
  return *map;
}

/**
 * \brief Returns the game that is running the map where this entity is.
 * \return The game.
 */
Game& MapEntity::get_game() {
  SOLARUS_ASSERT(map != NULL, "No map was set");
  return map->get_game();
}

/**
 * \brief Returns the game that is running the map where this entity is.
 * \return The game.
 */
const Game& MapEntity::get_game() const {
  SOLARUS_ASSERT(map != NULL, "No map was set");
  return map->get_game();
}

/**
 * \brief Returns the entities of the current map.
 * \return The entities.
 */
MapEntities& MapEntity::get_entities() {
  Debug::check_assertion(map != NULL, "No map was set");
  return map->get_entities();
}

/**
 * \brief Returns the entities of the current map.
 * \return The entities.
 */
const MapEntities& MapEntity::get_entities() const {
  Debug::check_assertion(map != NULL, "No map was set");
  return map->get_entities();
}

/**
 * \brief Returns the shared Lua context.
 * \return The Lua context where all scripts are run.
 */
LuaContext& MapEntity::get_lua_context() {

  Debug::check_assertion(main_loop != NULL,
      "This entity is not fully constructed yet");
  return main_loop->get_lua_context();
}

/**
 * \brief Returns the current equipment.
 * \return The equipment.
 */
Equipment& MapEntity::get_equipment() {
  return get_game().get_equipment();
}

/**
 * \brief Returns the current equipment.
 * \return The equipment.
 */
const Equipment& MapEntity::get_equipment() const {
  return get_game().get_equipment();
}

/**
 * \brief Returns the keys effect manager.
 * \return the keys effect
 */
KeysEffect& MapEntity::get_keys_effect() {
  return get_game().get_keys_effect();
}

/**
 * \brief Returns the game commands.
 * \return The commands.
 */
GameCommands& MapEntity::get_commands() {
  return get_game().get_commands();
}

/**
 * \brief Returns the savegame.
 * \return The savegame.
 */
Savegame& MapEntity::get_savegame() {
  return get_game().get_savegame();
}

/**
 * \brief Returns the savegame.
 * \return The savegame.
 */
const Savegame& MapEntity::get_savegame() const {
  return get_game().get_savegame();
}

/**
 * \brief Returns the hero
 * \return the hero
 */
Hero& MapEntity::get_hero() {
  return get_entities().get_hero();
}

/**
 * \brief Schedules this entity for removal.
 *
 * The entity will be removed from the map and destroyed.
 * This function is equivalent to calling map->get_entities()->remove_entity(this).
 */
void MapEntity::remove_from_map() {

  get_map().get_entities().remove_entity(this);
}

/**
 * \brief Notifies this entity that it is about to be removed.
 *
 * This function is called when the entity has just been added
 * to the list of entities that will be removed from the map
 * and deleted from the memory as soon as possible.
 */
void MapEntity::notify_being_removed() {

  get_lua_context().entity_on_removed(*this);
  this->being_removed = true;

  // If this entity defines a ground, tell people that it is disappearing.
  update_ground_observers();
}

/**
 * \brief Returns the layer of the entity on the map.
 * \return the layer of the entity on the map
 */
Layer MapEntity::get_layer() const {
  return layer;
}

/**
 * \brief Sets the layer of the entity on the map.
 *
 * If the entity is stored on the map in the class MapEntities,
 * you should not call this function directly: call
 * MapEntities::set_entity_layer() instead because the class MapEntities
 * stores different lists of entities for each layer.
 * TODO: use notify_layer_changed to update MapEntities instead
 *
 * \param layer the layer of the entity on the map
 */
void MapEntity::set_layer(Layer layer) {

  this->layer = layer;
  notify_layer_changed();
}

/**
 * \brief This function is called when the layer of this entity has just changed.
 *
 * Redefine it if you need to be notified.
 */
void MapEntity::notify_layer_changed() {

  if (is_on_map()) {
    check_collision_with_detectors(true);
    update_ground_observers();
    update_ground_below();

    if (are_movement_notifications_enabled()) {
      get_lua_context().entity_on_position_changed(*this, get_xy(), get_layer());
    }
  }
}

/**
 * \brief This function is called when the ground below this entity has
 * just changed.
 */
void MapEntity::notify_ground_below_changed() {
}

/**
 * \brief Returns the direction of the entity.
 *
 * This direction is not used by all kinds of entities
 * since some of them only use the direction property of their sprites
 * and/or their movements.
 *
 * \return the direction of the entity
 */
int MapEntity::get_direction() const {
  return direction;
}

/**
 * \brief Sets the direction property of this entity.
 * \param direction the direction
 */
void MapEntity::set_direction(int direction) {
  this->direction = direction;
}

/**
 * \brief Returns the current x position of the entity.
 * \return the x position of the entity
 */
int MapEntity::get_x() const {
  return bounding_box.get_x() + origin.get_x();
}

/**
 * \brief Returns the current y position of the entity.
 * \return the y position of the entity
 */
int MapEntity::get_y() const {
  return bounding_box.get_y() + origin.get_y();
}

/**
 * \brief Sets the x position of the entity.
 *
 * This function is called by the movement object of this entity.
 *
 * \param x the new x position
 */
void MapEntity::set_x(int x) {
  bounding_box.set_x(x - origin.get_x());
}

/**
 * \brief Sets the y position of the entity.
 *
 * This function is called by the movement object of this entity.
 *
 * \param y the new y position
 */
void MapEntity::set_y(int y) {
  bounding_box.set_y(y - origin.get_y());
}

/**
 * \brief Returns the coordinates of the origin point of the entity, relative to the map.
 *
 * These are the coordinates of the point as returned by get_x() and get_y().
 *
 * \return the coordinates of the entity on the map
 */
const Rectangle MapEntity::get_xy() const {
  return Rectangle(get_x(), get_y());
}

/**
 * \brief Sets the coordinates of the origin point of the entity, relative to the map.
 *
 * This function sets the coordinates of the point as returned by get_x() and get_y().
 *
 * \param x the new x coordinate of the entity on the map
 * \param y the new y coordinate of the entity on the map
 */
void MapEntity::set_xy(int x, int y) {
  set_x(x);
  set_y(y);
}

/**
 * \brief Sets the coordinates of the origin point of the entity, relative to the map.
 * 
 * This function sets the coordinates of the point as returned by get_x() and get_y().
 *
 * \param xy the new coordinates of the entity on the map (the width and height are ignored)
 */
void MapEntity::set_xy(const Rectangle& xy) {
  set_xy(xy.get_x(), xy.get_y());
}

/**
 * \brief Returns the x position of the entity's top-left corner.
 * \return the x position of the entity's top-left corner
 */
int MapEntity::get_top_left_x() const {
  return bounding_box.get_x();
}

/**
 * \brief Returns the y position of the entity's top-left corner.
 * \return the y position of the entity's top-left corner
 */
int MapEntity::get_top_left_y() const {
  return bounding_box.get_y();
}

/**
 * \brief Sets the x position of the entity's top-left corner.
 * \param x the new top-left x position
 */
void MapEntity::set_top_left_x(int x) {
  bounding_box.set_x(x);
}

/**
 * \brief Sets the y position of the entity's top-left corner.
 * \param y the new top-left y position
 */
void MapEntity::set_top_left_y(int y) {
  bounding_box.set_y(y);
}

/**
 * \brief Sets the position of the entity.
 *
 * This function sets the coordinates of the rectangle's top-left corner.
 *
 * \param x x position of the entity
 * \param y y position of the entity
 */
void MapEntity::set_top_left_xy(int x, int y) {
  set_top_left_x(x);
  set_top_left_y(y);
}

/**
 * \brief Returns the coordinates where this entity should be drawn.
 *
 * Most of the time, this function just returns get_xy().
 * But when the entity is moving, the movement may decide to display the
 * entity at a different position.
 *
 * \return the coordinates of the entity on the map
 */
const Rectangle MapEntity::get_displayed_xy() const {

  if (get_movement() == NULL) {
    return get_xy();
  }

  return get_movement()->get_displayed_xy();
}

/**
 * \brief Returns the width of the entity.
 * \return the width of the entity
 */
int MapEntity::get_width() const {
  return bounding_box.get_width();
}

/**
 * \brief Returns the height of the entity.
 * \return the height of the entity
 */
int MapEntity::get_height() const {
  return bounding_box.get_height();
}

/**
 * \brief Returns the size of the entity.
 * \return a rectangle whose width and height represent the size of the entity
 * (its coordinates should be ignored)
 */
const Rectangle& MapEntity::get_size() const {
  return bounding_box;
}

/**
 * \brief Sets the size of the entity.
 * \param width the entity's width
 * \param height the entity's height
 */
void MapEntity::set_size(int width, int height) {
  bounding_box.set_size(width, height);
}

/**
 * \brief Sets the size of the entity.
 * \param size a rectangle having the width and height to set to the entity
 */
void MapEntity::set_size(const Rectangle &size) {
  bounding_box.set_size(size);
}

/**
 * \brief Returns the bounding box of the entity.
 *
 * This function returns the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 *
 * \return the position and size of the entity
 */
const Rectangle& MapEntity::get_bounding_box() const {
  return bounding_box;
}

/**
 * \brief Sets the bounding box of the entity.
 *
 * This function sets the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 *
 * \param bounding_box the new position and size of the entity
 */
void MapEntity::set_bounding_box(const Rectangle &bounding_box) {
  this->bounding_box = bounding_box;
}

/**
 * \brief Sets the origin point and the size of the entity like its sprite.
 *
 * You may call this function only if the entity's bounding box
 * is the same as the sprite's rectangle.
 * Otherwise, you have to call set_size() and set_origin()
 * explicitely.
 */
void MapEntity::set_bounding_box_from_sprite() {

  Sprite &sprite = get_sprite();
  set_size(sprite.get_size());
  set_origin(sprite.get_origin());
}

/**
 * \brief Returns whether the entity's bounding box is aligned with the 8*8 grid of the map.
 * \return true if the entity's bounding box is aligned
 */
bool MapEntity::is_aligned_to_grid() const {
  return is_aligned_to_grid_x() && is_aligned_to_grid_y();
}

/**
 * \brief Returns whether the entity's top-left corner is aligned
 * horizontally with the 8*8 grid of the map.
 * \return true if the entity's top-left corner is aligned hotizontally
 */
bool MapEntity::is_aligned_to_grid_x() const {
  return get_top_left_x() % 8 == 0;
}

/**
 * \brief Returns whether the entity's top-left corner is aligned
 * vertically with the 8*8 grid of the map.
 * \return true if the entity's top-left corner is aligned vertically
 */
bool MapEntity::is_aligned_to_grid_y() const {
  return get_top_left_y() % 8 == 0;
}

/**
 * \brief Makes the entity's top-left corner aligned with the 8*8 grid of the map.
 *
 * Be careful: This function does not check the collisions with obstacles.
 */
void MapEntity::set_aligned_to_grid() {

  set_aligned_to_grid_x();
  set_aligned_to_grid_y();
}

/**
 * \brief Makes the entity's top-left corner aligned horizontally
 * with the 8*8 grid of the map.
 *
 * Be careful: This function does not check the collisions with obstacles.
 */
void MapEntity::set_aligned_to_grid_x() {

  int x = get_top_left_x() + 4;
  x -= x % 8;
  set_top_left_x(x);
}

/**
 * \brief Makes the entity's top-left corner aligned vertically
 * with the 8*8 grid of the map.
 *
 * Be careful: This function does not check the collisions with obstacles.
 */
void MapEntity::set_aligned_to_grid_y() {

  int y = get_top_left_y() + 4;
  y -= y % 8;
  set_top_left_y(y);
}

/**
 * \brief Returns the coordinates of the point the entity is looking at.
 *
 * You should redefine this method to define what is a facing point for your subclass.
 *
 * \return the coordinates of the point the entity is looking at
 */
const Rectangle MapEntity::get_facing_point() const {
  return Rectangle(-1, -1);
}

/**
 * \brief Returns the coordinates of the point the entity would be facing
 * if it was looking towards the specified direction.
 *
 * You should redefine this method to define what is a facing point for your subclass.
 *
 * \param direction a direction (0 to 3)
 * \return the coordinates of the point the entity in this direction
 */
const Rectangle MapEntity::get_facing_point(int direction) const {
  return Rectangle(-1, -1);
}

/**
 * \brief Returns the detector in front of this entity.
 * \return the detector this entity is touching, or NULL if there is no detector in front of him
 */
Detector* MapEntity::get_facing_entity() {
  return facing_entity;
}

/**
 * \brief Returns the detector in front of this entity.
 * \return the detector this entity is touching, or NULL if there is no detector in front of him
 */
const Detector* MapEntity::get_facing_entity() const {
  return facing_entity;
}

/**
 * \brief Sets the entity this entity is currently facing.
 *
 * This function is called when this entity is facing a new detector.
 *
 * \param facing_entity the detector this entity is now facing (possibly NULL)
 */
void MapEntity::set_facing_entity(Detector* facing_entity) {
  this->facing_entity = facing_entity;
  notify_facing_entity_changed(facing_entity);
}

/**
 * \brief Notifies this entity that its facing entity has just changed.
 * \param facing_entity the detector this entity is now facing (possibly NULL)
 */
void MapEntity::notify_facing_entity_changed(Detector* facing_entity) {
}

/**
 * \brief Returns the point that determines the ground below this entity.
 *
 * By default, returns the coordinates of the entity.
 * The size of the rectangle returned must be 1x1.
 *
 * \return The point used to determine the ground (relative to the map).
 */
const Rectangle MapEntity::get_ground_point() const {

  return get_xy();
}

/**
 * \brief Returns the coordinates of the center point of the entity's rectangle.
 * \return the coordinates of the center point of the entity
 */
const Rectangle MapEntity::get_center_point() const {
  return bounding_box.get_center();
}

/**
 * \brief Returns the name of the entity (if any).
 * \return the name of the entity, or an empty string if
 * the entity has no name.
 */
const std::string& MapEntity::get_name() const {
  return name;
}

/**
 * \brief Returns whether the name of this entity starts with the specified prefix.
 * \param prefix a prefix
 * \return true if the name starts with this prefix
 */
bool MapEntity::has_prefix(const std::string& prefix) const {
  return name.substr(0, prefix.size()) == prefix;
}

/**
 * \brief Returns the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * \return the origin point
 */
const Rectangle& MapEntity::get_origin() const {
  return origin;
}

/**
 * \brief Sets the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * \param x x coordinate of the origin
 * \param y y coordinate of the origin
 */
void MapEntity::set_origin(int x, int y) {

  bounding_box.add_xy(origin.get_x() - x, origin.get_y() - y);
  origin.set_xy(x, y);
}

/**
 * \brief Sets the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * \param origin x and y coordinates of the origin
 */
void MapEntity::set_origin(const Rectangle& origin) {
  set_origin(origin.get_x(), origin.get_y());
}

/**
 * \brief Returns the optimization distance of this entity.
 *
 * Above this distance from the visible area, the entity is suspended.
 *
 * \return the optimization distance (0 means infinite)
 */
int MapEntity::get_optimization_distance() const {
  return optimization_distance;
}

/**
 * \brief Returns the square of the optimization distance of this entity.
 * \return Square of the optimization distance (0 means infinite).
 */
int MapEntity::get_optimization_distance2() const {
  return optimization_distance2;
}

/**
 * \brief Sets the optimization distance of this entity.
 *
 * Above this distance from the visible area, the entity is suspended.
 *
 * \param distance the optimization distance (0 means infinite)
 */
void MapEntity::set_optimization_distance(int distance) {
  this->optimization_distance = distance;
  this->optimization_distance2 = distance * distance;
}

/**
 * \brief Returns whether the entity has at least one sprite.
 * \return true if the entity has at least one sprite.
 */
bool MapEntity::has_sprite() const {
  return sprites.size() != 0;
}

/**
 * \brief Returns the sprite created with the first call to create_sprite()
 * for this entity.
 * \return The first sprite created.
 */
Sprite& MapEntity::get_sprite() {
  return *sprites.front();
}

/**
 * \brief Returns the const version of the sprite created with the first call
 * to create_sprite() for this entity.
 * \return The first sprite created.
 */
const Sprite& MapEntity::get_sprite() const {
  return *sprites.front();
}

/**
 * \brief Returns all sprites of this entity.
 * \return The sprites.
 */
const std::vector<Sprite*>& MapEntity::get_sprites() {
  return sprites;
}

/**
 * \brief Adds a sprite to this entity.
 * \param animation_set_id id of the sprite's animation set to use
 * \param enable_pixel_collisions true to enable the pixel-perfect collision tests for this sprite
 * \return the sprite created
 */
Sprite& MapEntity::create_sprite(const std::string& animation_set_id,
    bool enable_pixel_collisions) {

  Sprite* sprite = new Sprite(animation_set_id);
  RefCountable::ref(sprite);

  if (enable_pixel_collisions) {
    sprite->enable_pixel_collisions();
  }

  sprites.push_back(sprite);
  return *sprite;
}

/**
 * \brief Marks a sprite of this entity to be removed as soon as possible.
 */
void MapEntity::remove_sprite(Sprite& sprite) {

  bool found = false;
  std::vector<Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end() && !found; it++) {
    if (*it == &sprite) {
      old_sprites.push_back(&sprite);
      found = true;
    }
  }

  Debug::check_assertion(found, "This sprite does not belong to this entity");
}

/**
 * \brief Removes all sprites of this entity.
 *
 * They will be destroyed at next iteration.
 */
void MapEntity::clear_sprites() {

  std::vector<Sprite*>::const_iterator it;
  for (it = sprites.begin(); it != sprites.end(); ++it) {
    old_sprites.push_back(*it);
  }
  sprites.clear();
}

/**
 * \brief Really destroys the sprites that were recently removed.
 */
void MapEntity::clear_old_sprites() {

  std::vector<Sprite*>::const_iterator it;
  const std::vector<Sprite*>::const_iterator end = old_sprites.end();
  for (it = old_sprites.begin(); it != end; ++it) {
    Sprite* sprite = *it;
    std::vector<Sprite*>::iterator it2;
    for (it2 = sprites.begin(); it2 != sprites.end(); ++it2) {
      if (*it2 == sprite) {
        sprites.erase(it2);
        break;
      }
    }
    RefCountable::unref(sprite);
  }
  old_sprites.clear();
}

/**
 * \brief Notifies this entity that the frame of one of its sprites has just changed.
 *
 * By default, nothing is done.
 *
 * \param sprite the sprite
 * \param animation the current animation
 * \param frame the new frame
 */
void MapEntity::notify_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame) {
}

/**
 * \brief Notifies this entity that the animation of one of its sprites
 * has just finished.
 *
 * By default, nothing is done.
 *
 * \param sprite the sprite
 * \param animation the animation just finished
 */
void MapEntity::notify_sprite_animation_finished(Sprite& sprite, const std::string& animation) {
}

/**
 * \brief Returns whether this entity is currently visible.
 * \return true if this entity is currently visible
 */
bool MapEntity::is_visible() const {
  return visible;
}

/**
 * \brief Sets whether this entity is visible.
 * \param visible true to make it visible
 */
void MapEntity::set_visible(bool visible) {
  this->visible = visible;
}

/**
 * \brief Returns the current movement of the entity.
 * \return the entity's movement, or NULL if there is no movement
 */
Movement* MapEntity::get_movement() {
  return movement;
}

/**
 * \brief Returns the current movement of the entity.
 * \return the entity's movement, or NULL if there is no movement
 */
const Movement* MapEntity::get_movement() const {
  return movement;
}

/**
 * \brief Sets the movement of this entity.
 *
 * Once you have called this function, the pointer to the movement is managed by the entity only.
 * Never delete it from outside! The movement will be deleted if clear_movement() is called
 * or when the entity is destroyed.
 *
 * If a previous movement was already set, it is not deleted (so that you can reassign it later).
 * Thus, most of the time, you should call clear_movement() before set_movement() to avoid a memory leak.
 *
 * \param movement the movement to set, or NULL to set no movement
 */
void MapEntity::set_movement(Movement* movement) {

  this->movement = movement;
  RefCountable::ref(movement);

  if (movement != NULL) {
    movement->set_entity(this);

    if (movement->is_suspended() != suspended) {
      movement->set_suspended(suspended || !is_enabled());
    }
  }
}

/**
 * \brief Destroys the movement of this entity.
 *
 * The entity immediately stops moving.
 * The movement object will be destroyed at the next cycle,
 * thus this function can be called by the movement object itself.
 */
void MapEntity::clear_movement() {

  if (movement != NULL) {
    movement->set_entity(NULL);         // Tell the movement to forget me.
    movement->set_lua_context(NULL);    // Stop future Lua callbacks.
    old_movements.push_back(movement);  // Destroy it later.
    movement = NULL;
  }
}

/**
 * \brief Destroys the old movements of this entity.
 */
void MapEntity::clear_old_movements() {

  std::vector<Movement*>::const_iterator it;
  const std::vector<Movement*>::const_iterator end = old_movements.end();
  for (it = old_movements.begin(); it != end; ++it) {
    Movement* movement = *it;
    RefCountable::unref(movement);
  }
  old_movements.clear();
}

/**
 * \brief Returns whether Lua movement events are enabled for this entity.
 *
 * If no, events entity:on_position_changed(), entity:on_obstacle_reached(),
 * entity:on_movement_changed() and entity:on_movement_finished() won't be
 * called.
 *
 * \return Whether movement events are currently enabled.
 */
bool MapEntity::are_movement_notifications_enabled() const {
  return main_loop != NULL && movement_events_enabled;
}

/**
 * \brief Sets whether Lua movement events are enabled for this entity.
 *
 * If no, events entity:on_position_changed(), entity:on_obstacle_reached(),
 * entity:on_movement_changed() and entity:on_movement_finished() won't be
 * called.
 *
 * \param notify \c true to enable movement events.
 */
void MapEntity::set_movement_events_enabled(bool notify) {
  this->movement_events_enabled = notify;
}

/**
 * \brief Notifies this entity that it has just failed to change its position
 * because of obstacles.
 *
 * This function is called only when the movement is not suspended.
 */
void MapEntity::notify_obstacle_reached() {

  if (are_movement_notifications_enabled()) {
    get_lua_context().entity_on_obstacle_reached(*this, *get_movement());
  }
}

/**
 * \brief This function is called when the entity has just moved.
 *
 * It checks collisions with the detectors on the map
 * and, if this entity defines a ground, updates entities sensible to their
 * ground.
 */
void MapEntity::notify_position_changed() {

  check_collision_with_detectors(true);
  update_ground_observers();
  update_ground_below();

  if (are_movement_notifications_enabled()) {
    get_lua_context().entity_on_position_changed(*this, get_xy(), get_layer());
  }
}

/**
 * \brief Checks collisions between this entity and the detectors of the map.
 * \param with_pixel_precise true to include pixel-precise collisions (if any),
 * false to only check simple collisions
 */
void MapEntity::check_collision_with_detectors(bool with_pixel_precise) {

  if (!is_on_map()) {
    // The entity is still being initialized.
    return;
  }

  if (get_distance_to_camera2() > optimization_distance2
      && optimization_distance > 0) {
    // Don't check entities far for the visible area.
    return;
  }

  // Detect simple collisions.
  get_map().check_collision_with_detectors(*this);

  // Detect pixel-precise collisions.
  std::vector<Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {

    Sprite& sprite = *(*it);
    if (sprite.are_pixel_collisions_enabled()) {
      get_map().check_collision_with_detectors(*this, sprite);
    }
  }
}

/**
 * \brief Checks pixel-precise collisions between a particular sprite of this
 * entity and the detectors of the map.
 * \param sprite the sprite to check
 */
void MapEntity::check_collision_with_detectors(Sprite& sprite) {

  if (get_distance_to_camera2() > optimization_distance2
      && optimization_distance > 0) {
    // Don't check entities far for the visible area.
    return;
  }

  get_map().check_collision_with_detectors(*this, sprite);
}

/**
 * \brief This function can be called by the movement object
 * to notify the entity when the movement has just changed
 * (e.g. the speed, the angle or the trajectory).
 *
 * TODO: actually call this function from all movement subclasses
 */
void MapEntity::notify_movement_changed() {

  if (are_movement_notifications_enabled()) {
    get_lua_context().entity_on_movement_changed(*this, *get_movement());
  }
}

/**
 * \brief This function is called when the movement of the entity is finished.
 */
void MapEntity::notify_movement_finished() {

  if (are_movement_notifications_enabled()) {
    get_lua_context().entity_on_movement_finished(*this);
  }
}

/**
 * \brief This function is called when this entity starts being moved by
 * another one.
 */
void MapEntity::notify_moving_by(MapEntity& entity) {
  // Do nothing by default.
}

/**
 * \brief This function is called when this entity has just finished to get
 * moved by another one.
 */
void MapEntity::notify_moved_by(MapEntity& entity) {
  // Do nothing by default.
}

/**
 * \brief Converts a direction into the corresponding one-pixel move on x and y.
 * \param direction8 a direction (0 to 7)
 * \return a rectangle with x and y set to -1, 0 or 1 depending on the direction
 */
const Rectangle& MapEntity::direction_to_xy_move(int direction8) {
  return directions_to_xy_moves[direction8];
}

/**
 * \brief Enables or disables this entity.
 * \param enabled true to enable the entity, false to disable it
 */
void MapEntity::set_enabled(bool enabled) {

  if (enabled) {
    // enable the entity as soon as possible
    this->waiting_enabled = true;
  }
  else {
    this->enabled = false;
    this->waiting_enabled = false;

    if (get_movement() != NULL) {
      get_movement()->set_suspended(suspended || !enabled);
    }

    std::vector<Sprite*>::iterator it;
    for (it = sprites.begin(); it != sprites.end(); it++) {

      Sprite& sprite = *(*it);
      sprite.set_suspended(suspended || !enabled);
    }

    if (is_on_map()) {
      notify_enabled(enabled);
    }
  }
}

/**
 * \brief Notifies this entity that it was just enabled or disabled.
 * \param enabled \c true if the entity is now enabled.
 */
void MapEntity::notify_enabled(bool enabled) {

  update_ground_observers();
  update_ground_below();
}

/**
 * \brief Returns whether this entity is an obstacle for another one when
 * it is enabled.
 *
 * By default, this function returns false.
 *
 * \param other another entity
 * \return true if this entity is an obstacle for the other one
 */
bool MapEntity::is_obstacle_for(const MapEntity& other) const {
  return false;
}

/**
 * \brief Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 *
 * This function returns false by default.
 *
 * \return true if this entity can collide with entities that are on another layer
 */
bool MapEntity::has_layer_independent_collisions() const {
  return false;
}

/**
 * \brief Returns whether a low wall is currently considered as an obstacle
 * by this entity.
 *
 * This function returns \c true by default.
 *
 * \return \c true if low walls are currently obstacle for this entity.
 */
bool MapEntity::is_low_wall_obstacle() const {
  return true;
}

/**
 * \brief Returns whether shallow water is currently considered as an obstacle by this entity.
 *
 * This function returns is_deep_water_obstacle() by default.
 *
 * \return true if shallow water is currently an obstacle for this entity
 */
bool MapEntity::is_shallow_water_obstacle() const {
  return is_deep_water_obstacle();
}

/**
 * \brief Returns whether deep water is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if deep water is currently an obstacle for this entity
 */
bool MapEntity::is_deep_water_obstacle() const {
  return true;
}

/**
 * \brief Returns whether a hole is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if the holes are currently an obstacle for this entity
 */
bool MapEntity::is_hole_obstacle() const {
  return true;
}

/**
 * \brief Returns whether lava is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if lava is currently an obstacle for this entity
 */
bool MapEntity::is_lava_obstacle() const {
  return true;
}

/**
 * \brief Returns whether prickles are currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if prickles are currently an obstacle for this entity
 */
bool MapEntity::is_prickle_obstacle() const {
  return true;
}

/**
 * \brief Returns whether a ladder is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if the ladders are currently an obstacle for this entity
 */
bool MapEntity::is_ladder_obstacle() const {
  return true;
}

/**
 * \brief Returns whether the hero is currently considered as an obstacle by this entity.
 *
 * This function returns false by default.
 *
 * \param hero the hero
 * \return true if the hero is currently an obstacle for this entity
 */
bool MapEntity::is_hero_obstacle(const Hero& hero) const {
  return false;
}

/**
 * \brief Returns whether a block is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \param block a block
 * \return true if the teletransporter is currently an obstacle for this entity
 */
bool MapEntity::is_block_obstacle(const Block& block) const {
  return true;
}

/**
 * \brief Returns whether a teletransporter is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is currently an obstacle for this entity
 */
bool MapEntity::is_teletransporter_obstacle(
    const Teletransporter& teletransporter) const {
  return true;
}

/**
 * \brief Returns whether a conveyor belt is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \param conveyor_belt a conveyor belt
 * \return true if the conveyor belt is currently an obstacle for this entity
 */
bool MapEntity::is_conveyor_belt_obstacle(
    const ConveyorBelt& conveyor_belt) const {
  return true;
}

/**
 * \brief Returns whether some stairs are currently considered as an obstacle for this entity.
 *
 * This function returns true by default.
 *
 * \param stairs an stairs entity
 * \return true if the stairs are currently an obstacle for this entity
 */
bool MapEntity::is_stairs_obstacle(const Stairs& stairs) const {
  return true;
}

/**
 * \brief Returns whether a sensor is currently considered as an obstacle by this entity.
 *
 * This function returns false by default.
 *
 * \param sensor a sensor
 * \return true if the sensor is currently an obstacle for this entity
 */
bool MapEntity::is_sensor_obstacle(const Sensor& sensor) const {
  return false;
}

/**
 * \brief Returns whether a switch is currently considered as an obstacle by this entity.
 *
 * By default, this function returns true for solid switches and false for other ones.
 *
 * \param sw a switch
 * \return true if the switch is currently an obstacle for this entity
 */
bool MapEntity::is_switch_obstacle(const Switch& sw) const {
  return sw.is_solid();
}

/**
 * \brief Returns whether a raised crystal block is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \param raised_block a crystal block raised
 * \return true if the raised block is currently an obstacle for this entity
 */
bool MapEntity::is_raised_block_obstacle(
    const CrystalBlock& raised_block) const {
  return true;
}

/**
 * \brief Returns whether a crystal is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 * It should be redefined only for entities that can activate a crystal: a pot, the boomerang, etc.
 *
 * \param crystal a crystal
 * \return true if the crystal is currently an obstacle for this entity
 */
bool MapEntity::is_crystal_obstacle(const Crystal& crystal) const {
  return true;
}

/**
 * \brief Returns whether a non-playing character is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \param npc a non-playing character
 * \return true if the NPC is currently an obstacle for this entity
 */
bool MapEntity::is_npc_obstacle(const NPC& npc) const {
  return true;
}

/**
 * \brief Returns whether an enemy is currently considered as an obstacle by this entity.
 *
 * This function returns false by default.
 *
 * \param enemy an enemy
 * \return true if the enemy is currently an obstacle for this entity
 */
bool MapEntity::is_enemy_obstacle(const Enemy& enemy) const {
  return false;
}

/**
 * \brief Returns whether a non-diagonal jumper is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \param jumper a non-diagonal jumper
 * \return true if the jumper is currently an obstacle for this entity
 */
bool MapEntity::is_jumper_obstacle(const Jumper& jumper) const {
  return true;
}

/**
 * \brief Returns whether a destructible item is currently considered as an obstacle by this entity.
 *
 * By default, this function returns true unless the destructible item is disabled
 * (e.g. a bomb flower that will regenerate).
 *
 * \param destructible a destructible item
 * \return true if the destructible item is currently an obstacle for this entity
 */
bool MapEntity::is_destructible_obstacle(
    const Destructible& destructible) const {

  return !destructible.is_disabled();
}

/**
 * \brief Returns whether a separator is currently considered as an obstacle
 * by this entity.
 *
 * By default, this function returns \c true.
 *
 * \param separator A separator.
 * \return \c true if the separator is currently an obstacle for this entity.
 */
bool MapEntity::is_separator_obstacle(const Separator& separator) const {

  return true;
}

/**
 * \brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 *
 * \return true if the sword is ignored
 */
bool MapEntity::is_sword_ignored() const {
  return false;
}

/**
 * \brief Returns whether the bounding box or a sprite of this entity overlaps
 * the visible part of the map
 * \return true if the entity is in the visible part of the map
 */
bool MapEntity::overlaps_camera() const {

  if (bounding_box.overlaps(get_map().get_camera_position())) {
    return true;
  }

  bool found = false;
  std::vector<Sprite*>::const_iterator it;
  for (it = sprites.begin(); it != sprites.end() && !found; it++) {
    const Sprite* sprite = *it;
    const Rectangle& sprite_origin = sprite->get_origin();
    Rectangle sprite_bounding_box = sprite->get_size();
    sprite_bounding_box.set_xy(get_xy());
    sprite_bounding_box.add_xy(-sprite_origin.get_x(), -sprite_origin.get_y());
    found = sprite_bounding_box.overlaps(get_map().get_camera_position());
  }
  return found;
}

/**
 * \brief Returns whether or not this entity's origin point is in
 * the specified rectangle.
 * \param rectangle the rectangle to check
 * \return true if this entity's origin point is in the specified rectangle
 */
bool MapEntity::is_origin_point_in(const Rectangle& rectangle) const {
  return rectangle.contains(get_x(), get_y());
}

/**
 * \brief Returns whether this entity's facing point is in
 * the specified rectangle.
 * \param rectangle the rectangle to check
 * \return true if this entity's facing point is in the specified rectangle
 */
bool MapEntity::is_facing_point_in(const Rectangle& rectangle) const {

  const Rectangle& facing_point = get_facing_point();
  return rectangle.contains(facing_point.get_x(), facing_point.get_y());
}

/**
 * \brief Returns whether a facing point of this entity is in
 * the specified rectangle.
 * \param rectangle the rectangle to check
 * \param direction direction of the facing point to consider (0 to 3)
 * \return true if this facing point is in the specified rectangle
 */
bool MapEntity::is_facing_point_in(
    const Rectangle& rectangle, int direction) const {

  const Rectangle& facing_point = get_facing_point(direction);
  return rectangle.contains(facing_point.get_x(), facing_point.get_y());
}

/**
 * \brief Returns whether or not this entity's center is in
 * the specified rectangle.
 * \param rectangle the rectangle to check
 * \return true if this entity's center is in the specified rectangle
 */
bool MapEntity::is_center_in(const Rectangle& rectangle) const {

  const Rectangle& center = get_center_point();
  return rectangle.contains(center.get_x(), center.get_y());
}

/**
 * \brief Returns the angle of the vector between the origin of this entity
 * and a point.
 * \param x X coordinate of the point.
 * \param y Y coordinate of the point.
 * \return The angle of the vector in radians, between 0 and Geometry::TWO_PI.
 */
double MapEntity::get_angle(int x, int y) const {
  return Geometry::get_angle(get_x(), get_y(), x, y);
}

/**
 * \brief Returns the angle of the vector between the origin of this entity
 * and the origin of another entity.
 * \param other The other entity.
 * \return The angle of the vector in radians, between 0 and Geometry::TWO_PI.
 */
double MapEntity::get_angle(const MapEntity& other) const {
  return Geometry::get_angle(get_x(), get_y(), other.get_x(), other.get_y());
}

/**
 * \brief Returns the distance between the origin of this entity and a point.
 * \param x x coordinate of the point
 * \param y y coordinate of the point
 * \return the distance between this entity and the point in pixels
 */
int MapEntity::get_distance(int x, int y) const {
  return (int) Geometry::get_distance(get_x(), get_y(), x, y);
}

/**
 * \brief Returns the distance between the origin of this entity
 * and the origin of another entity.
 * \param other the other entity
 * \return the distance between the two entities in pixels
 */
int MapEntity::get_distance(const MapEntity& other) const {
  return (int) Geometry::get_distance(get_x(), get_y(), other.get_x(), other.get_y());
}

/**
 * \brief Returns the distance between the origin of this entity
 * and the center point of the visible part of the map.
 * \return the distance in pixels
 */
int MapEntity::get_distance_to_camera() const {

  const Rectangle& camera = get_map().get_camera_position();
  return (int) Geometry::get_distance(
      get_x(),
      get_y(),
      camera.get_x() + camera.get_width() / 2,
      camera.get_y() + camera.get_height() / 2
  );
}

/**
 * \brief Returns the square of the distance between the origin of this entity
 * and the center point of the visible part of the map.
 * \return Square of the distance.
 */
int MapEntity::get_distance_to_camera2() const {

  const Rectangle& camera = get_map().get_camera_position();
  return Geometry::get_distance2(
      get_x(),
      get_y(),
      camera.get_x() + camera.get_width() / 2,
      camera.get_y() + camera.get_height() / 2
  );
}

/**
 * \brief Returns whether an entity is in the same region as this one.
 *
 * Regions are defined by the position of separators on the map.
 *
 * \param other Another entity.
 * \return \c true if both entities are in the same region.
 */
bool MapEntity::is_in_same_region(const MapEntity& other) const {

  const std::list<const Separator*>& separators = get_entities().get_separators();
  std::list<const Separator*>::const_iterator it;
  for (it = separators.begin(); it != separators.end(); ++it) {

    const Separator& separator = *(*it);
    if (separator.is_vertical()) {
      // Vertical separation.
      const int separation_x = separator.get_center_point().get_x();
      if (get_x() < separation_x && separation_x <= other.get_x()) {
        return false;
      }

      if (other.get_x() < separation_x && separation_x <= get_x()) {
        return false;
      }
    }
    else {
      // Horizontal separation.
      const int separation_y = separator.get_center_point().get_y();
      if (get_y() < separation_y && separation_y <= other.get_y()) {
        return false;
      }

      if (other.get_y() < separation_y && separation_y <= get_y()) {
        return false;
      }
    }
  }

  return true;
}

/**
 * \brief This function is called when a destructible item detects a non-pixel perfect collision with this entity.
 * \param destructible the destructible item
 * \param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_destructible(Destructible &destructible, CollisionMode collision_mode) {
}

/**
 * \brief This function is called when a teletransporter detects a collision with this entity.
 * \param teletransporter the teletransporter
 * \param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_teletransporter(Teletransporter &teletransporter, CollisionMode collision_mode) {
}

/**
 * \brief This function is called when a conveyor belt detects a collision with this entity.
 * \param conveyor_belt a conveyor belt
 * \param dx direction of the x move in pixels (0, 1 or -1)
 * \param dy direction of the y move in pixels (0, 1 or -1)
 */
void MapEntity::notify_collision_with_conveyor_belt(ConveyorBelt &conveyor_belt, int dx, int dy) {
}

/**
 * \brief This function is called when a stairs entity detects a collision with this entity.
 * \param stairs the stairs
 * \param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_stairs(Stairs &stairs, CollisionMode collision_mode) {
}

/**
 * \brief This function is called when a jumper detects a collision with this entity.
 * \param jumper The jumper.
 * \param collision_mode The collision mode that detected the event.
 */
void MapEntity::notify_collision_with_jumper(Jumper& jumper, CollisionMode collision_mode) {
}

/**
 * \brief This function is called when a sensor detects a collision with this entity.
 * \param sensor a sensor
 * \param collision_mode the collision mode that detected the collision
 */
void MapEntity::notify_collision_with_sensor(Sensor &sensor, CollisionMode collision_mode) {
}

/**
 * \brief This function is called when a switch detects a collision with this entity.
 * \param sw a switch
 * \param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_switch(Switch& sw,
    CollisionMode collision_mode) {
}

/**
 * \brief This function is called when the sprite of a switch
 * detects a pixel-precise collision with a sprite of this entity.
 * \param sw the switch
 * \param sprite_overlapping the sprite of the current entity that collides with the switch
 */
void MapEntity::notify_collision_with_switch(Switch& sw, Sprite& sprite_overlapping) {
}

/**
 * \brief This function is called when a crystal detects a collision with this entity.
 * \param crystal the crystal
 * \param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_crystal(Crystal &crystal,
    CollisionMode collision_mode) {
}

/**
 * \brief This function is called when the sprite of a crystal
 * detects a pixel-precise collision with a sprite of this entity.
 * \param crystal the crystal
 * \param sprite_overlapping the sprite of the current entity that collides with the crystal
 */
void MapEntity::notify_collision_with_crystal(Crystal &crystal, Sprite &sprite_overlapping) {
}

/**
 * \brief This function is called when a chest detects a collision with this entity.
 * \param chest the chest
 */
void MapEntity::notify_collision_with_chest(Chest& chest) {
}

/**
 * \brief This function is called when a block detects a collision with this entity.
 * \param block the block
 */
void MapEntity::notify_collision_with_block(Block& block) {
}

/**
 * \brief This function is called when a separator detects a collision with
 * this entity.
 * \param separator The separator.
 * \param collision_mode The collision mode that detected the event.
 */
void MapEntity::notify_collision_with_separator(
    Separator& separator, CollisionMode collision_mode) {

}

/**
 * \brief This function is called when bomb detects a collision with a this entity.
 * \param bomb the bomb
 * \param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_bomb(Bomb& bomb, CollisionMode collision_mode) {
}

/**
 * \brief This function is called when an explosion detects a collision with this entity.
 * \param explosion the explosion
 * \param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_explosion(Explosion& explosion,
    CollisionMode collision_mode) {
}

/**
 * \brief This function is called when an explosion's sprite
 * detects a pixel-perfect collision with a sprite of this entity.
 * \param explosion the explosion
 * \param sprite_overlapping the sprite of the current entity that collides with the explosion
 */
void MapEntity::notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping) {
}

/**
 * \brief Notifies this entity that a sprite of fire
 * detects a pixel-perfect collision with a sprite of this entity.
 * \param fire the fire
 * \param sprite_overlapping the sprite of the current entity that collides with the fire
 */
void MapEntity::notify_collision_with_fire(Fire& fire, Sprite& sprite_overlapping) {

}

/**
 * \brief This function is called when an enemy's rectangle detects a collision with this entity's rectangle.
 * \param enemy the enemy
 */
void MapEntity::notify_collision_with_enemy(Enemy &enemy) {
}

/**
 * \brief This function is called when an enemy's sprite collides with a sprite of this entity.
 * \param enemy the enemy
 * \param enemy_sprite the enemy's sprite that overlaps a sprite of this entity
 * \param this_sprite this entity's sprite that overlaps the enemy's sprite
 */
void MapEntity::notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite) {
}

/**
 * \brief Notifies this entity that it has just attacked an enemy.
 *
 * This function is called even if this attack was not successful.
 *
 * \param attack the attack
 * \param victim the enemy just attacked
 * \param result indicates how the enemy reacted to the attack
 * \param killed indicates that the attack has just killed the enemy
 */
void MapEntity::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {
}

/**
 * \brief Returns whether the movement and the animations of this entity are suspended.
 * \return true if the movement and the animations are suspended
 */
bool MapEntity::is_suspended() const {
  return suspended;
}

/**
 * \brief Suspends or resumes the movement and the animations of this entity.
 * \param suspended true to suspend the movement and the animations, false to resume them
 */
void MapEntity::set_suspended(bool suspended) {

  this->suspended = suspended;

  // remember the date if the entity is being suspended
  if (suspended) {
    when_suspended = System::now();
  }

  // suspend/unsuspend the sprites animations
  std::vector<Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {

    Sprite& sprite = *(*it);
    sprite.set_suspended(suspended || !is_enabled());
  }

  // suspend/unsuspend the movement
  if (movement != NULL) {
    movement->set_suspended(suspended || !is_enabled());
  }
}

/**
 * \brief Returns the date when this entity was suspended.
 * \return When this entity was suspended.
 */
uint32_t MapEntity::get_when_suspended() const {
  return when_suspended;
}

/**
 * \brief Makes this entity's sprites play their animation even when the game is suspended.
 * \param ignore_suspend true to keep playing the sprites when the game is suspended
 */
void MapEntity::set_animation_ignore_suspend(bool ignore_suspend) {

  std::vector<Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {

    Sprite& sprite = *(*it);
    sprite.set_ignore_suspend(ignore_suspend);
  }
}

/**
 * \brief Updates the entity.
 *
 * This function is called repeatedly by the map. By default, it updates the position
 * of the entity according to its movement (if any), and it updates the sprites frames
 * if there are sprites.
 * Redefine it in subclasses for the entities that should be updated
 * for other treatments but don't forget to call this method
 * to handle the movement and the sprites.
 */
void MapEntity::update() {

  // Static tiles are optimized and should never be used individually
  // once the map is created.
  SOLARUS_ASSERT(get_type() != ENTITY_TILE,
      "Attempt to update a static tile");

  // enable if necessary
  if (waiting_enabled) {
    Hero& hero = get_hero();
    if (!is_obstacle_for(hero) || !overlaps(hero)) {
      this->enabled = true;
      this->waiting_enabled = false;
      notify_enabled(true);

      if (get_movement() != NULL) {
        get_movement()->set_suspended(suspended || !enabled);
      }

      std::vector<Sprite*>::iterator it;
      for (it = sprites.begin(); it != sprites.end(); it++) {

        Sprite& sprite = *(*it);
        sprite.set_suspended(suspended || !enabled);
      }
    }
  }

  // check the facing entity
  if (facing_entity != NULL && facing_entity->is_being_removed()) {
    set_facing_entity(NULL);
  }

  // update the sprites
  std::vector<Sprite*>::const_iterator it;
  const std::vector<Sprite*>::const_iterator sprites_end = sprites.end();
  for (it = sprites.begin(); it != sprites_end; ++it) {

    Sprite& sprite = *(*it);
    sprite.update();
    if (sprite.has_frame_changed()) {

      if (sprite.are_pixel_collisions_enabled()) {
        check_collision_with_detectors(sprite);
      }

      notify_sprite_frame_changed(sprite, sprite.get_current_animation(), sprite.get_current_frame());
      if (sprite.is_animation_finished()) {
        notify_sprite_animation_finished(sprite, sprite.get_current_animation());
      }
    }
  }
  clear_old_sprites();

  // update the movement
  if (movement != NULL) {
    movement->update();
  }
  clear_old_movements();

  // suspend the entity if far from the camera
  bool far = get_distance_to_camera2() > optimization_distance2
      && optimization_distance > 0;
  if (far && !is_suspended()) {
    set_suspended(true);
  }
  else if (!far && is_suspended() && !get_game().is_suspended()) {
    set_suspended(false);
  }
}

/**
 * \brief Returns whether this entity should be drawn on the map.
 * \return true if the entity is visible and may have a sprite in the visible part
 * of the map.
 */
bool MapEntity::is_drawn() const {

  return is_visible()
      && (overlaps_camera()
          || get_distance_to_camera2() < optimization_distance2
          || !is_drawn_at_its_position()
      );
}

/**
 * \brief Returns whether this entity is drawn at its position on the map.
 *
 * Usually, this function returns true, and when it is the case, draw_on_map()
 * is called only for entities located in the camera's rectangle.
 *
 * However, some entities may want to be drawn in the camera even when their
 * position is outside, typically to make an illusion of movement like parallax
 * scrolling.
 *
 * \return true if this entity is drawn where it is located.
 */
bool MapEntity::is_drawn_at_its_position() const {
  return true;
}

/**
 * \brief Draws the entity on the map.
 *
 * By default, this function draws the entity's sprites (if any) and if
 * at least one of them is in the visible part of the map.
 * This function should do nothing if is_drawn() is false.
 */
void MapEntity::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  // Draw the sprites.
  std::vector<Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); ++it) {
    Sprite& sprite = *(*it);
    get_map().draw_sprite(sprite, get_displayed_xy());
  }
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& MapEntity::get_lua_type_name() const {
  return LuaContext::entity_module_name;
}

/**
 * \brief Returns the Lua name of this entity type.
 */
const std::string& MapEntity::get_type_name() const {

  return entity_type_names[get_type()];
}
