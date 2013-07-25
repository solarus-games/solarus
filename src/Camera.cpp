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
#include "Camera.h"
#include "Map.h"
#include "entities/MapEntity.h"
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "entities/CameraStopper.h"
#include "movements/TargetMovement.h"
#include "lowlevel/VideoManager.h"
#include "lua/LuaContext.h"

/**
 * \brief Creates a camera.
 * \param map The map.
 */
Camera::Camera(Map& map):
  map(map),
  position(VideoManager::get_instance()->get_quest_size()),
  fixed_on_hero(true),
  stopper_scrolling_dx(0),
  stopper_scrolling_dy(0),
  stopper_next_scrolling_date(0),
  restoring(false),
  speed(120),
  movement(NULL) {
}

/**
 * \brief Destructor.
 */
Camera::~Camera() {
  delete movement;
}

/**
 * \brief Returns the width of the visible area shown by the camera.
 * \return The width of the quest screen.
 */
int Camera::get_width() {
  return position.get_width();
}

/**
 * \brief Returns the height of the visible area shown by the camera.
 * \return The height of the quest screen.
 */
int Camera::get_height() {
  return position.get_height();
}

/**
 * \brief Updates the camera position.
 *
 * This function is called continuously by the game loop.
 */
void Camera::update() {

  // If the camera is not moving towards a target, center it on the hero.
  if (fixed_on_hero) {
    update_fixed_on_hero();
  }
  else if (movement != NULL) {
    update_moving();
  }
}

/**
 * \brief Updates the position of the camera when the camera is fixed
 * on the hero.
 */
void Camera::update_fixed_on_hero() {

  Debug::check_assertion(fixed_on_hero,
      "Illegal call to Camera::update_fixed_on_hero()");

  // First compute the camera coordinates ignoring map borders and camera stoppers.
  const Rectangle& hero_center = map.get_entities().get_hero().get_center_point();
  const int hero_x = hero_center.get_x();
  const int hero_y = hero_center.get_y();
  int x = hero_x - get_width() / 2;
  int y = hero_y - get_height() / 2;

  // See if there is a camera stopper in the rectangle.
  const std::list<CameraStopper*>& stoppers =
      map.get_entities().get_camera_stoppers();
  std::list<CameraStopper*>::const_iterator it;
  for (it = stoppers.begin(); it != stoppers.end(); ++it) {
    CameraStopper* stopper = *it;

    if (stopper->get_width() == 16) {
      // Vertical camera stopper.
      int separation_x = stopper->get_x() + 8;

      if (x < separation_x && separation_x < x + get_width()
          && stopper->get_y() < y + get_height()
          && y < stopper->get_y() + stopper->get_height()) {
        int left = separation_x - x;
        int right = x + get_width() - separation_x;
        if (left > right) {
          x = separation_x - get_width();
        }
        else {
          x = separation_x;
        }
      }
    }
    else if (stopper->get_height() == 16) {
      // Horizontal camera stopper.
      int separation_y = stopper->get_y() + 8;
      if (y < separation_y && separation_y < y + get_height()
          && stopper->get_x() < x + get_width()
          && x < stopper->get_x() + stopper->get_width()) {
        int top = separation_y - y;
        int bottom = y + get_height() - separation_y;
        if (top > bottom) {
          y = separation_y - get_height();
        }
        else {
          y = separation_y;
        }
      }
    }
    else {
      Debug::die("Wrong size of camera stopper: width or height must be 16");
    }
  }

  // Take care of the limits of the map.
  const Rectangle& map_location = map.get_location();
  if (map_location.get_width() < get_width()) {
    x = (map_location.get_width() - get_width()) / 2;
  }
  else {
    x = std::min(std::max(x, 0),
        map_location.get_width() - get_width());
  }

  if (map_location.get_height() < get_height()) {
    y = (map_location.get_height() - get_height()) / 2;
  }
  else {
    y = std::min(std::max(y, 0),
        map_location.get_height() - get_height());
  }

  position.set_xy(x, y);
}

/**
 * \brief Updates the position of the camera when the camera is moving
 * towards a point or back to the hero.
 */
void Camera::update_moving() {

  Debug::check_assertion(!fixed_on_hero,
      "Illegal call to Camera::update_fixed_on_hero()");

  if (movement == NULL) {
    return;
  }

  movement->update();
  int x = movement->get_x() - get_width() / 2;
  int y = movement->get_y() - get_height() / 2;

  if (movement->is_finished()) {
    delete movement;
    movement = NULL;

    if (restoring) {
      restoring = false;
      fixed_on_hero = true;
      map.get_lua_context().map_on_camera_back(map);
    }
    else {
      map.get_lua_context().notify_camera_reached_target(map);
    }
  }

  position.set_xy(x, y);
}

/**
 * \brief Returns the current position of the camera.
 *
 * This function returns the rectangle of the visible area of this camera.
 *
 * \return The visible area.
 */
const Rectangle& Camera::get_position() {
  return position;
}

/**
 * \brief Returns whether there is a camera movement.
 *
 * It may be a movement towards a point or a scrolling movement due to a
 * camera stopper.
 *
 * \return \c true if the camera is moving.
 */
bool Camera::is_moving() {
  return !fixed_on_hero
      || stopper_next_scrolling_date != 0;
}

/**
 * \brief Sets the speed of the camera movement.
 * \param speed speed of the movement in pixels per second
 */
void Camera::set_speed(int speed) {
  this->speed = speed;
}

/**
 * \brief Makes the camera move towards a destination point.
 *
 * The camera will be centered on this point.
 * If there was already a movement, the new one replaces it.
 *
 * \param target_x x coordinate of the target point
 * \param target_y y coordinate of the target point
 */
void Camera::move(int target_x, int target_y) {

  if (movement != NULL) {
    delete movement;
  }

  const Rectangle& map_location = map.get_location();
  target_x = std::min(std::max(target_x, get_width() / 2),
      map_location.get_width() - get_width() / 2);
  target_y = std::min(std::max(target_y, get_height() / 2),
      map_location.get_height() - get_height() / 2);

  movement = new TargetMovement(target_x, target_y, speed, true);
  movement->set_xy(position.get_x() + get_width() / 2, position.get_y() + get_height() / 2);

  fixed_on_hero = false;
}

/**
 * \brief Makes the camera move towards an entity.
 *
 * The camera will be centered on the entity's center point.
 * If there was already a movement, the new one replaces it.
 * Note that the camera will not update its movement if the entity moves.
 *
 * \param entity the target entity
 */
void Camera::move(MapEntity& entity) {

  const Rectangle& center = entity.get_center_point();
  move(center.get_x(), center.get_y());
}

/**
 * \brief Moves the camera back to the hero.
 *
 * The hero is not supposed to move during this time.
 * Once the movement is finished, the camera starts following the hero again.
 */
void Camera::restore() {

  move(map.get_entities().get_hero());
  restoring = true;
}

