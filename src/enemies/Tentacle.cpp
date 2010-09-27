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
#include "enemies/Tentacle.h"
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "Map.h"
#include "movements/PathFindingMovement.h"
#include "Sprite.h"

/**
 * @brief Constructor.
 * @param params name and position of the enemy to create
 */
Tentacle::Tentacle(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * @brief Destructor.
 */
Tentacle::~Tentacle() {

}

/**
 * @brief Initializes the enemy.
 */
void Tentacle::initialize() {

  // attack/defense features
  set_damage(2);
  set_life(1);

  // sprite
  create_sprite("enemies/tentacle");
  set_size(16, 16);
  set_origin(8, 13);

  // movement
  set_movement(new PathFindingMovement(&get_hero(), 3));
}

