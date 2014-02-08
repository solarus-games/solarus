/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/CustomEntity.h"
#include "lua/LuaContext.h"
#include "Sprite.h"
#include <lua.hpp>

namespace solarus {

/**
 * \brief Creates a custom entity.
 * \param game The game.
 * \param name Name of the entity or an empty string.
 * \param direction Direction of the entity (0 to 3).
 * \param layer The layer on the map.
 * \param x X position on the map.
 * \param y Y position on the map.
 * \param width Width of the entity.
 * \param height Height of the entity.
 * \param sprite_name Animation set id of a sprite or an empty string.
 * \param model Model of custom entity or an empty string.
 */
CustomEntity::CustomEntity(
    Game& game,
    const std::string& name,
    int direction,
    Layer layer,
    int x,
    int y,
    int width,
    int height,
    const std::string& sprite_name,
    const std::string& model):

  Detector(
      COLLISION_NONE,
      name, layer, x, y, width, height),

  model(model) {

  set_origin(8, 13);
  initialize_sprite(sprite_name, direction);
}

/**
 * \brief Destructor.
 */
CustomEntity::~CustomEntity() {
}

/**
 * \brief Returns the type of entity.
 * \return The type of entity.
 */
EntityType CustomEntity::get_type() const {
  return ENTITY_CUSTOM;
}

/**
 * \brief Returns the model of this custom entity if any.
 * \return The model or an empty string.
 */
const std::string& CustomEntity::get_model() const {
  return model;
}

/**
 * \brief Sets whether this custom entity can be traversed by other entities.
 *
 * This applies to entities that are not overridden by
 * set_traversable_by_entities(EntityType, bool)
 * or
 * set_traversable_by_entities(EntityType, int).
 *
 * \param traversable \c true to allow other entities to traverse this custom
 * entity.
 */
void CustomEntity::set_traversable_by_entities(bool traversable) {

  traversable_by_entities_general = TraversableInfo(*this, traversable);
}

/**
 * \brief Registers a Lua function to decide if this custom entity can be
 * traversed by other entities.
 *
 * This applies to entities that are not overridden by
 * set_traversable_by_entities(EntityType, bool)
 * or
 * set_traversable_by_entities(EntityType, int).
 *
 * \param traversable_test_ref Lua ref to a function that will do the test.
 */
void CustomEntity::set_traversable_by_entities(int traversable_test_ref) {

  traversable_by_entities_general = TraversableInfo(*this, traversable_test_ref);
}

/**
 * \brief Restores the default setting of whether this custom entity can be
 * traversed by other entities.
 *
 * This reverts the settings of previous calls to
 * set_traversable_by_entities(bool)
 * and
 * set_traversable_by_entities(int).
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
 * set_traversable_by_entities(int).
 *
 * \param type A type of entities.
 * \param traversable \c true to allow other entities to traverse this custom
 * entity.
 */
void CustomEntity::set_traversable_by_entities(
    EntityType type, bool traversable) {

  traversable_by_entities_type[type] = TraversableInfo(*this, traversable);
}

/**
 * \brief Registers a Lua function to decide if this custom entity can be
 * traversed by other entities of the specified type.
 *
 * This overrides for a specific type whatever was set by
 * set_traversable_by_entities(bool)
 * or
 * set_traversable_by_entities(int).
 *
 * \param type A type of entities.
 * \param traversable_test_ref Lua ref to a function that will do the test.
 */
void CustomEntity::set_traversable_by_entities(
    EntityType type, int traversable_test_ref) {

  traversable_by_entities_type[type] = TraversableInfo(*this, traversable_test_ref);
}

/**
 * \brief Restores the default setting of whether this custom entity can be
 * traversed by other entities of the specified type.
 *
 * This reverts the settings of previous calls to
 * set_traversable_by_entities(EntityType, bool)
 * and
 * set_traversable_by_entities(EntityType, int).
 *
 * \param type A type of entities.
 */
void CustomEntity::reset_traversable_by_entities(EntityType type) {

  traversable_by_entities_type.erase(type);
}

/**
 * \brief Sets whether this custom entity can traverse other entities.
 *
 * This applies to entities that are not overridden by
 * set_can_traverse_entities(EntityType, bool)
 * or
 * set_can_traverse_entities(EntityType, int).
 *
 * \param traversable \c true to allow this entity to traverse other entities.
 */
void CustomEntity::set_can_traverse_entities(bool traversable) {

  // TODO take can_traverse settings into account.

  can_traverse_entities_general = TraversableInfo(*this, traversable);
}

/**
 * \brief Registers a Lua function to decide if this custom entity can traverse
 * other entities.
 *
 * This applies to entities that are not overridden by
 * set_can_traverse_entities(EntityType, bool)
 * or
 * set_can_traverse_entities(EntityType, int).
 *
 * \param traversable_test_ref Lua ref to a function that will do the test.
 */
void CustomEntity::set_can_traverse_entities(int traversable_test_ref) {

  can_traverse_entities_general = TraversableInfo(*this, traversable_test_ref);
}

/**
 * \brief Restores the default setting of whether this custom entity can
 * traverse other entities.
 *
 * This reverts the settings of previous calls to
 * set_can_traverse_entities(bool)
 * and
 * set_can_traverse_entities(int).
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
 * set_can_traverse_entities(int).
 *
 * \param type A type of entities.
 * \param traversable \c true to allow this entity to traverse other entities
 * of the specified type.
 */
void CustomEntity::set_can_traverse_entities(
    EntityType type, bool traversable) {

  can_traverse_entities_type[type] = TraversableInfo(*this, traversable);
}

/**
 * \brief Registers a Lua function to decide if this custom entity can
 * traverse other entities of the specified type.
 *
 * This overrides for a specific type whatever was set by
 * set_can_traverse_entities(bool)
 * or
 * set_can_traverse_entities(int).
 *
 * \param type A type of entities.
 * \param traversable_test_ref Lua ref to a function that will do the test.
 */
void CustomEntity::set_can_traverse_entities(
    EntityType type, int traversable_test_ref) {

  can_traverse_entities_type[type] = TraversableInfo(*this, traversable_test_ref);
}

/**
 * \brief Restores the default setting of whether this custom entity can
 * traverse other entities of the specified type.
 callback_ref This reverts the settings of previous calls to
 * set_can_traverse_entities(EntityType, bool)
 * and
 * set_can_traverse_entities(EntityType, int).
 *
 * \param type A type of entities.
 */
void CustomEntity::reset_can_traverse_entities(EntityType type) {

  can_traverse_entities_type.erase(type);
}

/**
 * \brief Returns whether this custom entity can traverse a kind of ground.
 * \param ground A kind of ground.
 * \return \c true if this custom entity can traverse this kind of ground.
 */
bool CustomEntity::can_traverse_ground(Ground ground) const {

  // TODO call this function

  const std::map<Ground, bool>::const_iterator it =
    can_traverse_grounds.find(ground);

  if (it != can_traverse_grounds.end()) {
    // Return the boolean value that was set.
    return it->second;
  }

  // Return a default value.
  switch (ground) {

    case GROUND_EMPTY:
    case GROUND_TRAVERSABLE:
    case GROUND_GRASS:
    case GROUND_ICE:
    case GROUND_LADDER:
      return true;

    case GROUND_WALL:
    case GROUND_LOW_WALL:
    case GROUND_WALL_TOP_RIGHT:
    case GROUND_WALL_TOP_LEFT:
    case GROUND_WALL_BOTTOM_LEFT:
    case GROUND_WALL_BOTTOM_RIGHT:
    case GROUND_WALL_TOP_RIGHT_WATER:
    case GROUND_WALL_TOP_LEFT_WATER:
    case GROUND_WALL_BOTTOM_LEFT_WATER:
    case GROUND_WALL_BOTTOM_RIGHT_WATER:
    case GROUND_DEEP_WATER:
    case GROUND_SHALLOW_WATER:
    case GROUND_HOLE:
    case GROUND_PRICKLE:
    case GROUND_LAVA:
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
 * \brief Registers a function to be called when the specified test detects a
 * collision.
 * \param collision_test A built-in collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
void CustomEntity::add_collision_test(CollisionMode collision_test, int callback_ref) {

  Debug::check_assertion(collision_test != COLLISION_NONE, "Invalid collision mode");
  Debug::check_assertion(callback_ref != LUA_REFNIL, "Missing collision callback");

  if (collision_test == COLLISION_SPRITE) {
    add_collision_mode(COLLISION_SPRITE);
  }
  else {
    add_collision_mode(COLLISION_CUSTOM);
  }

  collision_tests.push_back(CollisionInfo(*this, collision_test, callback_ref));
}

/**
 * \brief Registers a function to be called when the specified test detects a
 * collision.
 * \param collision_test_ref Lua ref to a custom collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
void CustomEntity::add_collision_test(int collision_test_ref, int callback_ref) {

  Debug::check_assertion(callback_ref != LUA_REFNIL, "Missing collision callback");

  add_collision_mode(COLLISION_CUSTOM);

  collision_tests.push_back(CollisionInfo(*this, collision_test_ref, callback_ref));
}

/**
 * \brief Unregisters all collision test functions.
 */
void CustomEntity::clear_collision_tests() {

  // Disable all collisions checks.
  collision_tests.clear();
  set_collision_modes(COLLISION_NONE);
}

/**
 * \brief Creates the sprite specified.
 * \param sprite_name sprite Animation set of the entity, or an empty string
 * to create no sprite.
 * \param initial_direction Direction of the entity's sprite. Ignored if there
 * is no sprite of if the sprite has no such direction.
 */
void CustomEntity::initialize_sprite(
    const std::string& sprite_name, int initial_direction) {

  if (!sprite_name.empty()) {
    Sprite& sprite = create_sprite(sprite_name);
    if (initial_direction >= 0 && initial_direction < sprite.get_nb_directions()) {
      sprite.set_current_direction(initial_direction);
    }
  }
}

/**
 * \copydoc MapEntity::set_map
 */
void CustomEntity::set_map(Map& map) {

  Detector::set_map(map);

  get_lua_context().run_custom_entity(*this);
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other Another entity.
 * \return \c true if this entity is an obstacle for the other one.
 */
bool CustomEntity::is_obstacle_for(MapEntity& other) {

  EntityType type = other.get_type();

  const TraversableInfo* info = &traversable_by_entities_general;
  const std::map<EntityType, TraversableInfo>::const_iterator it =
    traversable_by_entities_type.find(type);
  if (it != traversable_by_entities_type.end()) {
    // This entity type overrides the general setting.
    info = &it->second;
  }

  if (info->is_empty()) {
    // Nothing was set: make the custom entity traversable by default.
    return false;
  }

  if (info->traversable_test_ref == LUA_REFNIL) {
    // A fixed boolean was set.
    return !info->traversable;
  }

  // A Lua boolean function was set.
  return !get_lua_context().do_custom_entity_traversable_test_function(
      info->traversable_test_ref, *this, other
  );

}

/**
 * \copydoc Detector::test_collision_custom
 */
bool CustomEntity::test_collision_custom(MapEntity& entity) {

  bool collision = false;
  detected_collision_callbacks.clear();

  std::vector<CollisionInfo>::const_iterator it;
  for (it = collision_tests.begin(); it != collision_tests.end(); ++it) {

    const CollisionInfo& info = *it;
    switch (info.built_in_test) {

      case COLLISION_OVERLAPPING:
        if (test_collision_rectangle(entity)) {
          collision = true;
          detected_collision_callbacks.push_back(info.callback_ref);
        }
        break;

      case COLLISION_CONTAINING:
        if (test_collision_inside(entity)) {
          collision = true;
          detected_collision_callbacks.push_back(info.callback_ref);
        }
        break;

      case COLLISION_ORIGIN:
        if (test_collision_origin_point(entity)) {
          collision = true;
          detected_collision_callbacks.push_back(info.callback_ref);
        }
        break;

      case COLLISION_FACING:
        if (test_collision_facing_point(entity)) {
          collision = true;
          detected_collision_callbacks.push_back(info.callback_ref);

          // Make sure only one entity can think "I am the facing entity".
          if (entity.get_facing_entity() == NULL) {
            entity.set_facing_entity(this);
          }
        }
        break;

      case COLLISION_TOUCHING:
        if (test_collision_facing_point_any(entity)) {
          collision = true;
          detected_collision_callbacks.push_back(info.callback_ref);
        }
        break;

      case COLLISION_CENTER:
        if (test_collision_center(entity)) {
          collision = true;
          detected_collision_callbacks.push_back(info.callback_ref);
        }
        break;

      case COLLISION_CUSTOM:
        if (get_lua_context().do_custom_entity_collision_test_function(
              info.custom_test_ref, *this, entity)
            ) {
          collision = true;
          detected_collision_callbacks.push_back(info.callback_ref);
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
 * \copydoc Detector::notify_collision(MapEntity&,CollisionMode)
 */
void CustomEntity::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  // One or several collisions were detected with an another entity.
  // The collision tests could have been of any kind
  // (even a custom Lua collision test function),
  // except COLLISION_SPRITE that is handled separately.

  Debug::check_assertion(collision_mode == COLLISION_CUSTOM,
      "Unexpected collision mode");

  std::vector<int>::const_iterator it;

  // There is a collision: execute the callbacks.
  for (it = detected_collision_callbacks.begin();
      it != detected_collision_callbacks.end();
      ++it) {

    int callback_ref = *it;
    get_lua_context().do_custom_entity_collision_callback(
        callback_ref, *this, entity_overlapping
    );
  }

  detected_collision_callbacks.clear();
}

/**
 * \copydoc Detector::notify_collision(MapEntity&,Sprite&,Sprite&)
 */
void CustomEntity::notify_collision(MapEntity& other_entity, Sprite& other_sprite, Sprite& this_sprite) {

  // A collision was detected with a sprite of another entity.
  std::vector<CollisionInfo>::const_iterator it;
  for (it = collision_tests.begin(); it != collision_tests.end(); ++it) {

    const CollisionInfo& info = *it;
    if (info.built_in_test == COLLISION_SPRITE) {
      // Execute the callback.
      get_lua_context().do_custom_entity_collision_callback(
          info.callback_ref,
          *this,
          other_entity,
          this_sprite,
          other_sprite
      );
    }
  }
}

/**
 * \brief Empty constructor.
 */
CustomEntity::TraversableInfo::TraversableInfo():
  entity(NULL),
  traversable_test_ref(LUA_REFNIL),
  traversable(false) {

}

/**
 * \brief Creates a boolean traversable property.
 * \param entity The custom entity.
 * \param traversable The value to store.
 */
CustomEntity::TraversableInfo::TraversableInfo(CustomEntity& entity, bool traversable):
  entity(&entity),
  traversable_test_ref(LUA_REFNIL),
  traversable(traversable) {

}

/**
 * \brief Creates a traversable property as a Lua boolean function.
 * \param entity The custom entity.
 * \param traversable_test_ref Lua ref to a function.
 */
CustomEntity::TraversableInfo::TraversableInfo(CustomEntity& entity, int traversable_test_ref):
  entity(&entity),
  traversable_test_ref(traversable_test_ref),
  traversable(false) {

}

/**
 * \brief Copy constructor.
 * \param other Another traversable property.
 */
CustomEntity::TraversableInfo::TraversableInfo(const TraversableInfo& other):
  entity(other.entity),
  traversable_test_ref(LUA_REFNIL),
  traversable(other.traversable) {

  if (entity != NULL) {
    traversable_test_ref = entity->get_lua_context().copy_ref(other.traversable_test_ref);
  }
}

/**
 * \brief Destroys a traversable property.
 */
CustomEntity::TraversableInfo::~TraversableInfo() {

  if (entity != NULL) {
    entity->get_lua_context().cancel_callback(traversable_test_ref);
  }
}

/**
 * \brief Assignment operator.
 * \param other Another traversable property.
 */
CustomEntity::TraversableInfo& CustomEntity::TraversableInfo::operator=(const TraversableInfo& other) {

  if (&other == this) {
    return *this;
  }

  if (entity != NULL) {
    entity->get_lua_context().cancel_callback(traversable_test_ref);
  }

  entity = other.entity;
  traversable_test_ref = LUA_REFNIL;

  if (entity != NULL) {
    traversable_test_ref = entity->get_lua_context().copy_ref(other.traversable_test_ref);
  }
  traversable = other.traversable;

  return *this;
}

/**
 * \brief Returns whether this traversable property is empty.
 * \return \c true if no property is set.
 */
bool CustomEntity::TraversableInfo::is_empty() const {

  return entity == NULL;
}

/**
 * \brief Empty constructor.
 */
CustomEntity::CollisionInfo::CollisionInfo():
  entity(NULL),
  built_in_test(COLLISION_NONE),
  custom_test_ref(LUA_REFNIL),
  callback_ref(LUA_REFNIL) {

}

/**
 * \brief Creates a collision test info.
 * \param entity The custom entity.
 * \param collision_test A built-in collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
CustomEntity::CollisionInfo::CollisionInfo(CustomEntity& entity, CollisionMode built_in_test, int callback_ref):
  entity(&entity),
  built_in_test(built_in_test),
  custom_test_ref(LUA_REFNIL),
  callback_ref(callback_ref) {

}

/**
 * \brief Creates a collision test info.
 * \param entity The custom entity.
 * \param collision_test_ref Lua ref to a custom collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
CustomEntity::CollisionInfo::CollisionInfo(CustomEntity& entity, int custom_test_ref, int callback_ref):
  entity(&entity),
  built_in_test(COLLISION_CUSTOM),
  custom_test_ref(custom_test_ref),
  callback_ref(callback_ref) {

}

/**
 * \brief Copy constructor.
 * \param other Another traversable property.
 */
CustomEntity::CollisionInfo::CollisionInfo(const CollisionInfo& other):
  entity(other.entity),
  built_in_test(other.built_in_test),
  custom_test_ref(LUA_REFNIL),
  callback_ref(LUA_REFNIL) {

  if (entity != NULL) {
    custom_test_ref = entity->get_lua_context().copy_ref(other.custom_test_ref);
    callback_ref = entity->get_lua_context().copy_ref(other.callback_ref);
  }

}

/**
 * \brief Destroys a collision test info.
 */
CustomEntity::CollisionInfo::~CollisionInfo() {

  if (entity != NULL) {
    entity->get_lua_context().cancel_callback(custom_test_ref);
    entity->get_lua_context().cancel_callback(callback_ref);
  }
}

/**
 * \brief Assignment operator.
 * \param other Another collision test info.
 */
CustomEntity::CollisionInfo& CustomEntity::CollisionInfo::operator=(const CollisionInfo& other) {

  if (&other == this) {
    return *this;
  }

  if (entity != NULL) {
    entity->get_lua_context().cancel_callback(custom_test_ref);
    entity->get_lua_context().cancel_callback(callback_ref);
  }

  entity = other.entity;
  built_in_test = other.built_in_test;
  custom_test_ref = LUA_REFNIL;
  callback_ref = LUA_REFNIL;

  if (entity != NULL) {
    custom_test_ref = entity->get_lua_context().copy_ref(other.custom_test_ref);
    callback_ref = entity->get_lua_context().copy_ref(other.callback_ref);
  }

  return *this;
}

}

