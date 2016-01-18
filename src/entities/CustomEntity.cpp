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
#include "solarus/entities/CustomEntity.h"
#include "solarus/entities/Block.h"
#include "solarus/entities/Bomb.h"
#include "solarus/entities/Stream.h"
#include "solarus/entities/Chest.h"
#include "solarus/entities/Crystal.h"
#include "solarus/entities/CrystalBlock.h"
#include "solarus/entities/Destructible.h"
#include "solarus/entities/Enemy.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/Fire.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Jumper.h"
#include "solarus/entities/Npc.h"
#include "solarus/entities/Sensor.h"
#include "solarus/entities/Separator.h"
#include "solarus/entities/Stairs.h"
#include "solarus/entities/Switch.h"
#include "solarus/entities/Teletransporter.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Sprite.h"
#include <lua.hpp>

namespace Solarus {

/**
 * \brief Creates a custom entity.
 * \param game The game.
 * \param name Name of the entity or an empty string.
 * \param direction Direction of the entity (0 to 3).
 * \param layer The layer on the map.
 * \param xy Coordinates on the map.
 * \param size Size of the entity.
 * \param sprite_name Animation set id of a sprite or an empty string.
 * \param model Model of custom entity or an empty string.
 */
CustomEntity::CustomEntity(
    Game& /* game */,
    const std::string& name,
    int direction,
    int layer,
    const Point& xy,
    const Size& size,
    const std::string& sprite_name,
    const std::string& model
):
  Entity(
      name, 0, layer, xy, size
  ),
  model(model),
  ground_observer(false),
  modified_ground(Ground::EMPTY) {

  set_collision_modes(CollisionMode::COLLISION_FACING);
  set_origin(8, 13);

  if (!sprite_name.empty()) {
    create_sprite(sprite_name);
  }
  set_sprites_direction(direction);
}

/**
 * \brief Returns the type of entity.
 * \return The type of entity.
 */
EntityType CustomEntity::get_type() const {
  return ThisType;
}

/**
 * \brief Returns the model of this custom entity if any.
 * \return The model or an empty string.
 */
const std::string& CustomEntity::get_model() const {
  return model;
}

/**
 * \brief Returns the direction of this custom entity.
 *
 * This is the direction applied to the sprites unless it is overridden
 * for particular sprites.
 *
 * \return The direction.
 */
int CustomEntity::get_sprites_direction() const {

  return get_direction();
}

/**
 * \brief Sets the direction of this custom entity.
 *
 * It will be applied to sprites that have such a direction.
 *
 * \param direction The direction.
 */
void CustomEntity::set_sprites_direction(int direction) {

  set_direction(direction);

  for (const SpritePtr& sprite: get_sprites()) {
    if (direction >= 0 && direction < sprite->get_nb_directions()) {
      sprite->set_current_direction(direction);
    }
  }
}

/**
 * \brief Returns the info about whether this custom entity can be traversed
 * by a type of entity.
 * \param type Type of entity to test.
 * \return The corresponding traversable property.
 */
const CustomEntity::TraversableInfo& CustomEntity::get_traversable_by_entity_info(
    EntityType type) {

  const auto it = traversable_by_entities_type.find(type);
  if (it != traversable_by_entities_type.end()) {
    // This entity type overrides the general setting.
    return it->second;
  }

  return traversable_by_entities_general;
}

/**
 * \brief Sets whether this custom entity can be traversed by the specified
 * entity.
 * \param entity The entity to test.
 * \return \c true if the entity can traverse this custom entity.
 */
bool CustomEntity::is_traversable_by_entity(Entity& entity) {

  // Find the obstacle settings.
  const TraversableInfo& info = get_traversable_by_entity_info(entity.get_type());

  if (info.is_empty()) {
    // Nothing was set: make the custom entity traversable by default.
    return true;
  }

  return info.is_traversable(*this, entity);
}

/**
 * \brief Sets whether this custom entity can be traversed by other entities.
 *
 * This applies to entities that are not overridden by
 * set_traversable_by_entities(EntityType, bool)
 * or
 * set_traversable_by_entities(EntityType, const ScopedLuaRef&).
 *
 * \param traversable \c true to allow other entities to traverse this custom
 * entity.
 */
void CustomEntity::set_traversable_by_entities(bool traversable) {

  traversable_by_entities_general = TraversableInfo(
      *get_lua_context(),
      traversable
  );
}

/**
 * \brief Registers a Lua function to decide if this custom entity can be
 * traversed by other entities.
 *
 * This applies to entities that are not overridden by
 * set_traversable_by_entities(EntityType, bool)
 * or
 * set_traversable_by_entities(EntityType, const ScopedLuaRef&).
 *
 * \param traversable_test_ref Lua ref to a function that will do the test.
 */
void CustomEntity::set_traversable_by_entities(
    const ScopedLuaRef& traversable_test_ref
) {
  traversable_by_entities_general = TraversableInfo(
      *get_lua_context(),
      traversable_test_ref
  );
}

/**
 * \brief Restores the default setting of whether this custom entity can be
 * traversed by other entities.
 *
 * This reverts the settings of previous calls to
 * set_traversable_by_entities(bool)
 * and
 * set_traversable_by_entities(const ScopedLuaRef&).
 */
void CustomEntity::reset_traversable_by_entities() {

  traversable_by_entities_general = TraversableInfo();
}

/**
 * \brief Sets whether this custom entity can be traversed by other entities
 * of the specified type.
 *
 * This overrides for a specific type whatever was set by
 * set_traversable_by_entities(bool)
 * or
 * set_traversable_by_entities(const ScopedLuaRef&).
 *
 * \param type A type of entities.
 * \param traversable \c true to allow other entities to traverse this custom
 * entity.
 */
void CustomEntity::set_traversable_by_entities(
    EntityType type, bool traversable) {

  traversable_by_entities_type[type] = TraversableInfo(
      *get_lua_context(),
      traversable
  );
}

/**
 * \brief Registers a Lua function to decide if this custom entity can be
 * traversed by other entities of the specified type.
 *
 * This overrides for a specific type whatever was set by
 * set_traversable_by_entities(bool)
 * or
 * set_traversable_by_entities(const ScopedLuaRef&).
 *
 * \param type A type of entities.
 * \param traversable_test_ref Lua ref to a function that will do the test.
 */
void CustomEntity::set_traversable_by_entities(
    EntityType type,
    const ScopedLuaRef& traversable_test_ref
) {
  traversable_by_entities_type[type] = TraversableInfo(
      *get_lua_context(),
      traversable_test_ref
  );
}

/**
 * \brief Restores the default setting of whether this custom entity can be
 * traversed by other entities of the specified type.
 *
 * This reverts the settings of previous calls to
 * set_traversable_by_entities(EntityType, bool)
 * and
 * set_traversable_by_entities(EntityType, const ScopedLuaRef&).
 *
 * \param type A type of entities.
 */
void CustomEntity::reset_traversable_by_entities(EntityType type) {

  traversable_by_entities_type.erase(type);
}

/**
 * \copydoc Entity::is_obstacle_for
 */
bool CustomEntity::is_obstacle_for(Entity& other) {

  return !is_traversable_by_entity(other);
}

/**
 * \brief Returns the info about whether this custom entity can traverse a
 * type of entity.
 * \param type Type of entity to test.
 * \return The corresponding traversable property.
 */
const CustomEntity::TraversableInfo& CustomEntity::get_can_traverse_entity_info(
    EntityType type) {

  // Find the obstacle settings.
  const auto it = can_traverse_entities_type.find(type);
  if (it != can_traverse_entities_type.end()) {
    // This entity type overrides the general setting.
    return it->second;
  }

  return can_traverse_entities_general;
}

/**
 * \brief Sets whether this custom entity can traverse other entities.
 *
 * This applies to entities that are not overridden by
 * set_can_traverse_entities(EntityType, bool)
 * or
 * set_can_traverse_entities(EntityType, const ScopedLuaRef&).
 *
 * \param traversable \c true to allow this entity to traverse other entities.
 */
void CustomEntity::set_can_traverse_entities(bool traversable) {

  can_traverse_entities_general = TraversableInfo(
      *get_lua_context(),
      traversable
  );
}

/**
 * \brief Registers a Lua function to decide if this custom entity can traverse
 * other entities.
 *
 * This applies to entities that are not overridden by
 * set_can_traverse_entities(EntityType, bool)
 * or
 * set_can_traverse_entities(EntityType, const ScopedLuaRef&).
 *
 * \param traversable_test_ref Lua ref to a function that will do the test.
 */
void CustomEntity::set_can_traverse_entities(const ScopedLuaRef& traversable_test_ref) {

  can_traverse_entities_general = TraversableInfo(
      *get_lua_context(),
      traversable_test_ref
  );
}

/**
 * \brief Restores the default setting of whether this custom entity can
 * traverse other entities.
 *
 * This reverts the settings of previous calls to
 * set_can_traverse_entities(bool)
 * and
 * set_can_traverse_entities(const ScopedLuaRef&).
 */
void CustomEntity::reset_can_traverse_entities() {

  can_traverse_entities_general = TraversableInfo();
}

/**
 * \brief Sets whether this custom entity can traverse other entities
 * of the specified type.
 *
 * This overrides for a specific type whatever was set by
 * set_can_traverse_entities(bool)
 * or
 * set_can_traverse_entities(const ScopedLuaRef&).
 *
 * \param type A type of entities.
 * \param traversable \c true to allow this entity to traverse other entities
 * of the specified type.
 */
void CustomEntity::set_can_traverse_entities(
    EntityType type,
    bool traversable
) {

  can_traverse_entities_type[type] = TraversableInfo(
      *get_lua_context(),
      traversable
  );
}

/**
 * \brief Registers a Lua function to decide if this custom entity can
 * traverse other entities of the specified type.
 *
 * This overrides for a specific type whatever was set by
 * set_can_traverse_entities(bool)
 * or
 * set_can_traverse_entities(const ScopedLuaRef&).
 *
 * \param type A type of entities.
 * \param traversable_test_ref Lua ref to a function that will do the test.
 */
void CustomEntity::set_can_traverse_entities(
    EntityType type,
    const ScopedLuaRef& traversable_test_ref
) {

  can_traverse_entities_type[type] = TraversableInfo(
      *get_lua_context(),
      traversable_test_ref
  );
}

/**
 * \brief Restores the default setting of whether this custom entity can
 * traverse other entities of the specified type.
 callback_ref This reverts the settings of previous calls to
 * set_can_traverse_entities(EntityType, bool)
 * and
 * set_can_traverse_entities(EntityType, const ScopedLuaRef&).
 *
 * \param type A type of entities.
 */
void CustomEntity::reset_can_traverse_entities(EntityType type) {

  can_traverse_entities_type.erase(type);
}

/**
 * \copydoc Entity::is_hero_obstacle
 */
bool CustomEntity::is_hero_obstacle(Hero& hero) {

  const TraversableInfo& info = get_can_traverse_entity_info(hero.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, hero);
  }
  return Entity::is_hero_obstacle(hero);
}

