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
#include "enemies/PapillosaurKing.h"
#include "movements/CollisionMovement.h"
#include "Random.h"
#include "Sprite.h"

/**
 * Constructor.
 * @param params name and position of the enemy to create
 */
PapillosaurKing::PapillosaurKing(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * Destructor.
 */
PapillosaurKing::~PapillosaurKing(void) {

}

/**
 * Initializes the enemy.
 */
void PapillosaurKing::initialize(void) {

  // attack/defense features
  set_damage(2, 0);
  set_life(6);
  minimum_shield_needed = 0;

  // sprite
  create_sprite("enemies/boss_papillosaurking");
  set_size(176, 96);
  set_origin(88, 64);

  // vulnerabilities
  for (int i = 0; i < ATTACK_NUMBER; i++) {
    vulnerabilities[i] = 0;
  }

}

/**
 * This function is called when the enemy need to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void PapillosaurKing::restart(void) {
  Enemy::restart();

}

