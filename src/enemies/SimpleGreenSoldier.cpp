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
#include "enemies/SimpleGreenSoldier.h"
#include "movements/StraightMovement.h"
#include "Random.h"
#include "Sprite.h"

/**
 * Constructor.
 * @param params name and position of the enemy to create
 */
SimpleGreenSoldier::SimpleGreenSoldier(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * Destructor.
 */
SimpleGreenSoldier::~SimpleGreenSoldier(void) {

}

/**
 * Initializes the enemy.
 */
void SimpleGreenSoldier::initialize(void) {

  // attack/defense features
  set_damage(2);
  set_life(2);
  minimum_shield_needed = 2;

  // sprite
  create_sprite("enemies/simple_green_soldier");
  set_size(16, 16);
  set_origin(8, 13);

  // movement
  set_movement(new StraightMovement(map, 0, 0, 0));
}

/**
 * This function is called when the enemy need to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void SimpleGreenSoldier::restart(void) {
  Enemy::restart();
  walk(Random::get_number(4));
}

/**
 * Updates the enemy.
 */
void SimpleGreenSoldier::update(void) {

  Enemy::update();

  if (!is_killed()) {
    Sprite *sprite = get_sprite();
    std::string animation = sprite->get_current_animation();
    if (get_movement()->is_stopped() && animation == "walking") {

      int rand = Random::get_number(2);

      if (rand == 0) {
	sprite->set_current_animation("stopped_watching_left");
      }
      else {
	sprite->set_current_animation("stopped_watching_right");
      }
    }

    if (sprite->is_animation_finished()) {

      int direction = sprite->get_current_direction();
      if (animation == "stopped_watching_left") {
	walk((direction + 1) % 4);
      }
      else if (animation == "stopped_watching_right") {
	walk((direction + 3) % 4);
      }
    }
  }
}

/**
 * Starts walking towards one of the four main directions.
 * @param direction a direction (0 to 3)
 */
void SimpleGreenSoldier::walk(int direction) {

  Sprite *sprite = get_sprite();
  sprite->set_current_animation("walking");
  sprite->set_current_direction(direction);

  StraightMovement *movement = (StraightMovement*) get_movement();
  int seconds = 2 + Random::get_number(3);
  movement->start(4, direction * 90, seconds * 1000);
}
