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
#include "enemies/ChainAndBall.h"
#include "movements/CircleMovement.h"
#include "Sprite.h"

/**
 *
 * Constructor.
 * @param params name and position of the enemy to create
 */
ChainAndBall::ChainAndBall(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * Destructor.
 */
ChainAndBall::~ChainAndBall(void) {

}

/**
 * Initializes this enemy.
 */
void ChainAndBall::initialize(void) {

  // attack/defense features
  set_damage(2);
  set_life(1);

  // sprite
  create_sprite("enemies/chain_and_ball");
  get_sprite()->set_current_animation("walking");

  // rectangle
  set_size(16, 16);
  set_origin(8, 8);

  // reactions to attacks
  set_no_attack_consequences();
  set_attack_consequence(ATTACK_SWORD, -1);

  // movement (temporary)
  CircleMovement *movement = new CircleMovement();
  movement->start(360, 32, get_xy());
  set_movement(movement);
}

/**
 * Updates this enemy.
 */
void ChainAndBall::update(void) {
  Enemy::update();
}

/**
 * Makes the chain and ball follow the specified entity.
 * @param entity the entity to follow
 */
void ChainAndBall::attach_to(MapEntity *entity) {
  attach_to(entity, 0, 0);
}

/**
 * Makes the chain and ball follow the specified entity.
 * @param entity the entity to follow
 * @param x x coordinate of where the chain movement should be centered on (relative to the entity followed)
 * @param y y coordinate of where the chain movement should be centered on (relative to the entity followed)
 */
void ChainAndBall::attach_to(MapEntity *entity, int x, int y) {
}

/**
 * Makes the chain and ball twirl around its current position.
 */
void ChainAndBall::twirl(void) {

  // set_movement(new CircleMovement(this, 48));
}

