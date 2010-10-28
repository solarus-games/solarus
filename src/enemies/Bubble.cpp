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
#include "enemies/Bubble.h"
#include "movements/RectilinearMovement.h"
#include "Sprite.h"
#include "lowlevel/Random.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Constructor.
 * @param params name and position of the enemy to create
 */
Bubble::Bubble(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * @brief Destructor.
 */
Bubble::~Bubble() {

}

/**
 * @brief Initializes the enemy.
 */
void Bubble::initialize() {

  // attack/defense features
  set_damage(2, 4);
  set_life(1);

  // sprite
  create_sprite("enemies/bubble");
  set_size(12, 12);
  set_origin(6, 6);

  // reactions to attack
  set_no_attack_consequences();

  // movement
  set_movement(new RectilinearMovement());
}

/**
 * @brief Restarts the normal movement of this enemy.
 *
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void Bubble::restart() {

  Enemy::restart();

  int movement_direction8 = Random::get_number(4) * 2 + 1;
  go(movement_direction8);
}

/**
 * @brief Makes the enemy go into the specified direction.
 * @param movement_direction8 the direction (1, 3, 5 or 7)
 */
void Bubble::go(int movement_direction8) {

  Debug::assert(movement_direction8 == 1
      || movement_direction8 == 3
      || movement_direction8 == 5
      || movement_direction8 == 7,
      StringConcat() << "Invalid Bubble direction: " << movement_direction8);

  this->movement_direction8 = movement_direction8;

  RectilinearMovement *movement = (RectilinearMovement*) get_movement();
  movement->set_speed(8);
  movement->set_direction(movement_direction8 * 45);
}


/**
 * @brief Updates the enemy.
 */
void Bubble::update() {

  Enemy::update();

  if (get_movement()->is_stopped()) {
    // collision
    bounce();
  }
}

/**
 * @brief Makes the enemy bounce against a wall.
 */
void Bubble::bounce() {

  static const int dxy[8][2] = {
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, 1},
    {1, 1}
  };

  int try1 = (movement_direction8 + 2) % 8;
  int try2 = (movement_direction8 + 6) % 8;
  int try3 = (movement_direction8 + 4) % 8;

  Movement *movement = get_movement();

  if (!movement->test_collision_with_obstacles(dxy[try1][0], dxy[try1][1])) {
    go(try1);
  }
  else if (!movement->test_collision_with_obstacles(dxy[try2][0], dxy[try2][1])) {
    go(try2);
  }
  else {
    go(try3);
  }
}