/**
 * \copydoc Entity::is_block_obstacle
 */
bool CustomEntity::is_block_obstacle(Block& block) {

  const TraversableInfo& info = get_can_traverse_entity_info(block.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, block);
  }
  return Entity::is_block_obstacle(block);
}

/**
 * \copydoc Entity::is_teletransporter_obstacle
 */
bool CustomEntity::is_teletransporter_obstacle(Teletransporter& teletransporter) {

  const TraversableInfo& info = get_can_traverse_entity_info(teletransporter.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, teletransporter);
  }
  return Entity::is_teletransporter_obstacle(teletransporter);
}

/**
 * \copydoc Entity::is_stream_obstacle
 */
bool CustomEntity::is_stream_obstacle(Stream& stream) {

  const TraversableInfo& info = get_can_traverse_entity_info(stream.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, stream);
  }
  return Entity::is_stream_obstacle(stream);
}

/**
 * \copydoc Entity::is_stairs_obstacle
 */
bool CustomEntity::is_stairs_obstacle(Stairs& stairs) {

  const TraversableInfo& info = get_can_traverse_entity_info(stairs.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, stairs);
  }
  return Entity::is_stairs_obstacle(stairs);
}

/**
 * \copydoc Entity::is_sensor_obstacle
 */
bool CustomEntity::is_sensor_obstacle(Sensor& sensor) {

  const TraversableInfo& info = get_can_traverse_entity_info(sensor.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, sensor);
  }
  return Entity::is_sensor_obstacle(sensor);
}

