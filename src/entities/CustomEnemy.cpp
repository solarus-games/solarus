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
#include "entities/CustomEnemy.h"
#include "lua/EnemyScript.h"
#include "Map.h"

/**
 * @brief Constructor.
 * @param params name and position of the enemy to create
 * @param breed breed of the enemy (this determines its Lua script)
 */
CustomEnemy::CustomEnemy(const ConstructionParameters &params, const std::string& breed):
  Enemy(params),
  breed(breed),
  script(NULL) {

}

/**
 * @brief Destructor.
 */
CustomEnemy::~CustomEnemy() {

  delete script;
}

/**
 * @brief Initializes the enemy.
 */
void CustomEnemy::initialize() {

  if (script == NULL) {   // TODO when CustomEnemy is merged with Enemy, make this test in notify_enabled() instead
    script = new EnemyScript(*this);
    script->set_suspended(is_suspended());
    script->event_appear();
  }
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

  if (script != NULL) {
    script->update();
  }
}

/**
 * @brief Suspends or resumes the entity.
 * @param suspended true to suspend the entity, false to resume it
 */
void CustomEnemy::set_suspended(bool suspended) {

  Enemy::set_suspended(suspended);

  if (script != NULL) {
    script->set_suspended(suspended);
  }
}

/**
 * @brief Displays the entity on the map.
 */
void CustomEnemy::display_on_map() {

  if (is_visible()) {
    script->event_pre_display();
  }

  Enemy::display_on_map();

  if (is_visible()) {
    script->event_post_display();
  }
}
/**
 * @brief Notifies this entity that it was just enabled or disabled.
 * @param enabled true if the entity is now enabled
 */
void CustomEnemy::notify_enabled(bool enabled) {

  Enemy::notify_enabled(enabled);

  if (script != NULL) {
    if (enabled) {
      script->event_enabled();
    }
    else {
      script->event_disabled();
    }
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
  script->event_restart();
}

/**
 * @brief Notifies this entity that it has just tried to change its position.
 *
 * This function is called only when the movement is not suspended.
 *
 * @param success true if the position has actually just changed
 */
void CustomEnemy::notify_movement_tried(bool success) {

  Enemy::notify_movement_tried(success);

  if (!is_being_hurt() && !success) {
    script->event_obstacle_reached();
  }
}

/**
 * @brief This function is called when the entity has just moved.
 */
void CustomEnemy::notify_position_changed() {

  Enemy::notify_position_changed();

  if (!is_being_hurt()) {
    script->event_position_changed(get_xy());
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
    script->event_layer_changed(get_layer());
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
    script->event_movement_changed(*get_movement());
  }
}

/**
 * @brief This function is called when the movement of the entity is finished.
 */
void CustomEnemy::notify_movement_finished() {

  Enemy::notify_movement_finished();

  if (!is_being_hurt()) {
    script->event_movement_finished(*get_movement());
  }
}

/**
 * @brief Notifies this entity that the frame of one of its sprites has just changed.
 * @param sprite the sprite
 * @param animation the current animation
 * @param frame the new frame
 */
void CustomEnemy::notify_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame) {

  Enemy::notify_sprite_frame_changed(sprite, animation, frame);
  script->event_sprite_frame_changed(sprite, animation, frame);
}

/**
 * @brief Notifies this entity that the animation of one of its sprites
 * has just finished.
 * @param sprite the sprite
 * @param animation the animation just finished
 */
void CustomEnemy::notify_sprite_animation_finished(Sprite& sprite, const std::string& animation) {

  Enemy::notify_sprite_animation_finished(sprite, animation);
  script->event_sprite_animation_finished(sprite, animation);
}

/**
 * @brief This function is called when this enemy detects a collision with another enemy.
 * @param other the other enemy
 * @param other_sprite the other enemy's sprite that overlaps a sprite of this enemy
 * @param this_sprite this enemy's sprite that overlaps the other
 */
void CustomEnemy::notify_collision_with_enemy(Enemy& other, Sprite& other_sprite, Sprite& this_sprite) {

  if (is_in_normal_state()) {
    script->event_collision_enemy(other.get_name(), other_sprite, this_sprite);
  }
}

/**
 * @brief This function is called when the enemy is attacked by a custom effect attack.
 * @param attack the attack
 * @param this_sprite the sprite of this enemy subject to the attack, or NULL
 * if the attack does not come from a pixel-precise collision test.
 * @return the number of health points lost (can be 0)
 */
int CustomEnemy::custom_attack(EnemyAttack attack, Sprite* this_sprite) {

  if (!is_in_normal_state()) {
    return 0;
  }

  return script->event_custom_attack_received(attack, this_sprite);
}

/**
 * @brief This function is called when the enemy has just been hurt.
 * @param source the source of the attack
 * @param attack the attack that was just successful
 * @param life_points the number of life points lost by this enemy (possibly 0)
 */
void CustomEnemy::notify_hurt(MapEntity& source, EnemyAttack attack, int life_points) {

  Enemy::notify_hurt(source, attack, life_points);
  script->event_hurt(attack, life_points);
}

/**
 * @brief This function is called when the enemy has just finished dying.
 */
void CustomEnemy::notify_dead() {

  Enemy::notify_dead();
  script->event_dead();
}

/**
 * @brief This function is called when the enemy is immobilized,
 * after the hurt animation.
 */
void CustomEnemy::notify_immobilized() {

  Enemy::notify_dead();
  script->event_immobilized();
}

/**
 * @brief Sends a message from another enemy to this enemy.
 * @param sender the sender
 * @param message the message
 */
void CustomEnemy::notify_message_received(Enemy& sender, const std::string& message) {

  Enemy::notify_message_received(sender, message);
  script->event_message_received(sender.get_name(), message);
}
