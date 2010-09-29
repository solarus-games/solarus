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
#include "enemies/Khorneth.h"
#include "movements/RandomWalkMovement.h"
#include "Game.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "lowlevel/Random.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"

/**
 * @brief Constructor.
 * @param params name and position of the enemy to create
 */
Khorneth::Khorneth(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * @brief Destructor.
 */
Khorneth::~Khorneth() {

}

/**
 * @brief Initializes the enemy.
 */
void Khorneth::initialize() {

  // attack/defense features
  set_damage(2, 0);
  set_life(5);
  set_pushed_back_when_hurt(false);
  //set_push_back_hero_on_sword(true); // not yet implemented

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

  // blades
  left_blade_life = 4;
  end_left_blade_hurt_date = 0;
  right_blade_life = 4;
  end_right_blade_hurt_date = 0;
  next_blade_attack_date = 0;
  blade_attack = false;
  blades_destroyed = false;
}

/**
 * @brief Returns the main sprite of Khorneth.
 * @return the main sprite
 */
Sprite* Khorneth::get_main_sprite() {
  return &get_sprite("enemies/khorneth");
}

/**
 * @brief Returns the left blade sprite.
 * @return the left blade sprite, or NULL if the left blade was destroyed
 */
Sprite* Khorneth::get_left_blade_sprite() {
  return has_left_blade() ? &get_sprite("enemies/khorneth_left_blade") : NULL;
}

/**
 * @brief Returns the right blade sprite.
 * @return the right blade sprite, or NULL if the right blade was destroyed
 */
Sprite* Khorneth::get_right_blade_sprite() {
  return has_right_blade() ? &get_sprite("enemies/khorneth_right_blade") : NULL;
}

/**
 * @brief Returns whether the left blade is still present.
 * @return true if the left blade is still present
 */
bool Khorneth::has_left_blade() {
  return left_blade_life > 0;
}

/**
 * @brief Returns whether the left blade is still present.
 * @return true if the left blade is still present
 */
bool Khorneth::has_right_blade() {
  return right_blade_life > 0;
}

/**
 * @brief Returns whether at least one blade is still present.
 * @return true if the left blade or the right blade is still present
 */
bool Khorneth::has_blade() {
  return !blades_destroyed;
}

/**
 * @brief Returns whether the left blade is currently playing animation "hurt".
 * @return true if the left blade is being hurt
 */
bool Khorneth::is_left_blade_being_hurt() {
  return end_left_blade_hurt_date != 0;
}

/**
 * @brief Returns whether the right blade is currently playing animation "hurt".
 * @return true if the right blade is being hurt
 */
bool Khorneth::is_right_blade_being_hurt() {
  return end_right_blade_hurt_date != 0;
}

/**
 * @brief Returns the consequence corresponding to the specified attack on the specified sprite of this enemy.
 * @param attack an attack this enemy is subject to
 * @param this_sprite the sprite attacked, or NULL if the attack does not come from
 * a pixel-perfect collision test
 * @return the corresponding attack.
 */
int Khorneth::get_attack_consequence(EnemyAttack attack, Sprite *this_sprite) {

  int result;

  if (attack != ATTACK_SWORD || !has_blade()) {
    result = Enemy::get_attack_consequence(attack);
  }
  else {
    if (!this_sprite->contains("blade")) {
      // body: ignore the attack
      result = 0;
    }
    else {
      // blades: sensible to the sword
      result = -3;
    }
  }
  return result;
}

/**
 * @brief This function is called when the enemy is attacked by a custom effect attack.
 * @param attack the attack
 * @param this_sprite the sprite of this enemy subject to the attack, or NULL
 * if the attack does not come from a pixel-perfect collision test.
 * @return the number of health points lost (can be 0)
 */
int Khorneth::custom_attack(EnemyAttack attack, Sprite *this_sprite) {

  if (has_left_blade() && this_sprite == get_left_blade_sprite() && this_sprite->get_current_animation() != "stopped") {

    this_sprite->set_current_animation("hurt");
    get_main_sprite()->set_current_animation("stopped");
    if (has_right_blade()) {
      get_right_blade_sprite()->set_current_animation("stopped");
    }
    stop_movement();
    Sound::play("boss_hurt");
    left_blade_life--;
    end_left_blade_hurt_date = System::now() + 400;
  }
  else if (has_right_blade() && this_sprite == get_right_blade_sprite() && this_sprite->get_current_animation() != "stopped") {

    this_sprite->set_current_animation("hurt");
    get_main_sprite()->set_current_animation("stopped");
    if (has_left_blade()) {
      get_left_blade_sprite()->set_current_animation("stopped");
    }
    stop_movement();
    Sound::play("boss_hurt");
    right_blade_life--;
    end_right_blade_hurt_date = System::now() + 400;
  }

  return 0;
}

/**
 * @brief Starts the last phase of Khorneth.
 *
 * This function is called when both blades are destroyed.
 */
void Khorneth::start_final_phase() {
  blades_destroyed = true;
  set_attack_consequence(ATTACK_SWORD, 1);
  clear_movement();
  set_movement(new RandomWalkMovement(4));
}

/**
 * @brief Restarts the normal movement of the enemy.
 *
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void Khorneth::restart() {
  Enemy::restart();

  if (has_blade()) {
    next_blade_attack_date = System::now() + 1000 * (2 + Random::get_number(4));
    blade_attack = false;
  }
}

/**
 * @brief Updates this enemy.
 */
void Khorneth::update() {
  
  if (is_in_normal_state()) {

    uint32_t now = System::now();

    // stop animation hurt of the blades
    if (is_left_blade_being_hurt() && now >= end_left_blade_hurt_date) {
      end_left_blade_hurt_date = 0;
      restore_movement();
      restart();

      if (left_blade_life <= 0) {
	Sound::play("stone");
	remove_sprite("enemies/khorneth_left_blade");

	if (!has_right_blade()) {
          start_final_phase();
	}
      }
    }
    else if (is_right_blade_being_hurt() && now >= end_right_blade_hurt_date) {
      end_right_blade_hurt_date = 0;
      restore_movement();
      restart();

      if (right_blade_life <= 0) {
	Sound::play("stone");
	remove_sprite("enemies/khorneth_right_blade");

	if (!has_left_blade()) {
          start_final_phase();
	}
      }
    }

    // blade attack
    else if (has_blade() && !blade_attack && now >= next_blade_attack_date) {
      blade_attack = true;
      int side;
      if (!has_right_blade()) {
	side = 0;
      }
      else if (!has_left_blade()) {
	side = 1;
      }
      else {
        side = Random::get_number(2);
      }
      std::string animation = (side == 0) ? "left_blade_attack" : "right_blade_attack";
      set_animation(animation);
      stop_movement();
    }
    else if (blade_attack && get_main_sprite()->is_animation_finished()) {
      blade_attack = false;
      restore_movement();
      restart();
    }

    // synchronize the blade animations
    Sprite *main_sprite = get_main_sprite();
    if (main_sprite->get_current_animation() != "stopped") {
      int frame = main_sprite->get_current_frame();
      if (has_left_blade() && !is_left_blade_being_hurt()) {
	get_left_blade_sprite()->set_current_frame(frame);
      }
      if (has_right_blade() && !is_right_blade_being_hurt()) {
	get_right_blade_sprite()->set_current_frame(frame);
      }
    }
  }

  Enemy::update();
}