/**
 * \copydoc Entity::is_switch_obstacle
 */
bool CustomEntity::is_switch_obstacle(Switch& sw) {

  const TraversableInfo& info = get_can_traverse_entity_info(sw.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, sw);
  }
  return Entity::is_switch_obstacle(sw);
}

/**
 * \copydoc Entity::is_raised_block_obstacle
 */
bool CustomEntity::is_raised_block_obstacle(CrystalBlock& raised_block) {

  const TraversableInfo& info = get_can_traverse_entity_info(raised_block.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, raised_block);
  }
  return Entity::is_raised_block_obstacle(raised_block);
}

/**
 * \copydoc Entity::is_crystal_obstacle
 */
bool CustomEntity::is_crystal_obstacle(Crystal& crystal) {

  const TraversableInfo& info = get_can_traverse_entity_info(crystal.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, crystal);
  }
  return Entity::is_crystal_obstacle(crystal);
}

/**
 * \copydoc Entity::is_npc_obstacle
 */
bool CustomEntity::is_npc_obstacle(Npc& npc) {

  const TraversableInfo& info = get_can_traverse_entity_info(npc.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, npc);
  }
  return Entity::is_npc_obstacle(npc);
}

/**
 * \copydoc Entity::is_enemy_obstacle
 */
bool CustomEntity::is_enemy_obstacle(Enemy& enemy) {

  const TraversableInfo& info = get_can_traverse_entity_info(enemy.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, enemy);
  }
  return Entity::is_enemy_obstacle(enemy);
}

