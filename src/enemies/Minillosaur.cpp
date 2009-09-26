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
#include "enemies/Minillosaur.h"
#include "entities/Hero.h"
#include "movements/StraightMovement.h"
#include "movements/PathFindingMovement.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Sound.h"

/**
 * Constructor.
 * @param params name and position of the enemy to create
 */
Minillosaur::Minillosaur(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * Destructor.
 */
Minillosaur::~Minillosaur(void) {

}

/**
 * Initializes this enemy.
 */
void Minillosaur::initialize(void) {

  // attack/defense features
  set_damage(2);
  set_life(2);
  minimum_shield_needed = 0;

  // sprite
  create_sprite("enemies/minillosaur");
  get_sprite()->set_current_animation("egg");

  // rectangle
  set_size(24, 32);
  set_origin(12, 20);

  // initial movement
  Hero *hero = zsdx->game->get_hero();
  set_movement(new StraightMovement(12, get_xy(), hero->get_xy(), 1500));

  // vulnerabilities
  set_no_vulnerabilities();
  set_vulnerability(ATTACK_SWORD, -3); // custom effect when striking the egg

  // state
  in_egg = true;
}

/**
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void Minillosaur::restart(void) {

  if (!in_egg) {
    Enemy::restart();
  }
}

/**
 * This function is called when the enemy is attacked by a custom effect attack.
 * @param attack the attack
 * @return the number of health points lost (can be 0)
 */
int Minillosaur::custom_attack(EnemyAttack attack) {

  if (attack == ATTACK_SWORD && get_sprite()->get_current_animation() == "egg") {
    ResourceManager::get_sound("monster_hurt")->play();
    clear_movement();
    get_sprite()->set_current_animation("egg_breaking");
  }
  return 0;
}

/**
 * Updates this enemy.
 */
void Minillosaur::update(void) {

  std::string animation = get_sprite()->get_current_animation();

  if (animation == "egg" && get_movement()->is_stopped()) {
    clear_movement();
    get_sprite()->set_current_animation("egg_breaking");
  }

  if (animation == "egg_breaking" && get_sprite()->is_animation_finished()) {
    get_sprite()->set_current_animation("walking");

    set_size(16, 16);
    set_origin(8, 12);
    set_aligned_to_grid();
    set_movement(new PathFindingMovement(zsdx->game->get_hero(), 4));
    set_default_vulnerabilities();
    in_egg = false;
  }

  Enemy::update();
}

