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
#include "enemies/Khotor.h"
#include "movements/RandomWalkMovement.h"
#include "Map.h"
#include "entities/MapEntities.h"
#include "enemies/ChainAndBall.h"

/**
 * Constructor.
 * @param params name and position of the enemy to create
 */
Khotor::Khotor(const ConstructionParameters &params):
  Enemy(params), chain(NULL) {

}

/**
 * Destructor.
 */
Khotor::~Khotor(void) {

}

/**
 * Initializes the enemy.
 */
void Khotor::initialize(void) {

  // attack/defense features
  set_damage(2, 0);
  set_life(12);
  set_pushed_back_when_hurt(false);

  // sprite
  create_sprite("enemies/khotor", true);
  set_size(48, 48);
  set_origin(24, 29);

  // reactions to attacks
  set_no_attack_consequences();
  set_attack_consequence(ATTACK_SWORD, 2);
  set_attack_consequence(ATTACK_THROWN_ITEM, 1);
  set_attack_consequence(ATTACK_BOW, 1);

  // movement
  set_movement(new RandomWalkMovement(3));

  // chain and ball
  chain = (ChainAndBall*) create(game, CHAIN_AND_BALL, RANK_NORMAL, -1, "chain_and_ball", get_layer(),
      get_x(), get_y(), 0, PickableItem::NONE, -1);
  //chain->attach_to(this, -16, -33, 64); // TODO when the walking+chain animation is available
  chain->attach_to(this, 0, -8, 64);
  map->get_entities()->add_entity(chain);
}

/**
 * Returns whether this entity should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Khotor::is_displayed_in_y_order(void) {
  return false; // unlike usual enemies
}

/**
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void Khotor::restart(void) {
  Enemy::restart();
}

/**
 * This function is called when the enemy has just been hurt.
 * @param source the source of the attack
 * @param attack the attack that was just successful
 * @param life_points the number of life points lost by this enemy
 */
void Khotor::just_hurt(MapEntity *source, EnemyAttack attack, int life_points) {

  if (get_life() <= 0) {

    // Khotor is dying: remove the chain and ball
    chain->remove_from_map();
  }
}