/**
 * \copydoc Entity::is_jumper_obstacle
 */
bool CustomEntity::is_jumper_obstacle(Jumper& jumper, const Rectangle& candidate_position) {

  const TraversableInfo& info = get_can_traverse_entity_info(jumper.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, jumper);
  }
  return Entity::is_jumper_obstacle(jumper, candidate_position);
}

/**
 * \copydoc Entity::is_destructible_obstacle
 */
bool CustomEntity::is_destructible_obstacle(Destructible& destructible) {

  const TraversableInfo& info = get_can_traverse_entity_info(destructible.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, destructible);
  }
  return Entity::is_destructible_obstacle(destructible);
}

/**
 * \copydoc Entity::is_separator_obstacle
 */
bool CustomEntity::is_separator_obstacle(Separator& separator) {

  const TraversableInfo& info = get_can_traverse_entity_info(separator.get_type());
  if (!info.is_empty()) {
    return !info.is_traversable(*this, separator);
  }
  return Entity::is_separator_obstacle(separator);
}

/**
 * \brief Returns whether this custom entity can traverse a kind of ground.
 * \param ground A kind of ground.
 * \return \c true if this custom entity can traverse this kind of ground.
 */
bool CustomEntity::can_traverse_ground(Ground ground) const {

  const auto& it = can_traverse_grounds.find(ground);

  if (it != can_traverse_grounds.end()) {
    // Return the boolean value that was set.
    return it->second;
  }

  // Return a default value.
  switch (ground) {

    case Ground::EMPTY:
    case Ground::TRAVERSABLE:
    case Ground::GRASS:
    case Ground::ICE:
    case Ground::LADDER:
      return true;

    case Ground::WALL:
    case Ground::LOW_WALL:
    case Ground::WALL_TOP_RIGHT:
    case Ground::WALL_TOP_LEFT:
    case Ground::WALL_BOTTOM_LEFT:
    case Ground::WALL_BOTTOM_RIGHT:
    case Ground::WALL_TOP_RIGHT_WATER:
    case Ground::WALL_TOP_LEFT_WATER:
    case Ground::WALL_BOTTOM_LEFT_WATER:
    case Ground::WALL_BOTTOM_RIGHT_WATER:
    case Ground::DEEP_WATER:
    case Ground::SHALLOW_WATER:
    case Ground::HOLE:
    case Ground::PRICKLE:
    case Ground::LAVA:
      return false;
  }

  return false;
}

/**
 * \brief Sets whether this custom entity can traverse a kind of ground.
 * \param ground A kind of ground.
 * \param traversable \c true to make this kind of ground traversable.
 */
void CustomEntity::set_can_traverse_ground(Ground ground, bool traversable) {

  can_traverse_grounds[ground] = traversable;
}

/**
 * \brief Restores the default setting of whether this custom entity can
 * traverse a kind of ground.
 *
 * This reverts the settings of previous calls to
 * set_can_traverse_ground(Ground, bool).
 *
 * \param ground A kind of ground.
 */
void CustomEntity::reset_can_traverse_ground(Ground ground) {

  can_traverse_grounds.erase(ground);
}

/**
 * \copydoc Entity::is_traversable_obstacle
 */
bool CustomEntity::is_traversable_obstacle() const {

  return !can_traverse_ground(Ground::TRAVERSABLE);
}

/**
 * \copydoc Entity::is_wall_obstacle
 */
bool CustomEntity::is_wall_obstacle() const {

  return !can_traverse_ground(Ground::WALL);
}

/**
 * \copydoc Entity::is_low_wall_obstacle
 */
bool CustomEntity::is_low_wall_obstacle() const {

  return !can_traverse_ground(Ground::LOW_WALL);
}

/**
 * \copydoc Entity::is_grass_obstacle
 */
bool CustomEntity::is_grass_obstacle() const {

  return !can_traverse_ground(Ground::GRASS);
}

/**
 * \copydoc Entity::is_shallow_water_obstacle
 */
bool CustomEntity::is_shallow_water_obstacle() const {

  return !can_traverse_ground(Ground::SHALLOW_WATER);
}

/**
 * \copydoc Entity::is_deep_water_obstacle
 */
bool CustomEntity::is_deep_water_obstacle() const {

  return !can_traverse_ground(Ground::DEEP_WATER);
}

