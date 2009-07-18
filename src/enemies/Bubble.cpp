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
#include "enemies/Bubble.h"
#include "movements/CollisionMovement.h"
#include "Random.h"
#include "Sprite.h"

/**
 * Constructor.
 * @param params name and position of the enemy to create
 */
Bubble::Bubble(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * Destructor.
 */
Bubble::~Bubble(void) {

}

/**
 * Initializes the enemy.
 */
void Bubble::initialize(void) {

  // attack/defense features
  set_damage(2, 4);
  set_life(1);
  minimum_shield_needed = 0;

  // sprite
  create_sprite("enemies/bubble");
  set_size(16, 16);
  set_origin(8, 8);

  // vulnerabilities
  for (int i = 0; i < ATTACK_NUMBER; i++) {
    vulnerabilities[i] = 0;
  }

  // movement
  set_movement(new CollisionMovement());
}

/**
 * This function is called when the enemy need to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void Bubble::restart(void) {
  Enemy::restart();

  int movement_direction8 = Random::get_number(4) * 2 + 1;
  go(movement_direction8);
}

/**
 * Makes the enemy go into the specified direction.
 * @param movement_direction8 the direction (1, 3, 5 or 7)
 */
void Bubble::go(int movement_direction8) {

  if (movement_direction8 != 1 &&
      movement_direction8 != 3 &&
      movement_direction8 != 5 &&
      movement_direction8 != 7) {
    DIE("Invalid Bubble direction: " << movement_direction8);
  }

  this->movement_direction8 = movement_direction8;
  get_movement()->set_speed(8);
  get_movement()->set_direction(movement_direction8 * 45);
}


/**
 * Updates the enemy.
 */
void Bubble::update(void) {

  Enemy::update();

  if (get_movement()->is_stopped()) {
    // collision
    bounce();
  }
}

/**
 * Makes the enemy bounce against a wall.
 */
void Bubble::bounce(void) {

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

  CollisionMovement *movement = (CollisionMovement*) get_movement();


  if (!movement->collision_with_map(dxy[try1][0], dxy[try1][1])) {
    go(try1);
  }
  else if (!movement->collision_with_map(dxy[try2][0], dxy[try2][1])) {
    go(try2);
  }
  else {
    go(try3);
  }
}

