/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/CustomEnemy.h"
#include "lua/LuaContext.h"
#include "Map.h"

/**
 * @brief Constructor.
 * @param params name and position of the enemy to create
 * @param breed breed of the enemy (this determines its Lua script)
 */
CustomEnemy::CustomEnemy(const ConstructionParameters &params,
    const std::string& breed):
  Enemy(params),
  breed(breed) {

}

/**
 * @brief Destructor.
 */
CustomEnemy::~CustomEnemy() {
}

/**
 * @brief Initializes the enemy.
 */
void CustomEnemy::initialize() {

  get_lua_context().notify_enemy_created(*this);
}

/**
 * @brief Returns the breed of this enemy.
 * @return the breed
 */
const std::string& CustomEnemy::get_breed() {
  return breed;
}

/**
 * @brief Notifies this entity that it has just been added to a map.
 * @param map the map
 */
void CustomEnemy::set_map(Map& map) {

  Enemy::set_map(map);
}

/**
 * @brief Updates the enemy.
 */
void CustomEnemy::update() {

  Enemy::update();
  get_lua_context().enemy_on_update(*this);
}

/**
 * @brief Suspends or resumes the entity.
 * @param suspended true to suspend the entity, false to resume it
 */
void CustomEnemy::set_suspended(bool suspended) {

  Enemy::set_suspended(suspended);
  get_lua_context().enemy_on_suspended(*this, suspended);
}

/**
 * @brief Displays the entity on the map.
 */
void CustomEnemy::display_on_map() {

  if (is_visible()) {
    get_lua_context().enemy_on_pre_display(*this);
  }

  Enemy::display_on_map();

  if (is_visible()) {
    get_lua_context().enemy_on_post_display(*this);
  }
}
/**
 * @brief Notifies this entity that it was just enabled or disabled.
 * @param enabled true if the entity is now enabled
 */
void CustomEnemy::notify_enabled(bool enabled) {

  Enemy::notify_enabled(enabled);
  if (enabled) {
    get_lua_context().enemy_on_enabled(*this);
  }
  else {
    get_lua_context().enemy_on_disabled(*this);
  }
}

/**
 * @brief Notifies this enemy that it should restart his movement.
 *
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void CustomEnemy::restart() {

  Enemy::restart();
  get_lua_context().enemy_on_restart(*this);
}

/**
 * @brief Notifies this entity that it has just failed to change its position
 * because of obstacles.
 */
void CustomEnemy::notify_obstacle_reached() {

  Enemy::notify_obstacle_reached();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_obstacle_reached(*this);
  }
}

/**
 * @brief This function is called when the entity has just moved.
 */
void CustomEnemy::notify_position_changed() {

  Enemy::notify_position_changed();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_position_changed(*this, get_xy());
  }
}

/**
 * @brief This function is called when the layer of this entity has just changed.
 *
 * Redefine it if you need to be notified.
 */
void CustomEnemy::notify_layer_changed() {

  Enemy::notify_layer_changed();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_layer_changed(*this, get_layer());
  }
}

/**
 * @brief This function can be called by the movement object
 * to notify the entity when the movement has just changed
 * (e.g. the speed, the angle or the trajectory).
 */
void CustomEnemy::notify_movement_changed() {

  Enemy::notify_movement_changed();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_movement_changed(*this, *get_movement());
  }
}

/**
 * @brief This function is called when the movement of the entity is finished.
 */
void CustomEnemy::notify_movement_finished() {

  Enemy::notify_movement_finished();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_movement_finished(*this, *get_movement());
  }
}

/**
 * @brief Notifies this entity that the frame of one of its sprites has just changed.
 * @param sprite the sprite
 * @param animation the current animation
 * @param frame the new frame
 */
void CustomEnemy::notify_sprite_frame_changed(
    Sprite& sprite, const std::string& animation, int frame) {

  Enemy::notify_sprite_frame_changed(sprite, animation, frame);
  get_lua_context().enemy_on_sprite_frame_changed(
      *this, sprite, animation, frame);
}

/**
 * @brief Notifies this entity that the animation of one of its sprites
 * has just finished.
 * @param sprite the sprite
 * @param animation the animation just finished
 */
void CustomEnemy::notify_sprite_animation_finished(
    Sprite& sprite, const std::string& animation) {

  Enemy::notify_sprite_animation_finished(sprite, animation);
  get_lua_context().enemy_on_sprite_animation_finished(*this, sprite, animation);
}

/**
 * @brief This function is called when this enemy detects a collision with another enemy.
 * @param other the other enemy
 * @param other_sprite the other enemy's sprite that overlaps a sprite of this enemy
 * @param this_sprite this enemy's sprite that overlaps the other
 */
void CustomEnemy::notify_collision_with_enemy(Enemy& other,
    Sprite& other_sprite, Sprite& this_sprite) {

  if (is_in_normal_state()) {
    get_lua_context().enemy_on_collision_enemy(
        *this, other, other_sprite, this_sprite);
  }
}

/**
 * @brief This function is called when the enemy is attacked by a custom effect attack.
 * @param attack the attack
 * @param this_sprite the sprite of this enemy subject to the attack, or NULL
 * if the attack does not come from a pixel-precise collision test.
 */
void CustomEnemy::custom_attack(EnemyAttack attack, Sprite* this_sprite) {

  get_lua_context().enemy_on_custom_attack_received(*this, attack, this_sprite);
}

/**
 * @brief This function is called when the enemy has just been hurt.
 * @param source the source of the attack
 * @param attack the attack that was just successful
 * @param life_points the number of life points lost by this enemy (possibly 0)
 */
void CustomEnemy::notify_hurt(MapEntity& source, EnemyAttack attack, int life_points) {

  Enemy::notify_hurt(source, attack, life_points);
  get_lua_context().enemy_on_hurt(*this, attack, life_points);
}

/**
 * @brief This function is called when the enemy has just finished dying.
 */
void CustomEnemy::notify_dead() {

  Enemy::notify_dead();
  get_lua_context().enemy_on_dead(*this);
}

/**
 * @brief This function is called when the enemy is immobilized,
 * after the hurt animation.
 */
void CustomEnemy::notify_immobilized() {

  Enemy::notify_dead();
  get_lua_context().enemy_on_immobilized(*this);
}

/**
 * @brief Sends a message from another enemy to this enemy.
 * @param sender the sender
 * @param message the message
 */
void CustomEnemy::notify_message_received(Enemy& sender, const std::string& message) {

  Enemy::notify_message_received(sender, message);
  get_lua_context().enemy_on_message_received(*this, sender, message);
}
