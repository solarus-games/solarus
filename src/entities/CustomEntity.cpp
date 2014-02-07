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

  // Release the Lua callbacks.
  clear_collision_tests();

  reset_traversable_by_entities();
  std::map<EntityType, TraversableInfo>::const_iterator it;
  for (it = traversable_by_entities_type.begin();
      it != traversable_by_entities_type.end();
      ++it) {
    const TraversableInfo& info = it->second;
    get_lua_context().cancel_callback(info.traversable_test_ref);
  }

  reset_can_traverse_entities();
  for (it = can_traverse_entities_type.begin();
      it != can_traverse_entities_type.end();
      ++it) {
    const TraversableInfo& info = it->second;
    get_lua_context().cancel_callback(info.traversable_test_ref);
  }
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

// TODO
void CustomEntity::set_traversable_by_entities(bool traversable) {

}

void CustomEntity::set_traversable_by_entities(int traversable_test_ref) {

}

void CustomEntity::reset_traversable_by_entities() {

}

void CustomEntity::set_traversable_by_entities(EntityType type, bool traversable) {

}

void CustomEntity::set_traversable_by_entities(EntityType type, int traversable_test_ref) {

}

void CustomEntity::reset_traversable_by_entities(EntityType type) {

}

void CustomEntity::set_can_traverse_entities(bool traversable) {

}

void CustomEntity::set_can_traverse_entities(int traversable_test_ref) {

}

void CustomEntity::reset_can_traverse_entities() {

}

void CustomEntity::set_can_traverse_entities(EntityType type, bool traversable) {

}

void CustomEntity::set_can_traverse_entities(EntityType type, int traversable_test_ref) {

}

void CustomEntity::reset_can_traverse_entities(EntityType type) {

}

bool CustomEntity::can_traverse_ground(Ground ground) const {

  return true;
}

void CustomEntity::set_can_traverse_ground(Ground ground, bool traversable) {

}

void CustomEntity::reset_can_traverse_ground(Ground ground) {

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

  collision_tests.push_back(CollisionInfo(collision_test, callback_ref));
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

  collision_tests.push_back(CollisionInfo(collision_test_ref, callback_ref));
}

/**
 * \brief Unregisters all collision test functions.
 */
void CustomEntity::clear_collision_tests() {

  // Release the Lua callbacks.
  std::vector<CollisionInfo>::iterator it;
  for (it = collision_tests.begin(); it != collision_tests.end(); ++it) {

    CollisionInfo& info = *it;
    get_lua_context().cancel_callback(info.custom_test_ref);
    get_lua_context().cancel_callback(info.callback_ref);
  }
  collision_tests.clear();

  // Disable all collisions checks.
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
bool CustomEntity::is_obstacle_for(const MapEntity& other) const {

  /* TODO
  EntityType type = other.get_type();

  const TraversableInfo* traversable_info = &traversable_by_entities_general;
  const std::map<EntityType, TraversableInfo>::const_iterator it =
    traversable_by_entities.find(type);
  if (it != traversable_by_entities.end()) {
    // This entity type overrides the general setting.
    traversable_info = &(*it);
  }
  */
  return false;
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
 * \brief Creates an empty traversable test info.
 */
CustomEntity::TraversableInfo::TraversableInfo():
  traversable_test_ref(LUA_REFNIL),
  traversable(false),
  is_empty(true) {

}

/**
 * \brief Creates a boolean traversable property.
 * \param traversable The value to store.
 */
CustomEntity::TraversableInfo::TraversableInfo(bool traversable):
  traversable_test_ref(LUA_REFNIL),
  traversable(traversable),
  is_empty(false) {

}

/**
 * \brief Creates a traversable property as a Lua boolean function.
 * \param traversable_test_ref Lua ref to a function.
 */
CustomEntity::TraversableInfo::TraversableInfo(int traversable_test_ref):
  traversable_test_ref(traversable_test_ref),
  traversable(false),
  is_empty(false) {

}

/**
 * \brief Creates a collision test info.
 * \param collision_test A built-in collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
CustomEntity::CollisionInfo::CollisionInfo(CollisionMode built_in_test, int callback_ref):
  built_in_test(built_in_test),
  custom_test_ref(LUA_REFNIL),
  callback_ref(callback_ref) {

}

/**
 * \brief Creates a collision test info.
 * \param collision_test_ref Lua ref to a custom collision test.
 * \param callback_ref Lua ref to a function to call when this collision is
 * detected.
 */
CustomEntity::CollisionInfo::CollisionInfo(int custom_test_ref, int callback_ref):
  built_in_test(COLLISION_CUSTOM),
  custom_test_ref(custom_test_ref),
  callback_ref(callback_ref) {

}

}

