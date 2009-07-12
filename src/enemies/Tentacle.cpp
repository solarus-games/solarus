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
#include "enemies/Tentacle.h"
#include "entities/Hero.h"
#include "movements/PathFindingMovement.h"
#include "Random.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"

/**
 * Constructor.
 * @param params name and position of the enemy to create
 */
Tentacle::Tentacle(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * Destructor.
 */
Tentacle::~Tentacle(void) {

}

/**
 * Initializes the enemy.
 */
void Tentacle::initialize(void) {

  // attack/defense features
  set_damage(2);
  set_life(1);
  minimum_shield_needed = 0;

  // sprite
  create_sprite("enemies/tentacle");
  set_size(16, 16);
  set_origin(8, 13);

  // movement
  set_movement(new PathFindingMovement(zsdx->game->get_hero(), 3));
}

/**
 * This function is called when the enemy need to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void Tentacle::restart(void) {
  Enemy::restart();
}

/**
 * Updates the enemy.
 */
void Tentacle::update(void) {

  Enemy::update();
}

