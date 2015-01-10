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
#include "solarus/entities/Sensor.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/MapEntities.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Debug.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param name name of the entity
 * \param layer layer of the entity
 * \param xy Coordinates where to create the entity.
 * \param size Size of the entity.
 */
Sensor::Sensor(
    const std::string& name,
    Layer layer,
    const Point& xy,
    const Size& size):
  Detector(COLLISION_CONTAINING | COLLISION_OVERLAPPING, name, layer, xy, size),
  activated_by_hero(false),
  notifying_script(false) {

  set_origin(8, 13);
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Sensor::get_type() const {
  return EntityType::SENSOR;
}

/**
 * \brief Returns whether entities of this type can be drawn.
 * \return true if this type of entity can be drawn
 */
bool Sensor::can_be_drawn() const {
  return false;
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other another entity
 * \return true if this entity is an obstacle for the other one
 */
bool Sensor::is_obstacle_for(MapEntity& other) {

  return other.is_sensor_obstacle(*this);
}

/**
 * \brief This function is called by the engine when an entity overlaps this sensor.
 * \param entity_overlapping the entity overlapping the detector
 * \param collision_mode the collision mode that detected the collision
 */
void Sensor::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_sensor(*this, collision_mode);
}

/**
 * \brief This function is called when an explosion detects a collision with this entity.
 * \param explosion an explosion
 * \param collision_mode the collision mode that detected the collision
 */
void Sensor::notify_collision_with_explosion(Explosion& /* explosion */, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_OVERLAPPING) {
    get_lua_context().sensor_on_collision_explosion(*this);
  }
}

/**
 * \brief Activates this sensor.
 *
 * This function is called when the hero overlaps the sensor.
 *
 * \param hero the hero
 */
void Sensor::activate(Hero& /* hero */) {

  if (!activated_by_hero) {

    activated_by_hero = true;

    // Notify Lua.
    notifying_script = true;
    get_lua_context().sensor_on_activated(*this);
    notifying_script = false;
  }
  else {
    if (!notifying_script && !get_game().is_suspended()) {
      notifying_script = true;
      get_lua_context().sensor_on_activated_repeat(*this);
      notifying_script = false;
    }
  }
}

/**
 * \brief Updates this entity.
 */
void Sensor::update() {

  Detector::update();

  if (activated_by_hero) {
    // check whether the hero is still present
    if (!test_collision_inside(get_hero())) {
      activated_by_hero = false;

      // Notify Lua.
      notifying_script = true;
      get_lua_context().sensor_on_left(*this);
      notifying_script = false;
    }
  }
}

}