/**
 * \copydoc Entity::is_hole_obstacle
 */
bool CustomEntity::is_hole_obstacle() const {

  return !can_traverse_ground(Ground::HOLE);
}

/**
 * \copydoc Entity::is_ice_obstacle
 */
bool CustomEntity::is_ice_obstacle() const {

  return !can_traverse_ground(Ground::ICE);
}

/**
 * \copydoc Entity::is_lava_obstacle
 */
bool CustomEntity::is_lava_obstacle() const {

  return !can_traverse_ground(Ground::LAVA);
}

/**
 * \copydoc Entity::is_prickle_obstacle
 */
bool CustomEntity::is_prickle_obstacle() const {

  return !can_traverse_ground(Ground::PRICKLE);
}

/**
 * \copydoc Entity::is_ladder_obstacle
 */
bool CustomEntity::is_ladder_obstacle() const {

  return !can_traverse_ground(Ground::LADDER);
}

/**
 * \brief Registers a function to be called when the specified test detects a
 * collision.
 * \param collision_test A built-in collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
void CustomEntity::add_collision_test(
    CollisionMode collision_test,
    const ScopedLuaRef& callback_ref
) {
  Debug::check_assertion(collision_test != COLLISION_NONE, "Invalid collision mode");
  Debug::check_assertion(!callback_ref.is_empty(), "Missing collision callback");

  if (collision_test == COLLISION_SPRITE) {
    add_collision_mode(COLLISION_SPRITE);
  }
  else {
    add_collision_mode(COLLISION_CUSTOM);
  }

  collision_tests.emplace_back(
      *get_lua_context(),
      collision_test,
      callback_ref
  );

  check_collision_with_detectors();
}

/**
 * \brief Registers a function to be called when the specified test detects a
 * collision.
 * \param collision_test_ref Lua ref to a custom collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
void CustomEntity::add_collision_test(
    const ScopedLuaRef& collision_test_ref,
    const ScopedLuaRef& callback_ref
) {
  Debug::check_assertion(!callback_ref.is_empty(), "Missing collision callback");

  add_collision_mode(COLLISION_CUSTOM);

  collision_tests.emplace_back(
      *get_lua_context(),
      collision_test_ref,
      callback_ref
  );

  check_collision_with_detectors();
}

/**
 * \brief Unregisters all collision test functions.
 */
void CustomEntity::clear_collision_tests() {

  // Disable all collisions checks.
  collision_tests.clear();
  set_collision_modes(COLLISION_FACING);
}

/**
 * \copydoc Entity::test_collision_custom
 */
bool CustomEntity::test_collision_custom(Entity& entity) {

  if (!successful_collision_tests.empty()) {
    // Avoid reentrant collision tests.
    return false;
  }

  bool collision = false;

  const std::vector<CollisionInfo> collision_tests = this->collision_tests;
  for (const CollisionInfo& info: collision_tests) {

    switch (info.get_built_in_test()) {

      case COLLISION_OVERLAPPING:
        if (test_collision_rectangle(entity)) {
          collision = true;
          successful_collision_tests.push_back(info);
        }
        break;

      case COLLISION_CONTAINING:
        if (test_collision_inside(entity)) {
          collision = true;
          successful_collision_tests.push_back(info);
        }
        break;

      case COLLISION_ORIGIN:
        if (test_collision_origin_point(entity)) {
          collision = true;
          successful_collision_tests.push_back(info);
        }
        break;

      case COLLISION_FACING:
        if (test_collision_facing_point(entity)) {
          collision = true;
          successful_collision_tests.push_back(info);
        }
        break;

      case COLLISION_TOUCHING:
        if (test_collision_touching(entity)) {
          collision = true;
          successful_collision_tests.push_back(info);
        }
        break;

      case COLLISION_CENTER:
        if (test_collision_center(entity)) {
          collision = true;
          successful_collision_tests.push_back(info);
        }
        break;

      case COLLISION_CUSTOM:
        if (get_lua_context()->do_custom_entity_collision_test_function(
              info.get_custom_test_ref(), *this, entity)
        ) {
          collision = true;
          successful_collision_tests.push_back(info);
        }
        break;

      case COLLISION_SPRITE:
        // Not handled here.
        break;

      case COLLISION_NONE:
        Debug::die("Invalid collision mode");
        break;
    }
  }

  return collision;
}

/**
 * \copydoc Entity::notify_collision(Entity&,CollisionMode)
 */
