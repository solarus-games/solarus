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
#include "enemies/Khorneth.h"
#include "movements/RandomWalkMovement.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"

/**
 * Constructor.
 * @param params name and position of the enemy to create
 */
Khorneth::Khorneth(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * Destructor.
 */
Khorneth::~Khorneth(void) {

}

/**
 * Initializes the enemy.
 */
void Khorneth::initialize(void) {

  // attack/defense features
  set_damage(2, 0);
  set_life(4);
  minimum_shield_needed = 0;

  // sprite
  create_sprite("enemies/khorneth");
  get_sprite()->get_animation_set()->enable_pixel_collisions();
  set_size(40, 48);
  set_origin(20, 45);
  set_collision_modes(COLLISION_SPRITE);

  // vulnerabilities
  for (int i = 0; i < ATTACK_NUMBER; i++) {
    vulnerabilities[i] = 0; // insensible to most attacks
  }
  vulnerabilities[ATTACK_SWORD] = 1;    // TODO temporary

  // movement
  set_movement(new RandomWalkMovement(3));
}

/**
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void Khorneth::restart(void) {
  Enemy::restart();
}

