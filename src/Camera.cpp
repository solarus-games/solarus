#include "Camera.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "entities/Hero.h"
#include "movements/TargetMovement.h"

/**
 * Creates a camera.
 * @param map the map
 */
Camera::Camera(Map *map):
  map(map), movement(NULL) {

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

  int x;
  int y;
  const SDL_Rect *map_location = map->get_location();

  // if the camera is not moving, center it on the hero
  if (!is_moving()) {
    Hero *hero = zsdx->game->get_hero();
    x = hero->get_x();
    y = hero->get_y();
  }
  else {
    movement->update();
    x = movement->get_x();
    y = movement->get_y();

    if (movement->is_finished()) {
      delete movement;
      movement = NULL;
    }
  }

  position.x = MIN(MAX(x - 160, 0), map_location->w - 320);
  position.y = MIN(MAX(y - 120, 0), map_location->h - 240);
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
 * Returns whether the camera is moving.
 * @return true if the camera is moving
 */
bool Camera::is_moving(void) {
  return movement != NULL;
}

/**
 * Makes the camera move towards a destination point.
 * The camera will be centered on this point.
 * If there was already a movement, the new one replaces it.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 * @param speed speed of the movement
 */
void Camera::set_movement(int target_x, int target_y, int speed) {

  if (movement != NULL) {
    delete movement;
  }

  const SDL_Rect *map_location = map->get_location();
  target_x = MIN(MAX(target_x, 160), map_location->w - 160);
  target_y = MIN(MAX(target_y, 120), map_location->h - 120);

  movement = new TargetMovement(target_x, target_y, speed);
  movement->set_position(position.x + 160, position.y + 120);
}