void CustomEntity::notify_collision(Entity& entity_overlapping, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_FACING) {
    // This collision mode is only useful to set the facing entity, which
    // is already done by the Entity class.
    return;
  }

  // One or several collisions were detected with an another entity.
  // The collision tests could have been of any kind
  // (even a custom Lua collision test function),
  // except COLLISION_SPRITE that is handled separately.

  Debug::check_assertion(collision_mode == COLLISION_CUSTOM,
      "Unexpected collision mode");

  // There is a collision: execute the callbacks.
  for (const CollisionInfo& info: successful_collision_tests) {
    get_lua_context()->do_custom_entity_collision_callback(
        info.get_callback_ref(), *this, entity_overlapping
    );
  }

  successful_collision_tests.clear();
}

/**
 * \copydoc Entity::notify_collision(Entity&, Sprite&, Sprite&)
 */
void CustomEntity::notify_collision(
    Entity& other_entity,
    Sprite& this_sprite,
    Sprite& other_sprite
) {
  // A collision was detected with a sprite of another entity.
  const std::vector<CollisionInfo> collision_tests = this->collision_tests;
  for (const CollisionInfo& info: collision_tests) {

    if (info.get_built_in_test() == COLLISION_SPRITE) {
      // Execute the callback.
      get_lua_context()->do_custom_entity_collision_callback(
          info.get_callback_ref(),
          *this,
          other_entity,
          this_sprite,
          other_sprite
      );
    }
  }
}

/**
 * \brief Notifies this custom entity that another entity has detected a
 * collision with it.
 * \param other_entity The other entity.
 */
void CustomEntity::notify_collision_from(Entity& other_entity) {

  // See if we also detect the other entity, and if yes, do the callbacks.
  if (test_collision_custom(other_entity)) {
    notify_collision(other_entity, COLLISION_CUSTOM);
  }
}

/**
 * \brief Notifies this custom entity that another entity has detected a
 * pixel-precise collision with it.
 * \param other_entity The other entity.
 * \param other_sprite Sprite of the other entity involved in the collision.
 * \param this_sprite Sprite of this custom entity entity involved in the collision.
 */
void CustomEntity::notify_collision_from(Entity& other_entity, Sprite& other_sprite, Sprite& this_sprite) {

  // No need to check the collision again: sprite collisions are symmetrical.
  notify_collision(other_entity, other_sprite, this_sprite);
}

/**
 * \copydoc Entity::notify_collision_with_destructible
 */
void CustomEntity::notify_collision_with_destructible(
    Destructible& destructible, CollisionMode /* collision_mode */) {

  notify_collision_from(destructible);
}

/**
 * \copydoc Entity::notify_collision_with_teletransporter
 */
void CustomEntity::notify_collision_with_teletransporter(
    Teletransporter& teletransporter, CollisionMode /* collision_mode */) {

  notify_collision_from(teletransporter);
}

/**
 * \copydoc Entity::notify_collision_with_stream
 */
void CustomEntity::notify_collision_with_stream(
    Stream& stream, int /* dx */, int /* dy */) {

  notify_collision_from(stream);
}

/**
 * \copydoc Entity::notify_collision_with_stairs
 */
void CustomEntity::notify_collision_with_stairs(
    Stairs& stairs, CollisionMode /* collision_mode */) {

  notify_collision_from(stairs);
}

/**
 * \copydoc Entity::notify_collision_with_jumper
 */
void CustomEntity::notify_collision_with_jumper(
    Jumper& jumper, CollisionMode /* collision_mode */) {

  notify_collision_from(jumper);
}

/**
 * \copydoc Entity::notify_collision_with_sensor
 */
void CustomEntity::notify_collision_with_sensor(
    Sensor& sensor, CollisionMode /* collision_mode */) {

  notify_collision_from(sensor);
}

/**
 * \copydoc Entity::notify_collision_with_switch(Switch&,CollisionMode)
 */
void CustomEntity::notify_collision_with_switch(
    Switch& sw, CollisionMode /* collision_mode */) {

  notify_collision_from(sw);
}

/**
 * \copydoc Entity::notify_collision_with_switch(Switch&,Sprite&)
 */
void CustomEntity::notify_collision_with_switch(
    Switch& sw, Sprite& sprite_overlapping) {

  notify_collision_from(sw, sw.get_sprite(), sprite_overlapping);
}

/**
 * \copydoc Entity::notify_collision_with_crystal(Crystal&,CollisionMode)
 */
void CustomEntity::notify_collision_with_crystal(
    Crystal& crystal, CollisionMode /* collision_mode */) {

  notify_collision_from(crystal);
}

/**
 * \copydoc Entity::notify_collision_with_crystal(Crystal&,Sprite&)
 */
void CustomEntity::notify_collision_with_crystal(
    Crystal& crystal, Sprite& sprite_overlapping) {

  notify_collision_from(crystal, crystal.get_sprite(), sprite_overlapping);
}

