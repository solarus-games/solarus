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
#include "Camera.h"
#include "Map.h"
#include "MapScript.h"
#include "entities/MapEntity.h"
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "movements/TargetMovement.h"

/**
 * @brief Creates a camera.
 * @param map the map
 */
Camera::Camera(Map *map):
  map(map), fixed_on_hero(true), restoring(false), speed(12), movement(NULL) {

  position.set_xy(0, 0);
}

/**
 * @brief Destructor.
 */
Camera::~Camera(void) {
  delete movement;
}

/**
 * @brief Updates the camera position.
 *
 * This function is called continuously.
 */
void Camera::update(void) {

  int x = position.get_x();
  int y = position.get_y();
  const Rectangle &map_location = map->get_location();

  // if the camera is not moving, center it on the hero
  if (is_fixed_on_hero()) {
    Hero *hero = map->get_entities()->get_hero();
    x = hero->get_x();
    y = hero->get_y();
    x = std::min(std::max(x - 160, 0), map_location.get_width() - 320);
    y = std::min(std::max(y - 120, 0), map_location.get_height() - 240);
  }
  else if (movement != NULL) {
    movement->update();
    x = movement->get_x() - 160;
    y = movement->get_y() - 120;

    if (movement->is_finished()) {
      delete movement;
      movement = NULL;

      if (restoring) {
	restoring = false;
	fixed_on_hero = true;
	map->get_script()->event_camera_back();
      }
      else {
	map->get_script()->event_camera_reached_target();
      }
    }
  }

  position.set_xy(x, y);
}

/**
 * @brief Returns the current position of the camera.
 *
 * This function returns the top-left corner of the visible area of the current map.
 * Only x and y are used (the size is always 320*240).
 *
 * @return the visible area
 */
Rectangle & Camera::get_position(void) {
  return position;
}

/**
 * @brief Returns whether the camera is fixed on the hero.
 *
 * Most of the time, the camera follows the hero and this function returns true.
 * If the camera is being moved somewhere else, this function returns false.
 *
 * @return true if the camera is fixed on the hero
 */
bool Camera::is_fixed_on_hero(void) {
  return fixed_on_hero;
}

/**
 * @brief Sets the speed of the camera movement.
 * @param speed speed of the movement
 */
void Camera::set_speed(int speed) {
  this->speed = speed;
}

/**
 * @brief Makes the camera move towards a destination point.
 *
 * The camera will be centered on this point.
 * If there was already a movement, the new one replaces it.
 *
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 */
void Camera::move(int target_x, int target_y) {

  if (movement != NULL) {
    delete movement;
  }

  const Rectangle &map_location = map->get_location();
  target_x = std::min(std::max(target_x, 160), map_location.get_width() - 160);
  target_y = std::min(std::max(target_y, 120), map_location.get_height() - 120);

  movement = new TargetMovement(target_x, target_y, speed);
  movement->set_position(position.get_x() + 160, position.get_y() + 120);

  fixed_on_hero = false;
}

/**
 * @brief Makes the camera move towards an entity.
 *
 * The camera will be centered on the entity's origin.
 * If there was already a movement, the new one replaces it.
 * Note that the camera will not update its movement if the entity moves.
 *
 * @param entity the target entity
 */
void Camera::move(MapEntity *entity) {
  move(entity->get_x(), entity->get_y());
}

/**
 * @brief Moves the camera back to the hero.
 *
 * The hero is not supposed to move during this time.
 * Once the movement is finished, the camera starts following the hero again.
 */
void Camera::restore(void) {
  move(map->get_entities()->get_hero());
  restoring = true;
}

