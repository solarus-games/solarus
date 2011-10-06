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
#include "lua/EnemyScript.h"
#include "entities/CustomEnemy.h"
#include "lowlevel/Debug.h"
#include "Game.h"
#include <lua.hpp>

/**
 * @brief Constructor.
 * @param enemy the enemy to control with this script
 */
EnemyScript::EnemyScript(CustomEnemy& enemy):
  Script(MAIN_API | GAME_API | MAP_API | ENEMY_API),
  game(enemy.get_game()),
  enemy(enemy) {

  std::string script_name = std::string("enemies/" + enemy.get_breed());
  load_if_exists(script_name);
}

/**
 * @brief Destructor.
 */
EnemyScript::~EnemyScript() {

}

/**
 * @brief Returns the game that runs this script.
 * @return the game
 */
Game& EnemyScript::get_game() {
  return game;
}

/**
 * @brief Returns the current map of the game.
 * @return the map
 */
Map& EnemyScript::get_map() {
  return enemy.get_map();
}

/**
 * @brief Returns the enemy controlled by this script.
 * @return the enemy
 */
Enemy& EnemyScript::get_enemy() {
  return enemy;
}

/**
 * @brief Reads some global variables from the sprite to initializes the
 * corresponding properties for the enemy.
 */
/* I'm not sure allowing to set properties like this is a good idea.
void EnemyScript::read_globals() {

  lua_State* l = context;

  // life
  lua_getglobal(l, "life");
  if (!lua_isnil(l, 1)) {
    int life = luaL_checkinteger(l, 1);
    enemy.set_life(life);
  }

  // damage
  lua_getglobal(l, "damage");
  if (!lua_isnil(l, 1)) {
    int damage = luaL_checkinteger(l, 1);
    enemy.damage_on_hero = damage;
  }

  // magic damage
  lua_getglobal(l, "magic_damage");
  if (!lua_isnil(l, 1)) {
    int magic_damage = luaL_checkinteger(l, 1);
    enemy.magic_damage_on_hero = magic_damage;
  }

  // pushed back when hurt
  lua_getglobal(l, "pushed_back_when_hurt");
  if (!lua_isnil(l, 1)) {
    bool push_back = lua_toboolean(l, 1);
    enemy.set_pushed_back_when_hurt(push_back);
  }

  // hurt sound style
  lua_getglobal(l, "hurt_sound_style");
  if (!lua_isnil(l, 1)) {
    const std::string& style_name = luaL_checkstring(l, 1);
    enemy.hurt_sound_style = Enemy::get_hurt_sound_style_by_name(style_name);
  }

  // minimum shield needed
  lua_getglobal(l, "minimum_shield_needed");
  if (!lua_isnil(l, 1)) {
    int shield_level = luaL_checkinteger(l, 1);
    enemy.minimum_shield_needed = shield_level;
  }
}
*/

/**
 * @brief Updates the script.
 */
void EnemyScript::update() {

  Script::update();

  if (is_loaded()) {
    event_update();
  }
}

/**
 * @brief This function is called when the game is being suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void EnemyScript::set_suspended(bool suspended) {

  Script::set_suspended(suspended);

  if (is_loaded()) {
    event_set_suspended(suspended);
  }
}

/**
 * @brief Notifies the script that it can update itself.
 *
 * This function is called at each cycle of the main loop,
 * so if you define it in your script, take care of the performances.
 */
void EnemyScript::event_update() {

  notify_script("event_update");
}

/**
 * @brief Notifies the script that the game is being suspended or resumed.
 * @param suspended true if the game becomes suspended, false if it is resumed.
 */
void EnemyScript::event_set_suspended(bool suspended) {

  notify_script("event_suspended", "b", suspended);
}

/**
 * @brief Notifies the script that an instance of the enemy has just been created on the map.
 */
void EnemyScript::event_appear() {

  notify_script("event_appear");
}

/**
 * @brief Notifies the script that the enemy has just been enabled.
 */
void EnemyScript::event_enabled() {

  notify_script("event_enabled");
}

/**
 * @brief Notifies the script that the enemy has just been disabled.
 */
void EnemyScript::event_disabled() {

  notify_script("event_disabled");
}

/**
 * @brief Notifies the script that the enemy is restarting its movement because
 * something happended (for example the enemy has just been created,
 * or it was just hurt)
 */
void EnemyScript::event_restart() {

  notify_script("event_restart");
}

/**
 * @brief Notifies the script that the enemy is about to be displayed.
 *
 * Your script may display additional things below the enemy.
 */
void EnemyScript::event_pre_display() {

  notify_script("event_pre_display");

}