/**
 * \copydoc Entity::notify_collision_with_chest
 */
void CustomEntity::notify_collision_with_chest(Chest& chest) {

  notify_collision_from(chest);
}

/**
 * \copydoc Entity::notify_collision_with_block
 */
void CustomEntity::notify_collision_with_block(Block& block) {

  notify_collision_from(block);
}

/**
 * \copydoc Entity::notify_collision_with_separator
 */
void CustomEntity::notify_collision_with_separator(
    Separator& separator, CollisionMode /* collision_mode */) {

  notify_collision_from(separator);
}

/**
 * \copydoc Entity::notify_collision_with_bomb
 */
void CustomEntity::notify_collision_with_bomb(
    Bomb& bomb, CollisionMode /* collision_mode */) {

  notify_collision_from(bomb);
}

/**
 * \copydoc Entity::notify_collision_with_explosion(Explosion&, CollisionMode)
 */
void CustomEntity::notify_collision_with_explosion(
    Explosion& explosion, CollisionMode /* collision_mode */) {

  notify_collision_from(explosion);
}

/**
 * \copydoc Entity::notify_collision_with_explosion(Explosion&,Sprite&)
 */
void CustomEntity::notify_collision_with_explosion(
    Explosion& explosion, Sprite& sprite_overlapping) {

  notify_collision_from(explosion, explosion.get_sprite(), sprite_overlapping);
}

/**
 * \copydoc Entity::notify_collision_with_fire
 */
void CustomEntity::notify_collision_with_fire(
    Fire& fire, Sprite& sprite_overlapping) {

  notify_collision_from(fire, fire.get_sprite(), sprite_overlapping);
}

/**
 * \copydoc Entity::notify_collision_with_enemy(Enemy&)
 */
void CustomEntity::notify_collision_with_enemy(Enemy& enemy) {

  notify_collision_from(enemy);
}

/**
 * \copydoc Entity::notify_collision_with_enemy(Enemy&, Sprite&, Sprite&)
 */
void CustomEntity::notify_collision_with_enemy(
    Enemy& enemy, Sprite& enemy_sprite, Sprite& this_sprite) {

  notify_collision_from(enemy, enemy_sprite, this_sprite);
}

/**
 * \copydoc Entity::notify_action_command_pressed
 */
bool CustomEntity::notify_action_command_pressed() {

  return get_lua_context()->entity_on_interaction(*this);
}

/**
 * \copydoc Entity::interaction_with_item
 */
bool CustomEntity::notify_interaction_with_item(EquipmentItem& item) {

  return get_lua_context()->entity_on_interaction_item(*this, item);
}

/**
 * \copydoc Entity::is_ground_observer
 */
bool CustomEntity::is_ground_observer() const {
  return ground_observer;
}

/**
 * \brief Checks if this entity becomes a ground observer or stops being one.
 */
void CustomEntity::update_ground_observer() {

  // If there is no on_ground_below_changed() event, don't bother
  // determine the ground below.
  bool ground_observer = get_lua_context()->userdata_has_field(
      *this, "on_ground_below_changed"
  );
  if (ground_observer != this->ground_observer) {
    this->ground_observer = ground_observer;
  }
}

/**
 * \copydoc Entity::notify_ground_below_changed
 */
void CustomEntity::notify_ground_below_changed() {

  get_lua_context()->custom_entity_on_ground_below_changed(
      *this, get_ground_below()
  );
}

/**
 * \copydoc Entity::get_modified_ground
 */
Ground CustomEntity::get_modified_ground() const {
  return modified_ground;
}

/**
 * \brief Changes the ground defined by this entity.
 * \param modified_ground The new ground to set, or Ground::EMPTY to clear it.
 */
void CustomEntity::set_modified_ground(Ground modified_ground) {

  if (modified_ground == this->modified_ground) {
    return;
  }

  // The ground changes, notify observers even if it changes to Ground::EMPTY.
  this->modified_ground = modified_ground;
  update_ground_observers();
}

/**
 * \copydoc Entity::notify_creating
 */
void CustomEntity::notify_creating() {

  Entity::notify_creating();

  get_lua_context()->run_custom_entity(*this);
}

/**
 * \copydoc Entity::update
 */
void CustomEntity::update() {

  Entity::update();
  update_ground_observer();

  if (is_suspended() || !is_enabled()) {
    return;
  }

  get_lua_context()->entity_on_update(*this);
}

/**
 * \copydoc Entity::set_suspended
 */
void CustomEntity::set_suspended(bool suspended) {

  Entity::set_suspended(suspended);

  get_lua_context()->entity_on_suspended(*this, suspended);
}

