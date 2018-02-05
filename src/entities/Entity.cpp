/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/core/Game.h"
#include "solarus/core/Geometry.h"
#include "solarus/core/MainLoop.h"
#include "solarus/core/Map.h"
#include "solarus/core/System.h"
#include "solarus/entities/CollisionMode.h"
#include "solarus/entities/Destructible.h"
#include "solarus/entities/Door.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/Entity.h"
#include "solarus/entities/EntityState.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Npc.h"
#include "solarus/entities/Separator.h"
#include "solarus/entities/SeparatorPtr.h"
#include "solarus/entities/StreamAction.h"
#include "solarus/entities/Switch.h"
#include "solarus/entities/Tileset.h"
#include "solarus/graphics/Sprite.h"
#include "solarus/graphics/SpriteAnimationSet.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/Movement.h"
#include <algorithm>
#include <iterator>
#include <list>
#include <utility>

namespace Solarus {

/**
 * \brief Creates an entity, specifying its position, its name and its direction.
 * \param name Name identifying the entity on the map or an empty string.
 * \param direction Direction of the entity.
 * \param layer Layer of the entity.
 * \param xy Coordinates where to create the entity.
 * \param size Size of the entity.
 */
Entity::Entity(
    const std::string& name,
    int direction,
    int layer,
    const Point& xy,
    const Size& size
):
  main_loop(nullptr),
  map(nullptr),
  layer(layer),
  bounding_box(xy, size),
  ground_below(Ground::EMPTY),
  origin(0, 0),
  name(name),
  direction(direction),
  user_properties(),
  sprites(),
  default_sprite_name(),
  visible(true),
  drawn_in_y_order(false),
  movement(nullptr),
  movement_notifications_enabled(true),
  facing_entity(nullptr),
  collision_modes(CollisionMode::COLLISION_NONE),
  layer_independent_collisions(false),
  stream_action(nullptr),
  initialized(false),
  being_removed(false),
  enabled(true),
  suspended(false),
  when_suspended(0),
  optimization_distance(default_optimization_distance),
  optimization_distance2(default_optimization_distance * default_optimization_distance) {

  Debug::check_assertion(size.width % 8 == 0 && size.height % 8 == 0,
      "Invalid entity size: width and height must be multiple of 8");
}

/**
 * \brief Destructor.
 *
 * The sprite and the movement of the entity are deleted (if any).
 */
Entity::~Entity() {

  stop_stream_action();

  clear_sprites();
  clear_old_sprites();
  clear_movement();
  clear_old_movements();
}

/**
 * \brief Returns the name identifying this type in Lua.
 *
 * This is the internal name of the metatable for the entity.
 *
 * \return The name identifying this type in Lua.
 */
const std::string& Entity::get_lua_type_name() const {
  return LuaContext::get_entity_internal_type_name(get_type());
}

/**
 * \brief Returns whether this entity is the hero controlled by the player.
 * \return true if this entity is the hero
 */
bool Entity::is_hero() const {
  return get_type() == EntityType::HERO;
}

/**
 * \brief Returns whether this entity is sensible to the ground below it.
 *
 * This function returns \c false by default.
 * If this function returns \c true,
 * notify_ground_below_changed() will be called when it changes.
 *
 * \return \c true if this entity is sensible to its ground.
 */
bool Entity::is_ground_observer() const {
  return false;
}

/**
 * \brief Returns whether this entity overrides the ground
 * of where it is placed.
 *
 * This is equivalent to get_modified_ground() != Ground::EMPTY.
 *
 * \return \c true if this entity changes the ground.
 */
bool Entity::is_ground_modifier() const {
  return get_modified_ground() != Ground::EMPTY;
}

/**
 * \brief Returns the ground defined by this entity.
 *
 * If a value other than Ground::EMPTY is returned, it means that this
 * entity modifies the ground of the map where it is placed.
 *
 * \return The ground defined by this entity.
 */
Ground Entity::get_modified_ground() const {
  return Ground::EMPTY;
}

/**
 * \brief Inform entities sensible to their ground that it may have just
 * changed because of this entity.
 *
 * This does the work even if this entity is not a ground modifier,
 * because this is necessary in case it just stopped being one.
 */
void Entity::update_ground_observers() {

  // Update overlapping entities that are sensible to their ground.
  const Rectangle& box = get_bounding_box();
  std::vector<EntityPtr> entities_nearby;
  get_entities().get_entities_in_rectangle(box, entities_nearby);
  for (const EntityPtr& entity_nearby: entities_nearby) {

    if (!entity_nearby->is_ground_observer()) {
      // The entity does not care about the ground below it.
      continue;
    }

    // Update the ground of observers that overlap or were just overlapping this one.
    if (entity_nearby->get_layer() == get_layer() &&
        (overlaps(entity_nearby->get_ground_point()) || overlaps(*entity_nearby))
        // FIXME this is not precise and does not work for entities that disappear.
    ) {
      entity_nearby->update_ground_below();
    }
  }
}

/**
 * \brief Returns the ground where this entity is.
 * \return The ground under this entity.
 */
Ground Entity::get_ground_below() const {

  if (is_ground_observer()) {
    // The information is already known.
    return ground_below;
  }

  return get_map().get_ground(
      get_layer(), get_ground_point(), this
  );
}

/**
 * \brief Recomputes the kind of ground below this entity.
 *
 * This function does nothing if the entity is not sensible to its ground.
 */
void Entity::update_ground_below() {

  if (!is_ground_observer()) {
    // This entity does not care about the ground below it.
    return;
  }

  if (!is_enabled() ||
      is_being_removed()) {
    return;
  }
  // Note that even if the entity is suspended,
  // the user might want to know the ground below it.

  if (map->test_collision_with_border(get_ground_point())) {
    // If the entity is outside the map, which is legal during a scrolling
    // transition, don't try to determine any ground.
    return;
  }

  Ground previous_ground = this->ground_below;
  this->ground_below = get_map().get_ground(
      get_layer(), get_ground_point(), this
  );
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
bool Entity::can_be_drawn() const {
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
 * \return \c true if this type of entity should be drawn at the same level
 * as the hero.
 */
bool Entity::is_drawn_in_y_order() const {
  return drawn_in_y_order;
}

/**
 * \brief Sets whether this entity should be drawn in y order.
 * \param drawn_in_y_order \c true to draw this entity at the same level
 * as the hero.
 */
void Entity::set_drawn_in_y_order(bool drawn_in_y_order) {
  this->drawn_in_y_order = drawn_in_y_order;
}

/**
 * \brief This function is called when a game command is pressed
 * and the game is not suspended.
 * \param command The command pressed.
 */
void Entity::notify_command_pressed(GameCommand /* game_command */) {
}

/**
 * \brief This function is called when a game command is released
 * if the game is not suspended.
 * \param command The command released.
 */
void Entity::notify_command_released(GameCommand /* game_command */) {
}

/**
 * \brief Returns whether this entity is on a map, that is,
 * whether set_map() has been called.
 * \return true if the entity is on a map
 */
bool Entity::is_on_map() const {
  return map != nullptr && map->is_loaded();
}

/**
 * \brief Sets the map where this entity is.
 *
 * Warning: when this function is called during the initialization of a new map,
 * the current map of the game is still the old one.
 *
 * \param map The map.
 */
void Entity::set_map(Map& map) {

  this->main_loop = &map.get_game().get_main_loop();
  this->map = &map;
  set_lua_context(&main_loop->get_lua_context());
  if (&get_game().get_current_map() == &map) {
    notify_tileset_changed();
  }

  this->ground_below = Ground::EMPTY;

  if (!initialized && map.is_loaded()) {
    // The entity is being created on a map already running.
    // In this case, we are ready to finish the initialization right now.
    finish_initialization();
  }
}

/**
 * @brief Returns whether this entity is fully initialized.
 *
 * This becomes true after entity:on_created() is called.
 *
 * @return @c true if the entity is initialized.
 */
bool Entity::is_initialized() const {
  return initialized;
}

/**
 * \brief Notifies this entity that its map has just started.
 *
 * The map was being loaded and is now ready.
 */
void Entity::notify_map_started() {

  if (!initialized) {
    // The map is now ready: we can finish the initialization of the entity.
    finish_initialization();
  }
}

/**
 * \brief Finishes the initialization of this entity once the map is loaded.
 *
 * This function must be called once the map is ready.
 * It calls notify_creating(), then the Lua event entity:on_created(),
 * and then notify_created().
 */
void Entity::finish_initialization() {

  Debug::check_assertion(!initialized, "Entity is already initialized");
  Debug::check_assertion(is_on_map(), "Missing map");
  Debug::check_assertion(get_map().is_loaded(), "Map is not ready");

  notify_creating();
  get_lua_context()->entity_on_created(*this);
  notify_created();

  initialized = true;
}

/**
 * \brief Notifies this entity that it is being created on a map.
 *
 * At this point, the map is already loaded and running,
 * including if the entity was created from the map data file.
 * Here, you can perform initializations that your entity need to do once the
 * map is known and running.
 * The entity:on_created() event will be called after this.
 */
void Entity::notify_creating() {

}

/**
 * \brief Notifies this entity that it has just been created on a map.
 *
 * At this point, the map is already loaded and running,
 * including if the entity was created from the map data file.
 * The entity:on_created() event has just been called.
 * You can redefine this function to perform additional initializations
 * after entity:on_created().
 */
void Entity::notify_created() {

  if (state != nullptr) {
    get_lua_context()->entity_on_state_changed(*this, get_state_name());
  }
}

/**
 * \brief Notifies this entity that the opening transition
 * of the map is finished.
 */
void Entity::notify_map_opening_transition_finished() {

  if (is_ground_observer()) {
    update_ground_below();
  }
}

/**
 * \brief Notifies this entity that the tileset of the map has just changed.
 *
 * This is useful for tileset-dependent sprites such as doors and blocks.
 */
void Entity::notify_tileset_changed() {

  for (const SpritePtr& sprite: get_sprites()) {
    sprite->set_tileset(get_map().get_tileset());
  }
}

/**
 * \brief Returns the map where this entity is.
 * \return the map
 */
Map& Entity::get_map() const {
  return *map;
}

/**
 * \brief Returns the game that is running the map where this entity is.
 * \return The game.
 */
Game& Entity::get_game() {
  SOLARUS_ASSERT(map != nullptr, "No map was set");
  return map->get_game();
}

/**
 * \brief Returns the game that is running the map where this entity is.
 * \return The game.
 */
const Game& Entity::get_game() const {
  SOLARUS_ASSERT(map != nullptr, "No map was set");
  return map->get_game();
}

/**
 * \brief Returns the entities of the current map.
 * \return The entities.
 */
Entities& Entity::get_entities() {
  Debug::check_assertion(map != nullptr, "No map was set");
  return map->get_entities();
}

/**
 * \brief Returns the entities of the current map.
 * \return The entities.
 */
const Entities& Entity::get_entities() const {
  Debug::check_assertion(map != nullptr, "No map was set");
  return map->get_entities();
}

/**
 * \brief Returns the current equipment.
 * \return The equipment.
 */
Equipment& Entity::get_equipment() {
  return get_game().get_equipment();
}

/**
 * \brief Returns the current equipment.
 * \return The equipment.
 */
const Equipment& Entity::get_equipment() const {
  return get_game().get_equipment();
}

/**
 * \brief Returns the keys effect manager.
 * \return the keys effect
 */
CommandsEffects& Entity::get_commands_effects() {
  return get_game().get_commands_effects();
}

/**
 * \brief Returns the game commands.
 * \return The commands.
 */
GameCommands& Entity::get_commands() {
  return get_game().get_commands();
}

/**
 * \brief Returns the savegame.
 * \return The savegame.
 */
Savegame& Entity::get_savegame() {
  return get_game().get_savegame();
}

/**
 * \brief Returns the savegame.
 * \return The savegame.
 */
const Savegame& Entity::get_savegame() const {
  return get_game().get_savegame();
}

/**
 * \brief Returns the hero
 * \return The hero.
 */
Hero& Entity::get_hero() {
  return get_entities().get_hero();
}

/**
 * \brief Schedules this entity for removal.
 *
 * The entity will be removed from the map and destroyed.
 * This function is equivalent to calling map->get_entities()->remove_entity(this).
 */
void Entity::remove_from_map() {

  get_entities().remove_entity(*this);
}

/**
 * \brief Notifies this entity that it is about to be removed.
 *
 * This function is called when the entity has just been added
 * to the list of entities that will be removed from the map
 * and deleted from the memory as soon as possible.
 */
void Entity::notify_being_removed() {

  get_lua_context()->entity_on_removed(*this);
  this->being_removed = true;

  // If this entity defines a ground, tell people that it is disappearing.
  if (is_on_map() &&
      map->is_started() &&
      is_ground_modifier()
  ) {
    update_ground_observers();
  }

  if (get_hero().get_facing_entity() == this) {
    get_hero().set_facing_entity(nullptr);
  }
}

/**
 * \brief Returns the layer of the entity on the map.
 * \return The layer of the entity on the map.
 */
int Entity::get_layer() const {
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
 * \param layer The layer of the entity on the map.
 */
void Entity::set_layer(int layer) {

  this->layer = layer;
  notify_layer_changed();
}

/**
 * \brief This function is called when the layer of this entity has just changed.
 *
 * Redefine it if you need to be notified.
 */
void Entity::notify_layer_changed() {

  if (!is_on_map()) {
    return;
  }

  if (is_detector()) {
    // Since this entity is a detector, all entities need to check
    // their collisions with it.
    get_map().check_collision_from_detector(*this);
  }

  // Check collisions between this entity and other detectors.
  check_collision_with_detectors();

  // Update the ground.
  if (is_ground_modifier()) {
    update_ground_observers();
  }
  update_ground_below();

  // Notify Lua.
  if (are_movement_notifications_enabled()) {
    get_lua_context()->entity_on_position_changed(*this, get_xy(), get_layer());
  }
}

/**
 * \brief This function is called when the ground below this entity has
 * just changed.
 */
void Entity::notify_ground_below_changed() {
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
int Entity::get_direction() const {
  return direction;
}

/**
 * \brief Sets the direction property of this entity.
 * \param direction the direction
 */
void Entity::set_direction(int direction) {

  if (direction != this->direction) {
    this->direction = direction;
    notify_direction_changed();
  }
}

/**
 * \brief Notifies this entity that its direction property has changed.
 */
void Entity::notify_direction_changed() {
}

/**
 * \brief Returns the current x position of the entity.
 * \return the x position of the entity
 */
int Entity::get_x() const {
  return bounding_box.get_x() + origin.x;
}

/**
 * \brief Returns the current y position of the entity.
 * \return the y position of the entity
 */
int Entity::get_y() const {
  return bounding_box.get_y() + origin.y;
}

/**
 * \brief Sets the x position of the entity.
 *
 * This function is called by the movement object of this entity.
 *
 * \param x the new x position
 */
void Entity::set_x(int x) {
  bounding_box.set_x(x - origin.x);
}

/**
 * \brief Sets the y position of the entity.
 *
 * This function is called by the movement object of this entity.
 *
 * \param y the new y position
 */
void Entity::set_y(int y) {
  bounding_box.set_y(y - origin.y);
}

/**
 * \brief Returns the coordinates of the origin point of the entity, relative to the map.
 *
 * These are the coordinates of the point as returned by get_x() and get_y().
 *
 * \return the coordinates of the entity on the map
 */
Point Entity::get_xy() const {
  return { get_x(), get_y() };
}

/**
 * \brief Sets the coordinates of the origin point of the entity, relative to the map.
 *
 * This function sets the coordinates of the point as returned by get_x() and get_y().
 *
 * \param x the new x coordinate of the entity on the map
 * \param y the new y coordinate of the entity on the map
 */
void Entity::set_xy(int x, int y) {
  set_x(x);
  set_y(y);
}

/**
 * \brief Sets the coordinates of the origin point of the entity, relative to the map.
 *
 * This function sets the coordinates of the point as returned by get_x() and get_y().
 *
 * \param xy the new coordinates of the entity on the map.
 */
void Entity::set_xy(const Point& xy) {
  set_xy(xy.x, xy.y);
}

/**
 * \brief Returns the x position of the entity's top-left corner.
 * \return the x position of the entity's top-left corner
 */
int Entity::get_top_left_x() const {
  return bounding_box.get_x();
}

/**
 * \brief Returns the y position of the entity's top-left corner.
 * \return the y position of the entity's top-left corner
 */
int Entity::get_top_left_y() const {
  return bounding_box.get_y();
}

/**
 * \brief Returns the position of the entity's top-left corner.
 * \return The position of the entity's top-left corner.
 */
Point Entity::get_top_left_xy() const {
  return { get_top_left_x(), get_top_left_y() };
}

/**
 * \brief Sets the x position of the entity's top-left corner.
 * \param x the new top-left x position
 */
void Entity::set_top_left_x(int x) {
  bounding_box.set_x(x);
}

/**
 * \brief Sets the y position of the entity's top-left corner.
 * \param y the new top-left y position
 */
void Entity::set_top_left_y(int y) {
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
void Entity::set_top_left_xy(int x, int y) {
  set_top_left_x(x);
  set_top_left_y(y);
}

/**
 * \brief Sets the position of the entity's top-left corner.
 * \param xy The new top-left position.
 */
void Entity::set_top_left_xy(const Point& xy) {
  set_top_left_xy(xy.x, xy.y);
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
 Point Entity::get_displayed_xy() const {

  if (get_movement() == nullptr) {
    return get_xy();
  }

  return get_movement()->get_displayed_xy();
}

/**
 * \brief Returns the width of the entity.
 * \return the width of the entity
 */
int Entity::get_width() const {
  return bounding_box.get_width();
}

/**
 * \brief Returns the height of the entity.
 * \return the height of the entity
 */
int Entity::get_height() const {
  return bounding_box.get_height();
}

/**
 * \brief Returns the size of the entity.
 * \return the size of the entity
 */
Size Entity::get_size() const {
  return bounding_box.get_size();
}

/**
 * \brief Sets the size of the entity.
 * \param width the entity's width
 * \param height the entity's height
 */
void Entity::set_size(int width, int height) {

  Debug::check_assertion(width % 8 == 0 && height % 8 == 0,
      "Invalid entity size: width and height must be multiple of 8");
  bounding_box.set_size(width, height);

  notify_size_changed();
}

/**
 * \brief Sets the size of the entity.
 * \param size to set to the entity
 */
void Entity::set_size(const Size& size) {

  set_size(size.width, size.height);
}

/**
 * \brief Notifies this entity that its size has just changed.
 */
void Entity::notify_size_changed() {

  notify_bounding_box_changed();
}

/**
 * \brief Returns the bounding box of the entity.
 *
 * This function returns the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 * The sprites of this entity may exceed the bounding box:
 * if you want a bounding box that takes sprites into account,
 * call get_max_bounding_box().
 *
 * \return The position and size of the entity.
 */
const Rectangle& Entity::get_bounding_box() const {
  return bounding_box;
}

/**
 * \brief Sets the bounding box of the entity.
 *
 * This function sets the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 *
 * \param bounding_box The new position and size of the entity.
 */
void Entity::set_bounding_box(const Rectangle &bounding_box) {
  this->bounding_box = bounding_box;
}

/**
 * \brief Returns the bounding box of the entity extended with some margin.
 * \param margin Margin to add to every side.
 * \return The extended bounding box.
 */
Rectangle Entity::get_extended_bounding_box(int margin) const {

  Rectangle extended_box = get_bounding_box();
  extended_box.set_xy(extended_box.get_xy() - Point(margin, margin));
  extended_box.set_size(extended_box.get_size() + 2 * Size(margin, margin));
  return extended_box;
}

/**
 * \brief Returns the bounding box of the entity including its sprites.
 *
 * The default implementation takes into account the bounding box of sprites
 * created with create_sprite().
 * Subclasses that manage sprites without create_sprite() should reimplement
 * this function to return a correct bounding box, otherwise sprite collisions
 * may fail to be detected.
 *
 * \return The bounding box of the entity including its sprites.
 */
Rectangle Entity::get_max_bounding_box() const {

  Rectangle result = get_bounding_box();
  for (const SpritePtr& sprite: get_sprites()) {
    Rectangle box = sprite->get_max_bounding_box();
    box.add_xy(sprite->get_xy());  // Take into account the sprite's own offset.
    box.add_xy(get_xy());  // Take into account the coordinates of the entity.
    result |= box;
    // TODO when the sprite's offset changes, update the bounding box
  }
  return result;
}

/**
 * \brief Notifies this entity that its bounding box may have changed.
 *
 * This function should be called when the value of get_bounding_box() or
 * get_max_bounding_box() changes.
 */
void Entity::notify_bounding_box_changed() {

  if (is_on_map()) {
    get_entities().notify_entity_bounding_box_changed(*this);
  }
}

/**
 * \brief Returns whether the entity's bounding box is aligned with the 8*8 grid of the map.
 * \return true if the entity's bounding box is aligned
 */
bool Entity::is_aligned_to_grid() const {
  return is_aligned_to_grid_x() && is_aligned_to_grid_y();
}

/**
 * \brief Returns whether the entity's top-left corner is aligned
 * horizontally with the 8*8 grid of the map.
 * \return true if the entity's top-left corner is aligned hotizontally
 */
bool Entity::is_aligned_to_grid_x() const {
  return get_top_left_x() % 8 == 0;
}

/**
 * \brief Returns whether the entity's top-left corner is aligned
 * vertically with the 8*8 grid of the map.
 * \return true if the entity's top-left corner is aligned vertically
 */
bool Entity::is_aligned_to_grid_y() const {
  return get_top_left_y() % 8 == 0;
}

/**
 * \brief Makes the entity's top-left corner aligned with the 8*8 grid of the map.
 *
 * Be careful: This function does not check the collisions with obstacles.
 */
void Entity::set_aligned_to_grid() {

  set_aligned_to_grid_x();
  set_aligned_to_grid_y();
}

/**
 * \brief Makes the entity's top-left corner aligned horizontally
 * with the 8*8 grid of the map.
 *
 * Be careful: This function does not check the collisions with obstacles.
 */
void Entity::set_aligned_to_grid_x() {

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
void Entity::set_aligned_to_grid_y() {

  int y = get_top_left_y() + 4;
  y -= y % 8;
  set_top_left_y(y);
}

/**
 * \brief Returns the coordinates of the point the entity is looking at.
 *
 * By default, this point depends on the direction of the sprite if any.
 * If there is no sprite, or if the sprite has not 4 directions,
 * then the movement is considered.
 * If there is no movement either, the entity is assumed to look to the north.
 * You can redefine this method to change what is a facing point for your
 * subclass.
 *
 * \return The coordinates of the point the entity is looking at.
 */
Point Entity::get_facing_point() const {

  int direction4 = 1;  // North by default.
  const SpritePtr& sprite = get_sprite();
  if (sprite != nullptr && sprite->get_nb_directions() == 4) {
    // Use the sprite to decide where the entity is looking to.
    direction4 = sprite->get_current_direction();
  }
  else {
    if (get_movement() != nullptr) {
      // Otherwise use the movement.
      direction4 = get_movement()->get_displayed_direction4();
    }
  }

  return get_touching_point(direction4);
}

/**
 * \brief Returns the coordinates of the point the entity would be facing
 * if it was looking towards the specified direction.
 * \param direction A direction (0 to 3).
 * \return The point the entity touching this direction.
 */
Point Entity::get_touching_point(int direction) const {

  Point touching_point = get_center_point();

  switch (direction) {

    // right
    case 0:
      touching_point.x += get_width() / 2;
      break;

    // up
    case 1:
      touching_point.y += -get_height() / 2 - 1;
      break;

    // left
    case 2:
      touching_point.x += -get_width() / 2 - 1;
      break;

    // down
    case 3:
      touching_point.y += get_height() / 2;
      break;

    default:
      Debug::die("Invalid direction for Entity::get_touching_point()");
  }
  return touching_point;
}

/**
 * \brief Returns the detector in front of this entity.
 * \return The detector this entity is facing, or nullptr if there is no detector in front of him.
 */
Entity* Entity::get_facing_entity() {
  return facing_entity;
}

/**
 * \brief Returns the detector in front of this entity.
 * \return The detector this entity is facing, or nullptr if there is no detector in front of him.
 */
const Entity* Entity::get_facing_entity() const {
  return facing_entity;
}

/**
 * \brief Sets the entity this entity is currently facing.
 *
 * This function is called when this entity is facing a new detector.
 *
 * \param facing_entity The detector this entity is now facing (possibly nullptr).
 */
void Entity::set_facing_entity(Entity* facing_entity) {

  this->facing_entity = facing_entity;
  notify_facing_entity_changed(facing_entity);
}

/**
 * \brief Notifies this entity that its facing entity has just changed.
 * \param facing_entity The detector this entity is now facing (possibly nullptr).
 */
void Entity::notify_facing_entity_changed(Entity* /* facing_entity */) {
}

/**
 * \brief Returns the point used to determine which ground is below this entity.
 * \return The point used to determine the ground (relative to the map).
 */
Point Entity::get_ground_point() const {

  // Return a point slightly above the origin point, otherwise the hero
  // falls too easily in bad grounds to the South.
  return { get_x(), get_y() - 2 };
}

/**
 * \brief Returns the coordinates of the center point of the entity's rectangle.
 * \return The coordinates of the center point of the entity.
 */
Point Entity::get_center_point() const {
  return bounding_box.get_center();
}

/**
 * \brief Returns the name of the entity (if any).
 * \return the name of the entity, or an empty string if
 * the entity has no name.
 */
const std::string& Entity::get_name() const {
  return name;
}

/**
 * \brief Sets the name of the entity.
 * \param name The name of the entity, or an empty string to unset the name.
 */
void Entity::set_name(const std::string& name) {

  this->name = name;
}

/**
 * \brief Returns whether this entity has a name.
 * \return \c true if the name is not empty.
 */
bool Entity::has_name() const {
  return !name.empty();
}

/**
 * \brief Returns whether the name of this entity starts with the specified prefix.
 * \param prefix a prefix
 * \return true if the name starts with this prefix
 */
bool Entity::has_prefix(const std::string& prefix) const {
  return name.substr(0, prefix.size()) == prefix;
}

/**
 * \brief Returns the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * \return the origin point
 */
const Point& Entity::get_origin() const {
  return origin;
}

/**
 * \brief Sets the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * \param x x coordinate of the origin
 * \param y y coordinate of the origin
 */
void Entity::set_origin(int x, int y) {

  bounding_box.add_xy(origin.x - x, origin.y - y);
  origin = { x, y };
}

/**
 * \brief Sets the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * \param origin x and y coordinates of the origin
 */
void Entity::set_origin(const Point& origin) {
  set_origin(origin.x, origin.y);
}

/**
 * \brief Returns the optimization distance of this entity.
 *
 * Above this distance from the visible area, the engine may decide to
 * skip updates.
 *
 * \return The optimization distance (0 means infinite).
 */
int Entity::get_optimization_distance() const {
  return optimization_distance;
}

/**
 * \brief Returns the square of the optimization distance of this entity.
 * \return Square of the optimization distance (0 means infinite).
 */
int Entity::get_optimization_distance2() const {
  return optimization_distance2;
}

/**
 * \brief Sets the optimization distance of this entity.
 *
 * Above this distance from the visible area, the engine may decide to
 * skip updates.
 *
 * \param distance The optimization distance (0 means infinite).
 */
void Entity::set_optimization_distance(int distance) {
  this->optimization_distance = distance;
  this->optimization_distance2 = distance * distance;
}

/**
 * \brief Returns the user-defined properties of this entity.
 * \return The user-defined properties.
 */
const std::vector<Entity::UserProperty>& Entity::get_user_properties() const {
  return user_properties;
}

/**
 * \brief Sets the user-defined properties of this entity.
 * \param user_properties The user-defined properties to set.
 */
void Entity::set_user_properties(const std::vector<UserProperty>& user_properties) {

  this->user_properties = user_properties;
}

/**
 * \brief Returns whether the entity has a user-defined property.
 * \param key Key of the user-defined property to check.
 * \return \c true if such a property exists.
 */
bool Entity::has_user_property(const std::string& key) const {

  for (const UserProperty& user_property : user_properties) {
    if (user_property.first == key) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Returns the value of a user property.
 * \param key Key of the property to get.
 * \return The corresponding value or an empty string.
 */
const std::string& Entity::get_user_property_value(const std::string& key) const {

  for (const UserProperty& user_property : user_properties) {
    if (user_property.first == key) {
      return user_property.second;
    }
  }

  static const std::string empty_string;
  return empty_string;
}

/**
 * \brief Sets the value of a user property.
 *
 * Creates the property if it does not exists yet.
 *
 * \param key Key of the property to set.
 * \param value The value to set.
 */
void Entity::set_user_property_value(const std::string& key, const std::string& value) {

  for (UserProperty& user_property : user_properties) {
    if (user_property.first == key) {
      user_property.second = value;
      return;
    }
  }

  // Not found: add a new property.
  user_properties.emplace_back(std::make_pair(key, value));
}

/**
 * \brief Removes a user property if it exists.
 * \param key Key of the property to remove.
 */
void Entity::remove_user_property(const std::string& key) {

  for (auto it = user_properties.begin(); it != user_properties.end(); ++it) {
    if (it->first == key) {
      user_properties.erase(it);
      return;
    }
  }
}

/**
 * \brief Returns whether the entity has at least one sprite.
 * \return true if the entity has at least one sprite.
 */
bool Entity::has_sprite() const {

  for (const NamedSprite& named_sprite : sprites) {
    if (!named_sprite.removed) {
      return true;
    }
  }
  return false;
}

/**
 * \brief Returns a sprite of this entity.
 * \param sprite_name Name of the sprite to get, or an empty string to
 * get the default sprite.
 * \return The corresponding sprite, or nullptr if there is no such sprite.
 */
SpritePtr Entity::get_sprite(const std::string& sprite_name) const {

  if (sprites.empty()) {
    // This entity has no sprite.
    return nullptr;
  }

  std::string valid_sprite_name;
  if (sprite_name.empty()) {
    // No sprite name specified: use the default one if any.
    if (default_sprite_name.empty()) {
      // No default sprite either: return the first one.
      for (const NamedSprite& named_sprite : sprites) {
        if (!named_sprite.removed) {
          return named_sprite.sprite;
        }
      }
    }
    else {
      valid_sprite_name = default_sprite_name;
    }
  }
  else {
    valid_sprite_name = sprite_name;
  }

  for (const NamedSprite& named_sprite : sprites) {
    // Find a sprite with the given name and that was not removed.
    if (named_sprite.name == valid_sprite_name &&
        !named_sprite.removed) {
      return named_sprite.sprite;
    }
  }
  return nullptr;
}

/**
 * \brief Returns all sprites of this entity.
 * \return The sprites.
 */
std::vector<SpritePtr> Entity::get_sprites() const {

  std::vector<SpritePtr> result;
  result.reserve(sprites.size());
  for (const NamedSprite& named_sprite : sprites) {
    if (!named_sprite.removed) {
      result.push_back(named_sprite.sprite);
    }
  }
  return result;
}

/**
 * \brief Returns all sprites of this entity and their names.
 * \return The sprites and their names.
 */
std::vector<Entity::NamedSprite> Entity::get_named_sprites() const {
  return sprites;
}

/**
 * \brief Creates a sprite and adds it to this entity.
 * \param animation_set_id Id of the sprite's animation set to use.
 * \param sprite_name Name to identify the sprite or an empty string.
 * \return The sprite created.
 */
SpritePtr Entity::create_sprite(
    const std::string& animation_set_id,
    const std::string& sprite_name
) {
  SpritePtr sprite = std::make_shared<Sprite>(animation_set_id);

  NamedSprite named_sprite;
  named_sprite.name = sprite_name;
  named_sprite.sprite = sprite;
  named_sprite.removed = false;
  sprites.emplace_back(named_sprite);
  notify_bounding_box_changed();
  return sprite;
}

/**
 * \brief Marks a sprite of this entity to be removed as soon as possible.
 * \return \c true in case of success, \c false if this entity has no such
 * sprite.
 */
bool Entity::remove_sprite(Sprite& sprite) {

  for (NamedSprite& named_sprite: sprites) {
    if (named_sprite.sprite.get() == &sprite) {
      if (named_sprite.removed) {
        continue;
      }
      named_sprite.removed = true;
      return true;
    }
  }

  return false;
}

/**
 * \brief Removes all sprites of this entity.
 *
 * They will be destroyed at next iteration.
 */
void Entity::clear_sprites() {

  for (NamedSprite& named_sprite: sprites) {
    named_sprite.removed = true;
  }
}

/**
 * \brief Really removes the sprites that were recently marked to be removed.
 */
void Entity::clear_old_sprites() {

  for (auto it = sprites.begin();
      it != sprites.end();
      // No ++it since elements can be removed while traversing
  ) {
    const NamedSprite& named_sprite = *it;
    if (named_sprite.removed) {
      it = sprites.erase(it);
    }
    else {
      ++it;
    }
  }
}

/**
 * \brief Changes the order of a sprite of this entity to display it first.
 * \return \c true in case of success, \c false if this entity has no such
 * sprite.
 */
bool Entity::bring_sprite_to_back(Sprite& sprite) {

  for (NamedSprite& named_sprite: sprites) {
    if (named_sprite.removed) {
      continue;
    }
    if (named_sprite.sprite.get() == &sprite) {
      NamedSprite copy = named_sprite;
      named_sprite.removed = true;
      // Bring to back means displaying first.
      sprites.insert(sprites.begin(), copy);
      return true;
    }
  }

  return false;
}

/**
 * \brief Changes the order of a sprite of this entity to display it last.
 * \return \c true in case of success, \c false if this entity has no such
 * sprite.
 */
bool Entity::bring_sprite_to_front(Sprite& sprite) {

  for (NamedSprite& named_sprite: sprites) {
    if (named_sprite.removed) {
      continue;
    }
    if (named_sprite.sprite.get() == &sprite) {
      NamedSprite copy = named_sprite;
      named_sprite.removed = true;
      // Bring to front means displaying last.
      sprites.push_back(copy);
      return true;
    }
  }

  return false;
}

/**
 * \brief Returns the name of the default sprite of this entity.
 * \return The default sprite name.
 * An empty name means the first one in creation order.
 */
std::string Entity::get_default_sprite_name() const {
  return default_sprite_name;
}

/**
 * \brief Sets the default sprite of this entity.
 * \param default_sprite_name The default sprite name to set.
 * An empty name means the first one in creation order.
 */
void Entity::set_default_sprite_name(const std::string& default_sprite_name) {

  this->default_sprite_name = default_sprite_name;
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
void Entity::notify_sprite_frame_changed(Sprite& /* sprite */, const std::string& /* animation */, int /* frame */) {
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
void Entity::notify_sprite_animation_finished(Sprite& /* sprite */, const std::string& /* animation */) {
}

/**
 * \brief Returns whether this entity is currently visible.
 * \return true if this entity is currently visible
 */
bool Entity::is_visible() const {
  return visible;
}

/**
 * \brief Sets whether this entity is visible.
 * \param visible true to make it visible
 */
void Entity::set_visible(bool visible) {
  this->visible = visible;
}

/**
 * \brief Returns the current movement of the entity.
 * \return the entity's movement, or nullptr if there is no movement
 */
const std::shared_ptr<Movement>& Entity::get_movement() {
  return movement;
}

/**
 * \brief Returns the current movement of the entity.
 * \return the entity's movement, or nullptr if there is no movement
 */
std::shared_ptr<const Movement> Entity::get_movement() const {
  return movement;
}

/**
 * \brief Sets the movement of this entity.
 * \param movement the movement to set, or nullptr to set no movement
 */
void Entity::set_movement(const std::shared_ptr<Movement>& movement) {

  clear_movement();
  this->movement = movement;

  if (movement != nullptr) {
    movement->set_lua_notifications_enabled(true);
    movement->set_entity(this);

    if (movement->is_suspended() != suspended) {
      movement->set_suspended(suspended || !is_enabled());
    }
    notify_movement_started();
  }
}

/**
 * \brief Clears the movement of this entity.
 *
 * The entity immediately stops moving.
 * The movement object will be released at the next cycle,
 * thus this function can be called by the movement object itself.
 */
void Entity::clear_movement() {

  if (movement != nullptr) {
    movement->set_entity(nullptr);                   // Tell the movement to forget me.
    movement->set_lua_notifications_enabled(false);  // Stop future Lua callbacks.
    old_movements.push_back(movement);               // Destroy it later.
    movement = nullptr;
  }
}

/**
 * \brief Destroys the old movements of this entity.
 */
void Entity::clear_old_movements() {

  old_movements.clear();
}

/**
 * \brief Returns whether Lua movement events are enabled for this entity.
 *
 * If no, events entity:on_position_changed(), entity:on_obstacle_reached(),
 * entity:on_movement_started(), entity:on_movement_changed() and
 * entity:on_movement_finished() won't be called.
 *
 * \return Whether movement events are currently enabled.
 */
bool Entity::are_movement_notifications_enabled() const {
  return main_loop != nullptr &&
      movement_notifications_enabled;
}

/**
 * \brief Sets whether Lua movement events are enabled for this entity.
 *
 * If no, events entity:on_position_changed(), entity:on_obstacle_reached(),
 * entity:on_movement_started(), entity:on_movement_changed() and
 * entity:on_movement_finished() won't be called.
 *
 * \param notify \c true to enable movement events.
 */
void Entity::set_movement_notifications_enabled(bool notify) {
  this->movement_notifications_enabled = notify;
}

/**
 * \brief Returns whether this entity is following a stream.
 * \return \c true if there is an active stream action.
 */
bool Entity::has_stream_action() const {
  return stream_action != nullptr && stream_action->is_active();
}

/**
 * \brief Returns the stream action currently applied if any.
 * \return The stream action of this entity or nullptr.
 */
const StreamAction* Entity::get_stream_action() const {
  return stream_action.get();
}

/**
 * \brief Returns the stream action currently applied if any.
 * \return The stream action of this entity or nullptr.
 */
StreamAction* Entity::get_stream_action() {
  return stream_action.get();
}

/**
 * \brief Makes this entity follow a stream.
 * \param stream_action The stream action to start.
 */
void Entity::start_stream_action(
    std::unique_ptr<StreamAction> stream_action
) {
  stop_stream_action();
  this->stream_action = std::move(stream_action);
}

/**
 * \brief Stops following a stream if any.
 */
void Entity::stop_stream_action() {

  stream_action = nullptr;
}

/**
 * \brief Notifies this entity that it has just failed to change its position
 * because of obstacles.
 *
 * This function is called only when the movement is not suspended.
 */
void Entity::notify_obstacle_reached() {

  if (are_movement_notifications_enabled()) {
    get_lua_context()->entity_on_obstacle_reached(*this, *get_movement());
  }
}

/**
 * \brief This function is called when the entity has just moved.
 *
 * It checks collisions with the detectors on the map
 * and, if this entity defines a ground, updates entities sensible to their
 * ground.
 *
 * TODO only keep notify_bounding_box_changed()
 */
void Entity::notify_position_changed() {

  // Notify the quadtree.
  notify_bounding_box_changed();

  if (is_detector()) {
    // Since this entity is a detector, all entities need to check
    // their collisions with it.
    get_map().check_collision_from_detector(*this);
  }

  // Check collisions between this entity and other detectors.
  check_collision_with_detectors();

  // Update the ground.
  if (is_ground_modifier()) {
    update_ground_observers();
  }
  update_ground_below();

  // Notify Lua.
  if (are_movement_notifications_enabled()) {
    get_lua_context()->entity_on_position_changed(*this, get_xy(), get_layer());
  }
}

/**
 * \brief Returns whether this entity is able to detect other entities.
 *
 * This is \c true if this entity has at least one collision mode.
 *
 * \return \c true if this entity can detect other entities.
 */
bool Entity::is_detector() const {
  return collision_modes != CollisionMode::COLLISION_NONE;
}

/**
 * \brief Sets the collision modes detected by this entity.
 * \param collision_modes The collision modes to set
 * (can be an OR combination of collision modes).
 */
void Entity::set_collision_modes(int collision_modes) {

  if (collision_modes & CollisionMode::COLLISION_SPRITE) {
    enable_pixel_collisions();
  }
  this->collision_modes = collision_modes;
}

/**
 * \brief Adds a collision mode to be detected this entity.
 * \param collision_mode The collision mode to add.
 */
void Entity::add_collision_mode(CollisionMode collision_mode) {

  set_collision_modes(this->collision_modes | collision_mode);
}

/**
 * \brief Returns whether the entity detects the specified.
 * \param collision_mode A collision mode.
 * \return \c true if this collision mode is detected by this entity.
 */
bool Entity::has_collision_mode(CollisionMode collision_mode) {
  return (this->collision_modes & collision_mode) != 0;
}

/**
 * \brief Enables the pixel-perfect collision checks for all sprites
 * of this entity.
 */
void Entity::enable_pixel_collisions() {

  for (const NamedSprite& named_sprite: sprites) {
    named_sprite.sprite->enable_pixel_collisions();
  }
}

/**
 * \brief Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * \return \c true if this entity can detect collisions with entities
 * that are on another layer.
 */
bool Entity::has_layer_independent_collisions() const {
  return layer_independent_collisions;
}

/**
 * \brief Sets whether this entity can detect collisions with entities even if
 * they are not on the same layer.
 * \param independent \c true if this entity can detect entities
 * that are on another layer.
 */
void Entity::set_layer_independent_collisions(bool independent) {
  this->layer_independent_collisions = independent;
}

/**
 * \brief Checks whether an entity collides with this detector.
 *
 * Only checks non pixel precise collisions.
 * Does nothing if this entity is not a detector.
 *
 * This function is called by the map when an entity has just moved.
 * It checks whether the entity collides with this detector.
 * Depending on this detector's collision mode(s), the appropriate
 * test_collision_* functions are called.
 * If there is a collision, the notify_collision() method is called.
 *
 * \param other The entity to check.
 */
void Entity::check_collision(Entity& other) {

  if (!is_detector()) {
    // No collision kind to detect.
    return;
  }

  if (&other == this) {
    return;
  }

  if (get_layer() != other.get_layer() && !has_layer_independent_collisions()) {
    // Not the same layer: no collision.
    return;
  }

  // Detect the collision depending on the collision modes.

  if (has_collision_mode(CollisionMode::COLLISION_OVERLAPPING) && test_collision_rectangle(other)) {
    notify_collision(other, CollisionMode::COLLISION_OVERLAPPING);
  }

  if (has_collision_mode(CollisionMode::COLLISION_CONTAINING) && test_collision_inside(other)) {
    notify_collision(other, CollisionMode::COLLISION_CONTAINING);
  }

  if (has_collision_mode(CollisionMode::COLLISION_ORIGIN) && test_collision_origin_point(other)) {
    notify_collision(other, CollisionMode::COLLISION_ORIGIN);
  }

  if (has_collision_mode(CollisionMode::COLLISION_FACING) && test_collision_facing_point(other)) {

    if (other.get_facing_entity() == nullptr) {
      // Make sure only one entity can think "I am the facing entity".
      other.set_facing_entity(this);
    }
    notify_collision(other, CollisionMode::COLLISION_FACING);
  }

  if (has_collision_mode(CollisionMode::COLLISION_TOUCHING) && test_collision_touching(other)) {
    notify_collision(other, CollisionMode::COLLISION_TOUCHING);
  }

  if (has_collision_mode(CollisionMode::COLLISION_CENTER) && test_collision_center(other)) {
    notify_collision(other, CollisionMode::COLLISION_CENTER);
  }

  if (has_collision_mode(CollisionMode::COLLISION_CUSTOM) && test_collision_custom(other)) {
    notify_collision(other, CollisionMode::COLLISION_CUSTOM);
  }
}

/**
 * \brief Checks whether any sprite of this detector collides
 * with a specific sprite of another entity.
 *
 * If there is a collision, the notify_collision(Entity&, Sprite&, Sprite&) method is called.
 *
 * \param other The entity to check.
 * \param other_sprite The sprite of that entity.
 */
void Entity::check_collision(Entity& other, Sprite& other_sprite) {

  if (!has_collision_mode(CollisionMode::COLLISION_SPRITE)) {
    return;
  }

  if (&other == this) {
    return;
  }

  if (get_layer() != other.get_layer() && !has_layer_independent_collisions()) {
    // Not the same layer: no collision.
    return;
  }

  if (!other_sprite.is_animation_started()) {
    // Animation is not running.
    return;
  }

  if (!other_sprite.are_pixel_collisions_enabled()) {
    return;
  }

  // We check the collision between the specified entity's sprite and
  // all sprites of the current entity.
  // Make a copy of the sprites list in case it gets reallocated while
  // traversing it.
  std::vector<SpritePtr> this_sprites = get_sprites();
  for (const SpritePtr& this_sprite: this_sprites) {

    if (!this_sprite->is_animation_started()) {
      continue;
    }

    if (!this_sprite->are_pixel_collisions_enabled()) {
      continue;
    }

    if (this_sprite->test_collision(other_sprite, get_x(), get_y(), other.get_x(), other.get_y())) {
      notify_collision(other, *this_sprite, other_sprite);
    }
  }
}

/**
 * \brief Checks whether a specific sprite of this detector collides
 * with any sprite of another entity.
 *
 * If there is a collision, the notify_collision(Entity&, Sprite&, Sprite&) method is called.
 *
 * \param other The entity to check.
 * \param other_sprite The sprite of that entity.
 */
void Entity::check_collision(Sprite& this_sprite, Entity& other) {

  if (!has_collision_mode(CollisionMode::COLLISION_SPRITE)) {
    return;
  }

  if (&other == this) {
    return;
  }

  if (get_layer() != other.get_layer() && !has_layer_independent_collisions()) {
    // Not the same layer: no collision.
    return;
  }

  if (!this_sprite.is_animation_started()) {
    // Animation is not running.
    return;
  }

  if (!this_sprite.are_pixel_collisions_enabled()) {
    return;
  }

  // We check the collision between the specified detector's sprite and
  // all sprites of the other entity.
  // Make a copy of the sprites list in case it gets reallocated while
  // traversing it.
  std::vector<SpritePtr> other_sprites = other.get_sprites();
  for (const SpritePtr& other_sprite: other_sprites) {

    if (!other_sprite->is_animation_started()) {
      continue;
    }

    if (!other_sprite->are_pixel_collisions_enabled()) {
      continue;
    }

    if (this_sprite.test_collision(*other_sprite, get_x(), get_y(), other.get_x(), other.get_y())) {
      notify_collision(other, this_sprite, *other_sprite);
    }
  }
}

/**
 * \brief Returns whether an entity is overlapping this entity with the given
 * collision test.
 * \param entity The entity.
 * \param collision_mode The collision test to perform.
 * \return \c true if there is a collision.
 */
bool Entity::test_collision(Entity& entity, CollisionMode collision_mode) {

  if (get_layer() != entity.get_layer() && !has_layer_independent_collisions()) {
    // Not the same layer: no collision.
    return false;
  }

  switch (collision_mode) {

  case CollisionMode::COLLISION_NONE:
    return false;

  case CollisionMode::COLLISION_OVERLAPPING:
    return test_collision_rectangle(entity);

  case CollisionMode::COLLISION_CONTAINING:
    return test_collision_inside(entity);

  case CollisionMode::COLLISION_ORIGIN:
    return test_collision_origin_point(entity);

  case CollisionMode::COLLISION_FACING:
    return test_collision_facing_point(entity);

  case CollisionMode::COLLISION_TOUCHING:
    return test_collision_touching(entity);

  case CollisionMode::COLLISION_CENTER:
    return test_collision_center(entity);

  case CollisionMode::COLLISION_CUSTOM:
    return test_collision_custom(entity);

  case CollisionMode::COLLISION_SPRITE:
    return test_collision_sprites(entity);
  }

  return false;
}

/**
 * \brief Returns whether an entity's rectangle is overlapping this entity's rectangle.
 *
 * The layer is not checked.
 *
 * \param entity The entity.
 * \return \c true if the entity's rectangle is overlapping this entity's rectangle.
 */
bool Entity::test_collision_rectangle(const Entity& entity) const {

  return entity.overlaps(*this);
}

/**
 * \brief Returns whether an entity's rectangle is entirely inside this entity's rectangle.
 *
 * The layer is not checked.
 *
 * \param entity The entity.
 * \return \c true if the entity's rectangle is entirely inside this entity's rectangle.
 */
bool Entity::test_collision_inside(const Entity& entity) const {

  return get_bounding_box().contains(entity.get_bounding_box());
}


/**
 * \brief Returns whether the origin point of an entity is overlapping this entity's rectangle.
 *
 * The layer is not checked.
 *
 * \param entity The entity.
 * \return \c true if the entity's origin point is overlapping the this entity's rectangle
 */
bool Entity::test_collision_origin_point(const Entity& entity) const {

  return entity.is_origin_point_in(get_bounding_box());
}

/**
 * \brief Returns whether the facing point of an entity is overlapping this entity's rectangle.
 *
 * The layer is not checked.
 *
 * \param entity The entity.
 * \return \c true if the entity's facing point is overlapping this entity's rectangle
 */
bool Entity::test_collision_facing_point(const Entity& entity) const {

  return entity.is_facing_point_in(get_bounding_box());
}

/**
 * \brief Returns whether a touching point of an entity
 * (in any of the four main directions)
 * is overlapping this entity's rectangle.
 *
 * The layer is not checked.
 *
 * \param entity The entity.
 * \return \c true if a touching point of the entity is overlapping
 * this entity's rectangle.
 */
bool Entity::test_collision_touching(const Entity& entity) const {

  const Rectangle& bounding_box = get_bounding_box();
  return entity.is_touching_point_in(bounding_box, 0)
      || entity.is_touching_point_in(bounding_box, 1)
      || entity.is_touching_point_in(bounding_box, 2)
      || entity.is_touching_point_in(bounding_box, 3);
}

/**
 * \brief Returns whether the center point of an entity is overlapping this entity's rectangle.
 *
 * The layer is not checked.
 *
 * \param entity The entity.
 * \return \c true if the entity's center is overlapping this entity's rectangle.
 */
bool Entity::test_collision_center(const Entity& entity) const {

  return entity.is_center_in(get_bounding_box());
}

/**
 * \brief Returns whether a sprite of this entity collides with a sprite of another one.
 *
 * The layer is not checked.
 * The test is pixel-precise.
 *
 * \param entity The entity.
 * \return \c true if sprites of both entities overlap.
 */
bool Entity::test_collision_sprites(Entity& entity) {

  for (const NamedSprite& this_named_sprite: sprites) {

    if (this_named_sprite.removed) {
      continue;
    }
    Sprite& this_sprite = *this_named_sprite.sprite;
    this_sprite.enable_pixel_collisions();
    for (const NamedSprite& other_named_sprite: entity.sprites) {

      if (other_named_sprite.removed) {
        continue;
      }
      Sprite& other_sprite = *other_named_sprite.sprite;
      other_sprite.enable_pixel_collisions();
      if (this_sprite.test_collision(other_sprite, get_x(), get_y(), entity.get_x(), entity.get_y())) {
        return true;
      }
    }
  }

  return false;
}

/**
 * \brief Returns whether an entity collides with this entity with respect to a custom rule.
 *
 * This method is called by check_collision(Entity*) when this entity's collision
 * mode is COLLISION_CUSTOM.
 * Redefine it if you want to use this collision mode.
 *
 * \param entity The entity.
 * \return \c true if the entity's collides with this entity with respect to the custom rule
 */
bool Entity::test_collision_custom(Entity& /* entity */) {

  return false;
}

/**
 * \brief Notifies this entity that it just detected a collision with another entity.
 *
 * This function is called by check_collision(Entity*)
 * when an entity overlaps this detector.
 *
 * \param entity_overlapping The entity overlapping this detector.
 * \param collision_mode The collision mode that detected the collision (useful if
 * this detector has several collision modes).
 */
void Entity::notify_collision(Entity& /* entity_overlapping */, CollisionMode /* collision_mode */) {

  // By default, nothing happens.
  // Redefine this method in the subclasses to do the appropriate action.
}

/**
 * \brief Notifies this entity that it just detected a pixel-perfect collision with another sprite.
 *
 * This function is called by check_collision(Entity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 *
 * \param other_entity The entity overlapping this detector.
 * \param this_sprite The sprite of this detector that is overlapping the other entity's sprite.
 * \param other_sprite The sprite of other_entity that is overlapping this detector.
 */
void Entity::notify_collision(
    Entity& /* other_entity */,
    Sprite& /* this_sprite */,
    Sprite& /* other_sprite */
) {
  // By default, nothing happens.
  // Redefine this method in the subclasses to do the appropriate action.
}

/**
 * \brief Checks collisions between this entity and the detectors of the map.
 *
 * Simple collisions are checked, and then pixel-precise collisions if they are
 * enabled for some sprites of this entity.
 */
void Entity::check_collision_with_detectors() {

  if (!is_on_map()) {
    // The entity is still being initialized.
    return;
  }

  if (!is_enabled()) {
    // The entity is disabled.
    return;
  }

  // Detect simple collisions.
  get_map().check_collision_with_detectors(*this);

  // Detect pixel-precise collisions.
  std::vector<NamedSprite> sprites = this->sprites;
  for (const NamedSprite& named_sprite: sprites) {
    if (named_sprite.removed) {
      continue;
    }
    Sprite& sprite = *named_sprite.sprite;
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
void Entity::check_collision_with_detectors(Sprite& sprite) {

  if (!is_enabled()) {
    // The entity is disabled.
    return;
  }

  get_map().check_collision_with_detectors(*this, sprite);
}

/**
 * \brief This function is called when a movement started on this entity.
 */
void Entity::notify_movement_started() {

  if (are_movement_notifications_enabled()) {
    get_lua_context()->entity_on_movement_started(*this, *get_movement());
  }
}

/**
 * \brief This function is called when the movement of the entity is finished.
 */
void Entity::notify_movement_finished() {

  if (are_movement_notifications_enabled()) {
    get_lua_context()->entity_on_movement_finished(*this);
  }
}

/**
 * \brief This function can be called by the movement object
 * to notify the entity when the movement has just changed
 * (e.g. the speed, the angle or the trajectory).
 */
void Entity::notify_movement_changed() {

  if (are_movement_notifications_enabled()) {
    get_lua_context()->entity_on_movement_changed(*this, *get_movement());
  }
}

/**
 * \brief This function is called when this entity starts being moved by
 * another one.
 * \param entity The other entity.
 */
void Entity::notify_moving_by(Entity& /* entity */) {
  // Do nothing by default.
}

/**
 * \brief This function is called when this entity has just finished to get
 * moved by another one.
 * \param entity The other entity.
 */
void Entity::notify_moved_by(Entity& /* entity */) {
  // Do nothing by default.
}

/**
 * \brief Converts a direction into the corresponding one-pixel move on x and y.
 * \param direction8 a direction (0 to 7)
 * \return a point with x and y set to -1, 0 or 1 depending on the direction
 */
const Point& Entity::direction_to_xy_move(int direction8) {

  static constexpr Point directions_to_xy_moves[] = {
    {  1,  0 },
    {  1, -1 },
    {  0, -1 },
    { -1, -1 },
    { -1,  0 },
    { -1,  1 },
    {  0,  1 },
    {  1,  1 }
  };

  return directions_to_xy_moves[direction8];
}

/**
 * \brief Enables or disables this entity.
 * \param enabled true to enable the entity, false to disable it
 */
void Entity::set_enabled(bool enabled) {

  if (this->enabled == enabled) {
    return;
  }

  if (enabled) {
    // Enable the entity.

    this->enabled = true;

    if (!is_suspended()) {
      // Enabling an entity that is not suspended:
      // unsuspend its movement, its sprites and its timers.
      if (get_movement() != nullptr) {
        get_movement()->set_suspended(false);
      }

      if (stream_action != nullptr) {
        stream_action->set_suspended(false);
      }

      for (const NamedSprite& named_sprite: sprites) {
        if (named_sprite.removed) {
          continue;
        }
        Sprite& sprite = *named_sprite.sprite;
        sprite.set_suspended(false);
      }

      if (is_on_map()) {
        get_lua_context()->set_entity_timers_suspended(*this, false);
      }
    }
    notify_enabled(true);
  }
  else {
    this->enabled = false;

    if (!is_suspended()) {
      // Disabling an entity that is not suspended:
      // suspend its movement, its sprites and its timers.
      if (get_movement() != nullptr) {
        get_movement()->set_suspended(true);
      }

      for (const NamedSprite& named_sprite: sprites) {
        if (named_sprite.removed) {
          continue;
        }
        Sprite& sprite = *named_sprite.sprite;
        sprite.set_suspended(true);
      }

      if (is_on_map()) {
        get_lua_context()->set_entity_timers_suspended(*this, true);
      }
    }
    notify_enabled(false);
  }
}

/**
 * \brief Notifies this entity that it was just enabled or disabled.
 * \param enabled \c true if the entity is now enabled.
 */
void Entity::notify_enabled(bool /* enabled */) {

  if (!is_on_map()) {
    return;
  }

  if (is_ground_modifier()) {
    update_ground_observers();
  }
  update_ground_below();
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 *
 * This function is called by the default implementation of
 * Entity::is_obstacle_for(Entity&, const Rectangle& candidate_position).
 *
 * \param other Another entity.
 * \return \c true if this entity is an obstacle for the other one.
 */
bool Entity::is_obstacle_for(Entity& /* other */) {

  return false;
}

/**
 * \brief Returns whether this entity is an obstacle for another one it that
 * other entity was at the specified place.
 *
 * There are two mechanisms to block entities: by another entity or by the
 * ground of the map.
 *
 * This function is about entities blocking other entities.
 * If you return \c true, the other entity will be blocked even if there are
 * traversable entities on top of yours.
 *
 * If your entity modifies the ground of the map (see is_ground_modifier()),
 * you then rely on the map ground mechanism. In this case, if
 * other entities at the same coordinates also modify the map ground and are
 * higher than your entity, their ground overrides yours.
 *
 * To respect this behavior, is_obstacle_for() should return false for
 * ground modifiers.
 *
 * \param other Another entity.
 * \param candidate_position Candidate position of this other entity.
 * \return \c true if this entity is an obstacle for the other one at that
 * position.
 */
bool Entity::is_obstacle_for(Entity& other, const Rectangle& /* candidate_position */) {

  // By default, use a position-independent test.
  // Most entities don't need the candidate position of the other one to decide
  // if they want to block them.
  return is_obstacle_for(other);
}

/**
 * \brief Returns whether a kind of ground is an obstacle for this entity.
 */
bool Entity::is_ground_obstacle(Ground ground) const {

  switch (ground) {

  case Ground::EMPTY:
    return false;

  case Ground::TRAVERSABLE:
    return is_traversable_obstacle();

  case Ground::WALL:
    return is_wall_obstacle();

  case Ground::LOW_WALL:
    return is_low_wall_obstacle();

  case Ground::WALL_TOP_RIGHT:
  case Ground::WALL_TOP_LEFT:
  case Ground::WALL_BOTTOM_LEFT:
  case Ground::WALL_BOTTOM_RIGHT:
  case Ground::WALL_TOP_RIGHT_WATER:
  case Ground::WALL_TOP_LEFT_WATER:
  case Ground::WALL_BOTTOM_LEFT_WATER:
  case Ground::WALL_BOTTOM_RIGHT_WATER:
    return is_wall_obstacle();

  case Ground::DEEP_WATER:
    return is_deep_water_obstacle();

  case Ground::SHALLOW_WATER:
    return is_shallow_water_obstacle();

  case Ground::GRASS:
    return is_grass_obstacle();

  case Ground::HOLE:
    return is_hole_obstacle();

  case Ground::ICE:
    return is_ice_obstacle();

  case Ground::LADDER:
    return is_ladder_obstacle();

  case Ground::PRICKLE:
    return is_prickle_obstacle();

  case Ground::LAVA:
    return is_lava_obstacle();

  }

  return false;
}

/**
 * \brief Returns whether traversable ground is currently considered as an
 * obstacle by this entity.
 *
 * This function returns \c false by default.
 *
 * \return \c true if traversable ground is currently obstacle for this entity.
 */
bool Entity::is_traversable_obstacle() const {
    return false;
}

/**
 * \brief Returns whether walls are currently considered as an
 * obstacle by this entity.
 *
 * This function returns \c true by default.
 *
 * \return \c true if wall ground is currently obstacle for this entity.
 */
bool Entity::is_wall_obstacle() const {
    return true;
}

/**
 * \brief Returns whether a low wall is currently considered as an obstacle
 * by this entity.
 *
 * This function returns \c true by default.
 *
 * \return \c true if low walls are currently obstacle for this entity.
 */
bool Entity::is_low_wall_obstacle() const {
  return true;
}

/**
 * \brief Returns whether grass ground is currently considered as an
 * obstacle by this entity.
 *
 * This function returns \c false by default.
 *
 * \return \c true if grass ground is currently obstacle for this entity.
 */
bool Entity::is_grass_obstacle() const {
  return false;
}

/**
 * \brief Returns whether shallow water is currently considered as an obstacle by this entity.
 *
 * This function returns is_deep_water_obstacle() by default.
 *
 * \return true if shallow water is currently an obstacle for this entity
 */
bool Entity::is_shallow_water_obstacle() const {
  return is_deep_water_obstacle();
}

/**
 * \brief Returns whether deep water is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if deep water is currently an obstacle for this entity
 */
bool Entity::is_deep_water_obstacle() const {
  return true;
}

/**
 * \brief Returns whether a hole is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if the holes are currently an obstacle for this entity
 */
bool Entity::is_hole_obstacle() const {
  return true;
}

/**
 * \brief Returns whether lava is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if lava is currently an obstacle for this entity
 */
bool Entity::is_lava_obstacle() const {
  return true;
}

/**
 * \brief Returns whether ice ground is currently considered as an
 * obstacle by this entity.
 *
 * This function returns \c false by default.
 *
 * \return \c true if ice ground is currently obstacle for this entity.
 */
bool Entity::is_ice_obstacle() const {
    return false;
}

/**
 * \brief Returns whether prickles are currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if prickles are currently an obstacle for this entity
 */
bool Entity::is_prickle_obstacle() const {
  return true;
}

/**
 * \brief Returns whether a ladder is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \return true if the ladders are currently an obstacle for this entity
 */
bool Entity::is_ladder_obstacle() const {
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
bool Entity::is_hero_obstacle(Hero& /* hero */) {
  return false;
}

/**
 * \brief Returns whether a block is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * \param block a block
 * \return true if the block is currently an obstacle for this entity
 */
bool Entity::is_block_obstacle(Block& /* block */) {
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
bool Entity::is_teletransporter_obstacle(Teletransporter& /* teletransporter */) {
  return true;
}

/**
 * \brief Returns whether a stream is currently considered as an obstacle by
 * this entity.
 *
 * This function returns true by default.
 *
 * \param stream A stream.
 * \return \c true if the stream is currently an obstacle for this entity.
 */
bool Entity::is_stream_obstacle(Stream& /* stream */) {
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
bool Entity::is_stairs_obstacle(Stairs& /* stairs */) {
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
bool Entity::is_sensor_obstacle(Sensor& /* sensor */) {
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
bool Entity::is_switch_obstacle(Switch& sw) {
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
bool Entity::is_raised_block_obstacle(CrystalBlock& /* raised_block */) {
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
bool Entity::is_crystal_obstacle(Crystal& /* crystal */) {
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
bool Entity::is_npc_obstacle(Npc& npc) {
  return !npc.is_traversable();
}

/**
 * \brief Returns whether a door is currently considered as an obstacle
 * by this entity.
 *
 * By default, this function returns \c true unless the door is open.
 *
 * \param door A door.
 * \return \c true if the door is currently an obstacle for this entity.
 */
bool Entity::is_door_obstacle(Door& door) {
  return !door.is_open();
}

/**
 * \brief Returns whether an enemy is currently considered as an obstacle by this entity.
 *
 * This function returns false by default.
 *
 * \param enemy an enemy
 * \return true if the enemy is currently an obstacle for this entity
 */
bool Entity::is_enemy_obstacle(Enemy& /* enemy */) {
  return false;
}

/**
 * \brief Returns whether a jumper is currently considered as an obstacle by this entity.
 * \param jumper A jumper.
 * \param candidate_position Candidate position of this entity to test.
 * \return \c true if the jumper is currently an obstacle for this entity at
 * this candidate position.
 */
bool Entity::is_jumper_obstacle(Jumper& /* jumper */, const Rectangle& /* candidate_position */) {
  return true;
}

/**
 * \brief Returns whether a destructible object is currently considered as an obstacle by this entity.
 *
 * By default, this function returns true.
 *
 * \param destructible a destructible item
 * \return true if the destructible item is currently an obstacle for this entity
 */
bool Entity::is_destructible_obstacle(Destructible& /* destructible */) {

  return true;
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
bool Entity::is_separator_obstacle(Separator& /* separator */) {

  return true;
}

/**
 * \brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 *
 * \return true if the sword is ignored
 */
bool Entity::is_sword_ignored() const {
  return false;
}

/**
 * \brief Returns whether the bounding box or a sprite of this entity overlaps
 * the visible part of the map
 * \return true if the entity is in the visible part of the map
 */
bool Entity::overlaps_camera() const {

  const CameraPtr& camera = get_map().get_camera();
  if (camera == nullptr) {
    return false;
  }

  if (camera->overlaps(bounding_box)) {
    return true;
  }

  for (const NamedSprite& named_sprite: sprites) {
    if (named_sprite.removed) {
      continue;
    }
    Sprite& sprite = *named_sprite.sprite;
    const Size& sprite_size = sprite.get_size();
    const Point& sprite_origin = sprite.get_origin();
    const Rectangle sprite_bounding_box(
        get_x() - sprite_origin.x,
        get_y() - sprite_origin.y,
        sprite_size.width,
        sprite_size.height
    );
    if (camera->overlaps(sprite_bounding_box)) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Returns whether or not this entity's origin point is in
 * the specified rectangle.
 * \param rectangle the rectangle to check
 * \return true if this entity's origin point is in the specified rectangle
 */
bool Entity::is_origin_point_in(const Rectangle& rectangle) const {
  return rectangle.contains(get_xy());
}

/**
 * \brief Returns whether this entity's facing point is in
 * the specified rectangle.
 * \param rectangle the rectangle to check
 * \return true if this entity's facing point is in the specified rectangle
 */
bool Entity::is_facing_point_in(const Rectangle& rectangle) const {

  const Point& facing_point = get_facing_point();
  return rectangle.contains(facing_point);
}

/**
 * \brief Returns whether a touching point of this entity is in
 * the specified rectangle.
 * \param rectangle the rectangle to check
 * \param direction direction of the touching point to consider (0 to 3)
 * \return true if this touching point is in the specified rectangle
 */
bool Entity::is_touching_point_in(
    const Rectangle& rectangle, int direction) const {

  const Point& touching_point = get_touching_point(direction);
  return rectangle.contains(touching_point);
}

/**
 * \brief Returns whether or not this entity's center is in
 * the specified rectangle.
 * \param rectangle the rectangle to check
 * \return true if this entity's center is in the specified rectangle
 */
bool Entity::is_center_in(const Rectangle& rectangle) const {

  const Point& center = get_center_point();
  return rectangle.contains(center);
}

/**
 * \brief Returns the angle of the vector between the origin of this entity
 * and a point.
 * \param x X coordinate of the point.
 * \param y Y coordinate of the point.
 * \return The angle of the vector in radians, between 0 and Geometry::TWO_PI.
 */
double Entity::get_angle(int x, int y) const {
  return Geometry::get_angle(get_x(), get_y(), x, y);
}

/**
 * \brief Returns the angle of the vector between the origin of this entity
 * and the origin of another entity.
 * \param other The other entity.
 * \return The angle of the vector in radians, between 0 and Geometry::TWO_PI.
 */
double Entity::get_angle(const Entity& other) const {
  return Geometry::get_angle(get_xy(), other.get_xy());
}

/**
 * \brief Returns the angle of the vector between the origin of this entity or
 * one of its sprite and the origin of another entity or one of its sprites.
 * \param other The other entity.
 * \param this_sprite Sprite of this entity to use instead of the entity itself
 * or nullptr.
 * \param other_sprite Sprite of the other entity to use instead of the entity
 * itself or nullptr.
 * \return The angle of the vector in radians, between 0 and Geometry::TWO_PI.
 */
double Entity::get_angle(
    const Entity& other,
    const Sprite* this_sprite,
    const Sprite* other_sprite
) const {

  // Add the coordinates of sprites as offsets.
  Point this_offset;
  if (this_sprite != nullptr) {
    this_offset += this_sprite->get_xy();
  }

  Point other_offset;
  if (other_sprite != nullptr) {
    other_offset += other_sprite->get_xy();
  }

  return Geometry::get_angle(
      get_x() + this_offset.x,
      get_y() + this_offset.y,
      other.get_x() + other_offset.x,
      other.get_y() + other_offset.y
  );
}

/**
 * \brief Returns the distance between the origin of this entity and a point.
 * \param x x coordinate of the point
 * \param y y coordinate of the point
 * \return the distance between this entity and the point in pixels
 */
int Entity::get_distance(int x, int y) const {
  return (int) Geometry::get_distance(get_x(), get_y(), x, y);
}

/**
 * \brief Returns the distance between the origin of this entity and a point.
 * \param point coordinates of the point
 * \return the distance between this entity and the point in pixels
 */
int Entity::get_distance(const Point& point) const {
  return (int) Geometry::get_distance(get_xy(), point);
}

/**
 * \brief Returns the distance between the origin of this entity
 * and the origin of another entity.
 * \param other the other entity
 * \return the distance between the two entities in pixels
 */
int Entity::get_distance(const Entity& other) const {
  return (int) Geometry::get_distance(get_xy(), other.get_xy());
}

/**
 * \brief Returns whether this entity is in the same region as another one.
 *
 * Regions are defined by the position of separators on the map.
 * The region of entities is the one of their center point.
 *
 * \param other Another entity.
 * \return \c true if both entities are in the same region.
 */
bool Entity::is_in_same_region(const Entity& other) const {

    return is_in_same_region(other.get_center_point());
}

/**
 * \brief Returns whether this entity is in the same region as a point.
 *
 * Regions are defined by the position of separators on the map.
 * The region of this entity is the one of its center point.
 *
 * \param xy A point.
 * \return \c true if this entity and the point are in the same region.
 */
bool Entity::is_in_same_region(const Point& xy) const {

  const Point& this_xy = get_center_point();
  const Point& other_xy = xy;

  const std::set<ConstSeparatorPtr>& separators =
      get_entities().get_entities_by_type<Separator>();
  for (const ConstSeparatorPtr& separator: separators) {

    if (separator->is_vertical()) {
      // Vertical separation.
      if (this_xy.y < separator->get_top_left_y() ||
          this_xy.y >= separator->get_top_left_y() + separator->get_height()) {
        // This separator is irrelevant: the entity is not in either side,
        // it is too much to the north or to the south.
        //
        //     |
        //     |
        //     |
        //
        //  x
        //
        continue;
      }

      if (other_xy.y < separator->get_top_left_y() ||
          other_xy.y >= separator->get_top_left_y() + separator->get_height()) {
        // This separator is irrelevant: the other entity is not in either side.
        // it is too much to the north or to the south.
        continue;
      }

      // Both entities are in the zone of influence of this separator.
      // See if they are in the same side.
      const int separation_x = separator->get_center_point().x;
      if (this_xy.x < separation_x &&
          separation_x <= other_xy.x) {
        // Different side.
        return false;
      }

      if (other_xy.x < separation_x &&
          separation_x <= this_xy.x) {
        // Different side.
        return false;
      }
    }
    else {
      // Horizontal separation.
      if (this_xy.x < separator->get_top_left_x() ||
          this_xy.x >= separator->get_top_left_x() + separator->get_width()) {
        continue;
      }

      if (other_xy.x < separator->get_top_left_x() ||
          other_xy.x >= separator->get_top_left_x() + separator->get_width()) {
        continue;
      }

      const int separation_y = separator->get_center_point().y;
      if (this_xy.y < separation_y &&
          separation_y <= other_xy.y) {
        return false;
      }

      if (other_xy.y < separation_y &&
          separation_y <= this_xy.y) {
        return false;
      }
    }
  }

  return true;
}

/**
 * \brief This function is called when the position, movement or state changes.
 *
 * Subclasses can redefine it to perform some checks.
 */
void Entity::check_position() {
}

/**
 * \brief This function is called when a destructible item detects a non-pixel perfect collision with this entity.
 * \param destructible the destructible item
 * \param collision_mode the collision mode that detected the event
 */
void Entity::notify_collision_with_destructible(Destructible& /* destructible */, CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when a teletransporter detects a collision with this entity.
 * \param teletransporter the teletransporter
 * \param collision_mode the collision mode that detected the event
 */
void Entity::notify_collision_with_teletransporter(Teletransporter& /* teletransporter */, CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when a stream detects a collision with this
 * entity.
 * \param stream A stream.
 * \param dx Direction of the x move in pixels (0, 1 or -1).
 * \param dy Direction of the y move in pixels (0, 1 or -1).
 */
void Entity::notify_collision_with_stream(
    Stream& /* stream */, int /* dx */, int /* dy */) {
}

/**
 * \brief This function is called when a stairs entity detects a collision with this entity.
 * \param stairs the stairs
 * \param collision_mode the collision mode that detected the event
 */
void Entity::notify_collision_with_stairs(Stairs& /* stairs */, CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when a jumper detects a collision with this entity.
 * \param jumper The jumper.
 * \param collision_mode The collision mode that detected the event.
 */
void Entity::notify_collision_with_jumper(Jumper& /* jumper */, CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when a sensor detects a collision with this entity.
 * \param sensor a sensor
 * \param collision_mode the collision mode that detected the collision
 */
void Entity::notify_collision_with_sensor(Sensor& /* sensor */, CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when a switch detects a collision with this entity.
 * \param sw a switch
 * \param collision_mode the collision mode that detected the event
 */
void Entity::notify_collision_with_switch(Switch& /* sw */,
    CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when the sprite of a switch
 * detects a pixel-precise collision with a sprite of this entity.
 * \param sw the switch
 * \param sprite_overlapping the sprite of the current entity that collides with the switch
 */
void Entity::notify_collision_with_switch(Switch& /* sw */, Sprite& /* sprite_overlapping */) {
}

/**
 * \brief This function is called when a crystal detects a collision with this entity.
 * \param crystal the crystal
 * \param collision_mode the collision mode that detected the event
 */
void Entity::notify_collision_with_crystal(Crystal& /* crystal */,
    CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when the sprite of a crystal
 * detects a pixel-precise collision with a sprite of this entity.
 * \param crystal the crystal
 * \param sprite_overlapping the sprite of the current entity that collides with the crystal
 */
void Entity::notify_collision_with_crystal(Crystal& /* crystal */, Sprite& /* sprite_overlapping */) {
}

/**
 * \brief This function is called when a chest detects a collision with this entity.
 * \param chest the chest
 */
void Entity::notify_collision_with_chest(Chest& /* chest */) {
}

/**
 * \brief This function is called when a block detects a collision with this entity.
 * \param block the block
 */
void Entity::notify_collision_with_block(Block& /* block */) {
}

/**
 * \brief This function is called when a separator detects a collision with
 * this entity.
 * \param separator The separator.
 * \param collision_mode The collision mode that detected the event.
 */
void Entity::notify_collision_with_separator(
    Separator& /* separator */, CollisionMode /* collision_mode */) {

}

/**
 * \brief This function is called when bomb detects a collision with a this entity.
 * \param bomb the bomb
 * \param collision_mode the collision mode that detected the event
 */
void Entity::notify_collision_with_bomb(Bomb& /* bomb */, CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when an explosion detects a collision with this entity.
 * \param explosion the explosion
 * \param collision_mode the collision mode that detected the event
 */
void Entity::notify_collision_with_explosion(Explosion& /* explosion */,
    CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when an explosion's sprite
 * detects a pixel-perfect collision with a sprite of this entity.
 * \param explosion the explosion
 * \param sprite_overlapping the sprite of the current entity that collides with the explosion
 */
void Entity::notify_collision_with_explosion(Explosion& /* explosion */, Sprite& /* sprite_overlapping */) {
}

/**
 * \brief Notifies this entity that a sprite of fire
 * detects a pixel-perfect collision with a sprite of this entity.
 * \param fire the fire
 * \param sprite_overlapping the sprite of the current entity that collides with the fire
 */
void Entity::notify_collision_with_fire(Fire& /* fire */, Sprite& /* sprite_overlapping */) {
}

/**
 * \brief This function is called when an enemy detects a collision with this entity.
 * \param enemy The enemy.
 * \param collision_mode The collision mode that detected the event.
 */
void Entity::notify_collision_with_enemy(Enemy& /* enemy */, CollisionMode /* collision_mode */) {
}

/**
 * \brief This function is called when an enemy's sprite collides with a sprite of this entity.
 * \param enemy the enemy
 * \param enemy_sprite the enemy's sprite that overlaps a sprite of this entity
 * \param this_sprite this entity's sprite that overlaps the enemy's sprite
 */
void Entity::notify_collision_with_enemy(Enemy& /* enemy */, Sprite& /* enemy_sprite */, Sprite& /* this_sprite */) {
}

/**
 * \brief Notifies this entity that it has just attacked an enemy.
 *
 * This function is called even if this attack was not successful.
 *
 * \param attack The attack.
 * \param victim The enemy just hurt.
 * \param victim_sprite The enemy's sprite that was touched or nullptr.
 * \param result How the enemy has reacted to the attack.
 * \param killed Whether the attack has just killed the enemy.
 */
void Entity::notify_attacked_enemy(
    EnemyAttack /* attack */,
    Enemy& /* victim */,
    const Sprite* /* victim_sprite */,
    EnemyReaction::Reaction& /* result */,
    bool /* killed */) {
}

/**
 * \brief Notifies this entity that the player is interacting with it by
 * pressing the action command.
 *
 * Only possible if this entity detects collisions of type
 * CollisionMode::COLLISION_FACING.
 *
 * This function is called when the player presses the action command
 * while the hero is facing this entity, unless the action command effect
 * does not allow the hero to interact with the entity, like while he is
 * carrying an object.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make the hero interact with
 * this entity.
 *
 * \return \c true if an interaction happened.
 */
bool Entity::notify_action_command_pressed() {
  return false;
}

/**
 * \brief Notifies this entity that the player is interacting with it by
 * using an equipment item.
 *
 * Only possible if this entity detects collisions of type
 * CollisionMode::COLLISION_FACING.
 *
 * This function is called when the player uses an equipment item
 * while the hero is facing this entity.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make your entity react to an
 * equipment item.
 *
 * \param item The equipment item used.
 * \return \c true if an interaction occurred.
 */
bool Entity::notify_interaction_with_item(EquipmentItem& /* item */) {
  return false;
}

/**
 * \brief This function is called when the player tries to push or pull this
 * entity.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make something happen with the entity.
 *
 * \return \c true if this entity was pushed or pulled successfully.
 */
bool Entity::start_movement_by_hero() {
  return false;
}

/**
 * \brief This function is called when the player finishes to push or pull
 * this entity.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make something happen with the entity.
 */
void Entity::stop_movement_by_hero() {
}

/**
 * \brief This function is called when the player is tapping his sword against this entity.
 * \return The sound to play when tapping this entity with the sword.
 */
std::string Entity::get_sword_tapping_sound() {
  return "sword_tapping";
}

/**
 * \brief Returns whether the movement and the animations of this entity are suspended.
 * \return true if the movement and the animations are suspended
 */
bool Entity::is_suspended() const {
  return suspended;
}

/**
 * \brief Suspends or resumes the movement and the animations of this entity.
 * \param suspended true to suspend the movement and the animations, false to resume them
 */
void Entity::set_suspended(bool suspended) {

  this->suspended = suspended;

  // Remember the date if the entity is being suspended.
  if (suspended) {
    when_suspended = System::now();
  }

  // Suspend/unsuspend sprite animations.
  for (const NamedSprite& named_sprite: sprites) {
    if (named_sprite.removed) {
      continue;
    }
    Sprite& sprite = *named_sprite.sprite;
    sprite.set_suspended(suspended || !is_enabled());
  }

  // Suspend/unsuspend the movement.
  if (movement != nullptr) {
    movement->set_suspended(suspended || !is_enabled());
  }
  if (stream_action != nullptr) {
    stream_action->set_suspended(suspended || !is_enabled());
  }

  // Suspend/unsuspend timers.
  if (is_on_map()) {
    get_lua_context()->set_entity_timers_suspended(*this, suspended || !is_enabled());
  }

  if (!suspended) {
    // Collision tests were disabled when the entity was suspended.
    if (is_on_map()) {
      get_map().check_collision_from_detector(*this);
      check_collision_with_detectors();
    }
  }
}

/**
 * \brief Returns the date when this entity was suspended.
 * \return When this entity was suspended.
 */
uint32_t Entity::get_when_suspended() const {
  return when_suspended;
}

/**
 * \brief Makes this entity's sprites play their animation even when the game is suspended.
 * \param ignore_suspend true to keep playing the sprites when the game is suspended
 */
void Entity::set_animation_ignore_suspend(bool ignore_suspend) {

  for (const NamedSprite& named_sprite: sprites) {
    if (named_sprite.removed) {
      continue;
    }
    Sprite& sprite = *named_sprite.sprite;
    sprite.set_ignore_suspend(ignore_suspend);
  }
}

/**
 * \brief Updates the entity.
 *
 * This function is called repeatedly by the map.
 * By default, it updates the position
 * of the entity according to its movement (if any),
 * and it updates the sprites frames if there are sprites.
 * Redefine it in subclasses for the entities that should be updated
 * for other treatments but don't forget to call this method
 * to handle the movement and the sprites.
 */
void Entity::update() {

  // Static tiles are optimized and should never be used individually
  // once the map is created.
  SOLARUS_ASSERT(get_type() != EntityType::TILE,
      "Attempt to update a static tile");

  if (is_being_removed()) {
    return;
  }

  // Check the facing entity.
  if (facing_entity != nullptr && facing_entity->is_being_removed()) {
    set_facing_entity(nullptr);
  }

  // Update the sprites.
  std::vector<NamedSprite> sprites = this->sprites;
  for (const NamedSprite& named_sprite: sprites) {
    if (named_sprite.removed) {
      continue;
    }
    Sprite& sprite = *named_sprite.sprite;

    sprite.update();
    if (sprite.has_frame_changed()) {
      // The frame has just changed.
      // Pixel-precise collisions need to be rechecked.
      if (sprite.are_pixel_collisions_enabled()) {

        if (is_detector()) {
          // Since this entity is a detector, all entities need to check
          // their pixel-precise collisions with it.
          get_map().check_collision_from_detector(*this, sprite);
        }

        check_collision_with_detectors(sprite);
      }

      notify_sprite_frame_changed(sprite, sprite.get_current_animation(), sprite.get_current_frame());
      if (sprite.is_animation_finished()) {
        notify_sprite_animation_finished(sprite, sprite.get_current_animation());
      }
    }
  }
  clear_old_sprites();

  // Update the movement.
  if (movement != nullptr) {
    movement->update();
  }
  clear_old_movements();
  if (stream_action != nullptr) {
    stream_action->update();
    if (!get_stream_action()->is_active()) {
      stop_stream_action();
    }
  }

  // Update the state if any.
  update_state();
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
bool Entity::is_drawn_at_its_position() const {
  return true;
}

/**
 * \brief Draws the entity on the map.
 *
 * By default, this function draws the entity's sprites (if any) and if
 * at least one of them is in the visible part of the map.
 * This function should do nothing if is_drawn() is false.
 */
void Entity::draw_on_map() {

  // Draw the sprites.
  for (const NamedSprite& named_sprite: sprites) {
    if (named_sprite.removed) {
      continue;
    }
    Sprite& sprite = *named_sprite.sprite;
    get_map().draw_visual(sprite, get_displayed_xy());
  }
}

/**
 * \brief Returns the current state of this entity.
 * \return The state.
 */
Entity::State& Entity::get_state() const {
    return *state.get();
}

/**
 * \brief Changes the entity's internal state.
 *
 * This function stops the old state and starts the new one.
 * The old state will also be automatically destroyed, but not right now,
 * in order to allow this function to be called by the old state itself safely.
 *
 * \param state The new state of the hero. The hero object takes ownership of
 * this object.
 */
void Entity::set_state(State* new_state) {

  // Stop the previous state.
  State* old_state = this->state.get();
  if (old_state != nullptr) {

    old_state->stop(new_state);  // Should not change the state again.

    // Sanity check.
    if (old_state != this->state.get()) {
      // old_state->stop() called set_state() again in the meantime.
      // This is not a normal situation since we only called stop() to allow
      // new_state to start.
      Debug::error(std::string("Entity state '") + old_state->get_name()
                + "' did not stop properly to let state '" + new_state->get_name()
                + "' go, it started state '" + this->state->get_name() + "' instead. "
                + "State '" + new_state->get_name() + "' will be forced.");

      // Let's start the state that was supposed to start in the first place.
      // Note that old_state is already in the old_states list.
      set_state(new_state);
      return;
    }
  }

  // Don't delete the previous state immediately since it may be the caller
  // of this function.
  this->old_states.emplace_back(std::move(this->state));

  this->state = std::unique_ptr<State>(new_state);
  this->state->start(old_state);  // May also change the state again.

  if (this->state.get() == new_state) {
    // If the state has not already changed again.
    check_position();
  }
}

/**
 * \brief Returns the name of the entity's internal state.
 * \return A name describing the current state of the entity.
 * Returns an empty string if this entity has no state.
 */
std::string Entity::get_state_name() const {

  if (state == nullptr) {
    return "";
  }
  return state->get_name();
}

/**
 * \brief Updates the entity's internal state if any.
 *
 * This function is called repeatedly by the game loop.
 */
void Entity::update_state() {

  // Update the current state
  if (state != nullptr) {
    state->update();
  }

  // Cleanup old states.
  old_states.clear();
}

}
