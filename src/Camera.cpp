/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Camera.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "MapScript.h"
#include "entities/Hero.h"
#include "movements/TargetMovement.h"

/**
 * Creates a camera.
 * @param map the map
 */
Camera::Camera(Map *map):
  map(map), fixed_on_hero(true), restoring(false), speed(12), movement(NULL) {

  position.x = 0;
  position.y = 0;
}

/**
 * Destructor.
 */
Camera::~Camera(void) {
  delete movement;
}

/**
 * Updates the camera position.
 */
void Camera::update(void) {

  int x = position.x;
  int y = position.y;
  const SDL_Rect *map_location = map->get_location();

  // if the camera is not moving, center it on the hero
  if (is_fixed_on_hero()) {
    Hero *hero = zsdx->game->get_hero();
    x = hero->get_x();
    y = hero->get_y();
    x = MIN(MAX(x - 160, 0), map_location->w - 320);
    y = MIN(MAX(y - 120, 0), map_location->h - 240);
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
      }
      else {
	map->get_script()->event_camera_reached_target();
      }
    }
  }

  position.x = x;
  position.y = y;
}

/**
 * Returns the coordinates of the top-left corner of the 
 * visible area of the current map.
 * Only x and y are indicated.
 * @return the visible area
 */
SDL_Rect *Camera::get_position(void) {
  return &position;
}

/**
 * Returns whether the camera is fixed on the hero,
 * i.e. if it it not being moved elsewhere.
 * @return true if the camera is fixed on the hero
 */
bool Camera::is_fixed_on_hero(void) {
  return fixed_on_hero;
}

/**
 * Sets the speed of the camera movement.
 * @param speed speed of the movement
 */
void Camera::set_speed(int speed) {
  this->speed = speed;
}

/**
 * Makes the camera move towards a destination point.
 * The camera will be centered on this point.
 * If there was already a movement, the new one replaces it.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 */
void Camera::move(int target_x, int target_y) {

  if (movement != NULL) {
    delete movement;
  }

  const SDL_Rect *map_location = map->get_location();
  target_x = MIN(MAX(target_x, 160), map_location->w - 160);
  target_y = MIN(MAX(target_y, 120), map_location->h - 120);

  movement = new TargetMovement(target_x, target_y, speed);
  movement->set_position(position.x + 160, position.y + 120);

  fixed_on_hero = false;
}

/**
 * Makes the camera move towards an entity.
 * The camera will be centered on the entity's origin.
 * If there was already a movement, the new one replaces it.
 * @param entity the target entity
 */
void Camera::move(MapEntity *entity) {
  move(entity->get_x(), entity->get_y());
}

/**
 * Moves the camera back to the hero.
 * When the movement finishes, the camera follows the hero again.
 */
void Camera::restore(void) {
  move(zsdx->game->get_hero());
  restoring = true;
}