/**
 * \copydoc Entity::notify_enabled
 */
void CustomEntity::notify_enabled(bool enabled) {

  Entity::notify_enabled(enabled);

  if (enabled) {
    get_lua_context()->entity_on_enabled(*this);
  }
  else {
    get_lua_context()->entity_on_disabled(*this);
  }
}

/**
 * \copydoc Entity::draw_on_map
 */
void CustomEntity::draw_on_map() {

  get_lua_context()->entity_on_pre_draw(*this);
  Entity::draw_on_map();
  get_lua_context()->entity_on_post_draw(*this);
}

/**
 * \brief Empty constructor.
 */
CustomEntity::TraversableInfo::TraversableInfo():
    lua_context(nullptr),
    traversable_test_ref(),
    traversable(false) {

}

/**
 * \brief Creates a boolean traversable property.
 * \param lua_context The Lua context.
 * \param traversable The value to store.
 */
CustomEntity::TraversableInfo::TraversableInfo(
    LuaContext& lua_context,
    bool traversable
):
    lua_context(&lua_context),
    traversable_test_ref(),
    traversable(traversable) {

}

/**
 * \brief Creates a traversable property as a Lua boolean function.
 * \param lua_context The Lua context.
 * \param traversable_test_ref Lua ref to a function.
 */
CustomEntity::TraversableInfo::TraversableInfo(
    LuaContext& lua_context,
    const ScopedLuaRef& traversable_test_ref
):
    lua_context(&lua_context),
    traversable_test_ref(traversable_test_ref),
    traversable(false) {

}

/**
 * \brief Returns whether this traversable property is empty.
 * \return \c true if no property is set.
 */
bool CustomEntity::TraversableInfo::is_empty() const {

  return lua_context == nullptr;
}

/**
 * \brief Tests this traversable property with the specified other entity.
 *
 * This traversable property must not be empty.
 *
 * \param current_entity A custom entity.
 * \param other_entity Another entity.
 * \return \c true if traversing is allowed, \c false otherwise.
 */
bool CustomEntity::TraversableInfo::is_traversable(
    CustomEntity& current_entity,
    Entity& other_entity
) const {

  Debug::check_assertion(!is_empty(), "Empty traversable info");

  if (traversable_test_ref.is_empty()) {
    // A fixed boolean was set.
    return traversable;
  }

  // A Lua boolean function was set.
  return lua_context->do_custom_entity_traversable_test_function(
      traversable_test_ref, current_entity, other_entity
  );
}

/**
 * \brief Empty constructor.
 */
CustomEntity::CollisionInfo::CollisionInfo():
    lua_context(nullptr),
    built_in_test(COLLISION_NONE),
    custom_test_ref(),
    callback_ref() {

}

/**
 * \brief Creates a collision test info.
 * \param lua_context The Lua context.
 * \param collision_test A built-in collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
CustomEntity::CollisionInfo::CollisionInfo(
    LuaContext& lua_context,
    CollisionMode built_in_test,
    const ScopedLuaRef& callback_ref
):
    lua_context(&lua_context),
    built_in_test(built_in_test),
    custom_test_ref(),
    callback_ref(callback_ref) {

  Debug::check_assertion(!callback_ref.is_empty(), "Missing callback ref");
}

/**
 * \brief Creates a collision test info.
 * \param lua_context The Lua context.
 * \param collision_test_ref Lua ref to a custom collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
CustomEntity::CollisionInfo::CollisionInfo(
    LuaContext& lua_context,
    const ScopedLuaRef& custom_test_ref,
    const ScopedLuaRef& callback_ref
):
    lua_context(&lua_context),
    built_in_test(COLLISION_CUSTOM),
    custom_test_ref(custom_test_ref),
    callback_ref(callback_ref) {

  Debug::check_assertion(!callback_ref.is_empty(), "Missing callback ref");
}

/**
 * \brief Returns the built-in collision test to perform.
 * \return The built-in collision test, or COLLISION_NONE if this is a
 * customized collision test.
 */
CollisionMode CustomEntity::CollisionInfo::get_built_in_test() const {
  return built_in_test;
}

/**
 * \brief Returns the customized collision test to perform.
 * \return A Lua ref to the customized collision test function,
 * or an empty ref if this is a built-in collision test.
 */
const ScopedLuaRef& CustomEntity::CollisionInfo::get_custom_test_ref() const {
  return custom_test_ref;
}

/**
 * \brief Returns the function to call when the collision is detected.
 * \return A Lua ref to the callback.
 */
const ScopedLuaRef& CustomEntity::CollisionInfo::get_callback_ref() const {
  return callback_ref;
}

}

