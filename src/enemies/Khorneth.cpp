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

  // sprite
  create_sprite("enemies/khorneth", true);
  create_sprite("enemies/khorneth_left_blade", true);
  create_sprite("enemies/khorneth_right_blade", true);
  set_size(40, 48);
  set_origin(20, 45);
  set_collision_modes(COLLISION_SPRITE);

  // reactions to attacks
  set_no_attack_consequences();

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

/**
 * Returns the consequence corresponding to the specified attack on the specified sprite of this enemy.
 * @param attack an attack this enemy is subject to
 * @param this_sprite the sprite attacked, or NULL if the attack does not come from
 * a pixel-perfect collision test
 * @return the corresponding attack.
 */
int Khorneth::get_attack_consequence(EnemyAttack attack, Sprite *this_sprite) {

  int result;

  if (attack != ATTACK_SWORD) {
    result = Enemy::get_attack_consequence(attack);
  }
  else {
    const SpriteAnimationSetId &id = this_sprite->get_animation_set_id();
    if (id == "enemies/khorneth") {
      // body: protected against the attacks
      result = -1;
    }
    else {
      // blades: sensible to the sword
      result = -3;
    }
  }
  return result;
}

/**
 * This function is called when the enemy is attacked by a custom effect attack.
 * @param attack the attack
 * @param this_sprite the sprite of this enemy subject to the attack, or NULL
 * if the attack does not come from a pixel-perfect collision test.
 * @return the number of health points lost (can be 0)
 */
int Khorneth::custom_attack(EnemyAttack attack, Sprite *this_sprite) {

  if (this_sprite->contains("left_blade")) {
    // TODO
  }

  return 0;
}