/**
 * @brief Notifies the script that the enemy is about to be displayed.
 *
 * Your script may display additional things above the enemy.
 */
void EnemyScript::event_post_display() {

  notify_script("event_post_display");
}

/**
 * @brief Notifies the script that the enemy has just moved.
 * @param xy the new position of the enemy on the map
 */
void EnemyScript::event_position_changed(const Rectangle& xy) {

  notify_script("event_position_changed", "ii", xy.get_x(), xy.get_y());
}

/**
 * @brief Notifies the script that the layer of the enemy has changed.
 * @param layer the new layer
 */
void EnemyScript::event_layer_changed(Layer layer) {

  notify_script("event_layer_changed", "i", layer);
}

/**
 * @brief Notifies the script that the movement of the enemy
 * was stopped because of an obstacle.
 */
void EnemyScript::event_obstacle_reached() {

  notify_script("event_obstacle_reached");
}

/**
 * @brief Notifies the script that something has just changed
 * in the movement of the enemy.
 * @param movement the current movement of the enemy
 */
void EnemyScript::event_movement_changed(Movement& movement) {

  notify_script("event_movement_changed", "i", create_movement_handle(movement));
}

/**
 * @brief Notifies the script that the movement of the enemy has just finished.
 * @param movement the current movement of the enemy
 */
void EnemyScript::event_movement_finished(Movement& movement) {

  notify_script("event_movement_finished", "i", create_movement_handle(movement));
}

/**
 * @brief Notifies the script that the animation of a sprite of the enemy has just finished.
 * @param sprite a sprite of the enemy
 * @param animation the animation that was playing
 */
void EnemyScript::event_sprite_animation_finished(Sprite& sprite, const std::string& animation) {

  notify_script("event_sprite_animation_finished", "is",
      create_sprite_handle(sprite), animation.c_str());
}

/**
 * @brief Notifies the script that the animation of a sprite of the enemy has just finished.
 * @param sprite a sprite of the enemy
 * @param animation the current animation
 * @param frame the new frame
 */
void EnemyScript::event_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame) {

  notify_script("event_sprite_frame_changed", "isi",
      create_sprite_handle(sprite), animation.c_str(), frame);
}

/**
 * @brief Notifies the script that this enemy has detected a collision with another enemy.
 * @param other_name name of the other enemy
 * @param other_sprite the other enemy's sprite that overlaps a sprite of this enemy
 * @param this_sprite this enemy's sprite that overlaps the other
 */
void EnemyScript::event_collision_enemy(const std::string& other_name, Sprite& other_sprite, Sprite& this_sprite) {

  notify_script("event_collision_enemy", "sii", other_name.c_str(),
      create_sprite_handle(other_sprite), create_sprite_handle(this_sprite));
}

/**
 * @brief Notifies the script that the enemy is receiving an attack
 * with a custom effect, which means that your script decides what happens.
 * and returns the number of life points to remove.
 * @param attack the type of attack received
 * @param sprite the sprite of the enemy that receives the attack, or NULL
 * if the attack does not come from a pixel-precise collision test.
 * @return the number of life points to remove from the enemy
 */
int EnemyScript::event_custom_attack_received(EnemyAttack attack, Sprite* sprite) {

  int result = 0;
  if (sprite != NULL) {
    // pixel-perfect collision
    notify_script("event_custom_attack_received", "si i",
        Enemy::get_attack_name(attack).c_str(), create_sprite_handle(*sprite), &result);
  }
  else {
    notify_script("event_custom_attack_received", "s i",
        Enemy::get_attack_name(attack).c_str(), &result);
  }
  return result;
}

/**
 * @brief Notifies the script that the enemy has just been hurt.
 * @param attack the type of attack received
 * @param life_lost the number of life points just lost
 */
void EnemyScript::event_hurt(EnemyAttack attack, int life_lost) {

  notify_script("event_hurt", "si", Enemy::get_attack_name(attack).c_str(), life_lost);
}

/**
 * @brief Notifies the script that the enemy just died.
 *
 * This function is called when the dying animation is totally finished.
 */
void EnemyScript::event_dead() {

  notify_script("event_dead");
}

/**
 * @brief Notifies the script that the enemy has just been immobilized.
 *
 * This function is called after the hurt animation is finished,
 * when the immobilized animation has just started.
 */
void EnemyScript::event_immobilized() {

  notify_script("event_immobilized");
}

/**
 * @brief Notifies the script the enemy has received a message from another enemy.
 * @param src_enemy_name name of the sender
 * @param message the message received
 */
void EnemyScript::event_message_received(const std::string& src_enemy_name, const std::string& message) {

  notify_script("event_message_received", "ss", src_enemy_name.c_str(), message.c_str());
}
