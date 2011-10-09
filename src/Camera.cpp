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
#include "lua/MapScript.h"
#include "entities/MapEntity.h"
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "movements/TargetMovement.h"

/**
 * @brief Creates a camera.
 * @param map the map
 */
Camera::Camera(Map& map):
  map(map),
  fixed_on_hero(true),
  restoring(false),
  position(0, 0, 320, 240),
  speed(120),
  movement(NULL) {

}

/**
 * @brief Destructor.
 */
Camera::~Camera() {
  delete movement;
}

/**
 * @brief Updates the camera position.
 *
 * This function is called continuously.
 */
void Camera::update() {

  int x = position.get_x();
  int y = position.get_y();
  const Rectangle& map_location = map.get_location();

  // if the camera is not moving, center it on the hero
  if (is_fixed_on_hero()) {
    const Rectangle& hero_center = map.get_entities().get_hero().get_center_point();
    x = hero_center.get_x();
    y = hero_center.get_y();
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
        map.get_script().notify_camera_back();
      }
      else {
        map.get_script().notify_camera_reached_target();
      }
    }
  }

  position.set_xy(x, y);
}

/**
 * @brief Returns the current position of the camera.
 *
 * This function returns the rectangle of the visible area of this camera.
 *
 * @return the visible area
 */
const Rectangle& Camera::get_position() {
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
bool Camera::is_fixed_on_hero() {
  return fixed_on_hero;
}

/**
 * @brief Sets the speed of the camera movement.
 * @param speed speed of the movement in pixels per second
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

  const Rectangle& map_location = map.get_location();
  target_x = std::min(std::max(target_x, 160), map_location.get_width() - 160);
  target_y = std::min(std::max(target_y, 120), map_location.get_height() - 120);

  movement = new TargetMovement(target_x, target_y, speed);
  movement->set_xy(position.get_x() + 160, position.get_y() + 120);

  fixed_on_hero = false;
}

/**
 * @brief Makes the camera move towards an entity.
 *
 * The camera will be centered on the entity's center point.
 * If there was already a movement, the new one replaces it.
 * Note that the camera will not update its movement if the entity moves.
 *
 * @param entity the target entity
 */
void Camera::move(MapEntity& entity) {

  const Rectangle& center = entity.get_center_point();
  move(center.get_x(), center.get_y());
}

/**
 * @brief Moves the camera back to the hero.
 *
 * The hero is not supposed to move during this time.
 * Once the movement is finished, the camera starts following the hero again.
 */
void Camera::restore() {

  move(map.get_entities().get_hero());
  restoring = true;
}

