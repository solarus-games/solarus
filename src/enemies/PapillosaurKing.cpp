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
#include "entities/MapEntities.h"
#include "movements/RandomWalkMovement.h"
#include "Map.h"
#include "Random.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "ResourceManager.h"
#include "Sound.h"

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
  get_sprite()->get_animation_set()->enable_pixel_collisions();
  set_size(176, 96);
  set_origin(88, 64);
  set_collision_modes(COLLISION_SPRITE);

  // vulnerabilities
  for (int i = 0; i < ATTACK_NUMBER; i++) {
    vulnerabilities[i] = 0; // insensible to most attacks
  }
  vulnerabilities[ATTACK_SWORD] = -1;    // protected against sword
  vulnerabilities[ATTACK_EXPLOSION] = 1; // sensible to explosions
}

/**
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void PapillosaurKing::restart(void) {
  Enemy::restart();

  // state
  next_egg_phase_date = SDL_GetTicks() + 3000;
  next_egg_date = 0;
  get_sprite()->set_current_animation("walking");

  // movement
  if (movement == NULL) {
    set_movement(new RandomWalkMovement(5));
  }
}

/**
 * Returns whether a destructible item is currently considered as an obstacle for this entity.
 * @param destructible_item a destructible item
 * @return true if the destructible item is currently an obstacle for this entity
 */
bool PapillosaurKing::is_destructible_item_obstacle(DestructibleItem *destructible_item) {
  // as the papillosaur is a big flying boss, it can fly over destructible items
  return false;
}

/**
 * Suspends or resumes the entity.
 * @param suspended true to suspend the entity, false to resume it
 */
void PapillosaurKing::set_suspended(bool suspended) {

  Enemy::set_suspended(suspended);

  if (!suspended) {
    Uint32 diff = SDL_GetTicks() - when_suspended;
    next_egg_phase_date += diff;
    next_egg_date += diff;
  }
}

/**
 * Updates this entity.
 */
void PapillosaurKing::update(void) {

  Enemy::update();

  if (suspended) {
    return;
  }

  // create minillosaur eggs
  Uint32 now = SDL_GetTicks();
  if (is_in_normal_state() && now >= next_egg_phase_date - 500) {

    // time to start an egg phase
    if (get_movement() != NULL) {
      // stop the movement
      clear_movement();
    }
    else if (now >= next_egg_phase_date) {

      if (get_sprite()->get_current_animation() != "preparing_egg") {
	// after a delay, start the fast animation and play a sound
	ResourceManager::get_sound("boss_charge")->play();
	get_sprite()->set_current_animation("preparing_egg");
	next_egg_date = now + 1500;
	nb_eggs_to_create = (get_life() < 3) ? 3 : 1;
      }
      else if (now >= next_egg_date) {

	MapEntity *egg = create(MINILLOSAUR, RANK_NORMAL, -1, get_name() + "_minillosaur",
	    get_layer(), get_x(), get_y() + 16, 0, PickableItem::NONE, -1);
	map->get_entities()->add_entity(egg);
	ResourceManager::get_sound("boss_fireball")->play();

	if (--nb_eggs_to_create > 0) {
	  next_egg_date = now + 500;
	}
	else {
	  next_egg_phase_date = now + 5000 + (1 + Random::get_number(3) * 1000);
	  get_sprite()->set_current_animation("walking");
	  set_movement(new RandomWalkMovement(5));
	}
      }
    }
  }
}

/**
 * This function is called when the enemy has just been hurt.
 * @param source the source of the attack
 * @param attack the attack that was just successful
 * @param life_points the number of life points lost by this enemy
 */
void PapillosaurKing::just_hurt(MapEntity *source, EnemyAttack attack, int life_points) {

  if (get_life() <= 0) {
    // the papillosaur is dying: remove the minillosaur eggs
    std::list<MapEntity*> *sons = map->get_entities()->get_entities_with_prefix(ENEMY, get_name() + "_minillosaur");

    for (std::list<MapEntity*>::iterator it = sons->begin(); it != sons->end(); it++) {
      map->get_entities()->remove_entity(*it);
    }
  }
}

