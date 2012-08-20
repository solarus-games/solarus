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
#include "lua/LuaContext.h"
#include "entities/Enemy.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "movements/Movement.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Geometry.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"
#include <lua.hpp>

const std::string LuaContext::entity_enemy_module_name = "sol.entity.enemy";

const char* LuaContext::enemy_attack_names[] = {
  "sword",
  "thrown_item",
  "explosion",
  "arrow",
  "hookshot",
  "boomerang",
  "fire",
  "script",
  NULL
};

const char* LuaContext::enemy_hurt_style_names[] = {
  "normal",
  "monster",
  "boss",
  NULL
};

const char* LuaContext::enemy_obstacle_behavior_names[] = {
  "normal",
  "flying",
  "swimming",
  NULL
};

/**
 * @brief Initializes the enemy features provided to Lua.
 */
void LuaContext::register_enemy_module() {

  static const luaL_Reg methods[] = {
      { "get_map", enemy_api_get_map },
      { "get_name", enemy_api_get_name },
      { "get_life", enemy_api_get_life },
      { "set_life", enemy_api_set_life },
      { "add_life", enemy_api_add_life },
      { "remove_life", enemy_api_remove_life },
      { "get_damage", enemy_api_get_damage },
      { "set_damage", enemy_api_set_damage },
      { "get_magic_damage", enemy_api_get_magic_damage },
      { "set_magic_damage", enemy_api_set_magic_damage },
      { "is_pushed_back_when_hurt", enemy_api_is_pushed_back_when_hurt },
      { "set_pushed_back_when_hurt", enemy_api_set_pushed_back_when_hurt },
      { "get_push_hero_on_sword", enemy_api_get_push_hero_on_sword },
      { "set_push_hero_on_sword", enemy_api_set_push_hero_on_sword },
      { "get_can_hurt_hero_running", enemy_api_get_can_hurt_hero_running },
      { "set_can_hurt_hero_running", enemy_api_set_can_hurt_hero_running },
      { "get_hurt_style", enemy_api_get_hurt_style },
      { "set_hurt_style", enemy_api_set_hurt_style },
      { "get_can_attack", enemy_api_get_can_attack },
      { "set_can_attack", enemy_api_set_can_attack },
      { "get_minimum_shield_needed", enemy_api_get_minimum_shield_needed },
      { "set_minimum_shield_needed", enemy_api_set_minimum_shield_needed },
      { "set_attack_consequence", enemy_api_set_attack_consequence },
      { "set_attack_consequence_sprite", enemy_api_set_attack_consequence_sprite },
      { "set_default_attack_consequences", enemy_api_set_default_attack_consequences },
      { "set_default_attack_consequences_sprite", enemy_api_set_default_attack_consequences_sprite },
      { "set_invincible", enemy_api_set_invincible },
      { "set_invincible_sprite", enemy_api_set_invincible_sprite },
      { "set_layer_independent_collisions", enemy_api_set_layer_independent_collisions },
      { "set_treasure", enemy_api_set_treasure },
      { "set_no_treasure", enemy_api_set_no_treasure },
      { "set_random_treasure", enemy_api_set_random_treasure },
      { "get_obstacle_behavior", enemy_api_get_obstacle_behavior },
      { "set_obstacle_behavior", enemy_api_set_obstacle_behavior },
      { "get_optimization_distance", enemy_api_get_optimization_distance },
      { "set_optimization_distance", enemy_api_set_optimization_distance },
      { "get_size", enemy_api_get_size },
      { "set_size", enemy_api_set_size },
      { "get_origin", enemy_api_get_origin },
      { "set_origin", enemy_api_set_origin },
      { "get_position", enemy_api_get_position },
      { "set_position", enemy_api_set_position },
      { "get_distance_to_hero", enemy_api_get_distance_to_hero },
      { "get_angle_to_hero", enemy_api_get_angle_to_hero },
      { "test_obstacles", enemy_api_test_obstacles },
      { "snap_to_grid", enemy_api_snap_to_grid },
      { "get_movement", enemy_api_get_movement },
      { "start_movement", enemy_api_start_movement },
      { "stop_movement", enemy_api_stop_movement },
      { "restart", enemy_api_restart },
      { "hurt", enemy_api_hurt },
      { "create_sprite", enemy_api_create_sprite },
      { "remove_sprite", enemy_api_remove_sprite },
      { "get_sprite", enemy_api_get_sprite },
      { "create_son", enemy_api_create_son },
      { "get_father", enemy_api_get_father },
      { "send_message", enemy_api_send_message },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { NULL, NULL }
  };
  register_type(entity_enemy_module_name, methods, metamethods);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is an
 * enemy and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The enemy.
 */
Enemy& LuaContext::check_enemy(lua_State* l, int index) {
  return static_cast<Enemy&>(check_userdata(l, index, entity_enemy_module_name));
}

/**
 * @brief Pushes an enemy userdata onto the stack.
 * @param l A Lua context.
 * @param enemy An enemy.
 */
void LuaContext::push_enemy(lua_State* l, Enemy& enemy) {
  push_userdata(l, enemy);
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_game.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_map(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  push_map(l, enemy.get_map());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_name(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushstring(l, enemy.get_name().c_str());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_life());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int life = luaL_checkinteger(l, 2);

  enemy.set_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_add_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int points = luaL_checkinteger(l, 2);

  enemy.set_life(enemy.get_life() + points);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_remove_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int points = luaL_checkinteger(l, 2);

  enemy.set_life(enemy.get_life() - points);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_damage());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int damage = luaL_checkinteger(l, 2);

  enemy.set_damage(damage);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_magic_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_magic_damage());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_magic_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int magic_damage = luaL_checkinteger(l, 2);

  enemy.set_magic_damage(magic_damage);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_is_pushed_back_when_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_pushed_back_when_hurt());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_pushed_back_when_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool push_back = lua_toboolean(l, 2);

  enemy.set_pushed_back_when_hurt(push_back);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_push_hero_on_sword(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_push_hero_on_sword());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_push_hero_on_sword(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool push = lua_toboolean(l, 2);

  enemy.set_push_hero_on_sword(push);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_can_hurt_hero_running(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_can_hurt_hero_running());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_can_hurt_hero_running(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool can_hurt_hero_running = lua_toboolean(l, 2);

  enemy.set_can_hurt_hero_running(can_hurt_hero_running);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_hurt_style(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Enemy::HurtStyle hurt_style = enemy.get_hurt_style();

  lua_pushstring(l, enemy_hurt_style_names[hurt_style]);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_hurt_style(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int hurt_style = luaL_checkoption(l, 2, NULL, enemy_hurt_style_names);

  enemy.set_hurt_style(Enemy::HurtStyle(hurt_style));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_can_attack(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_can_attack());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_can_attack(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool can_attack = lua_toboolean(l, 2);

  enemy.set_can_attack(can_attack);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_minimum_shield_needed(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  int shield_level = enemy.get_minimum_shield_needed();

  lua_pushinteger(l, shield_level);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_minimum_shield_needed(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int shield_level = luaL_checkinteger(l, 2);

  enemy.set_minimum_shield_needed(shield_level);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_attack_consequence(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int attack = luaL_checkoption(l, 2, NULL, enemy_attack_names);

  if (lua_isnumber(l, 3)) {
    int life_points = luaL_checkinteger(l, 3);
    Debug::check_assertion(life_points > 0, StringConcat()
        << "Invalid attack consequence: " << life_points);
    enemy.set_attack_consequence(EnemyAttack(attack), EnemyReaction::HURT, life_points);
  }
  else {
    // TODO: simplify or encapsulate the C++ part of specifying attack consequences
    // (but the important thing is that the Lua API is easy to use)
    const std::string& reaction_name = lua_tostring(l, 3);
    EnemyReaction::ReactionType reaction = EnemyReaction::get_reaction_by_name(reaction_name);
    enemy.set_attack_consequence(EnemyAttack(attack), reaction);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_attack_consequence_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);
  int attack = luaL_checkoption(l, 3, NULL, enemy_attack_names);

  if (lua_isnumber(l, 4)) {
    int life_points = luaL_checkinteger(l, 4);
    Debug::check_assertion(life_points > 0, StringConcat()
        << "Invalid attack consequence: " << life_points);
    enemy.set_attack_consequence_sprite(sprite, EnemyAttack(attack), EnemyReaction::HURT, life_points);
  }
  else {
    const std::string& reaction_name = lua_tostring(l, 4);
    EnemyReaction::ReactionType reaction = EnemyReaction::get_reaction_by_name(reaction_name);
    enemy.set_attack_consequence_sprite(sprite, EnemyAttack(attack), reaction);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_default_attack_consequences(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.set_default_attack_consequences();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_default_attack_consequences_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);

  enemy.set_default_attack_consequences_sprite(sprite);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_invincible(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.set_no_attack_consequences();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_invincible_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);

  enemy.set_no_attack_consequences_sprite(sprite);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_layer_independent_collisions(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool independent = lua_toboolean(l, 2) != 0;

  enemy.set_layer_independent_collisions(independent);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_treasure(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  const std::string& item_name = luaL_checkstring(l, 2);
  int variant = luaL_checkinteger(l, 3);
  int savegame_variable = luaL_checkinteger(l, 4);

  Treasure treasure(enemy.get_game(), item_name, variant, savegame_variable);
  enemy.set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_no_treasure(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Treasure treasure(enemy.get_game(), "_none", 1, -1);
  enemy.set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_random_treasure(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Treasure treasure(enemy.get_game(), "_random", 1, -1);
  enemy.set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_obstacle_behavior(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Enemy::ObstacleBehavior behavior = enemy.get_obstacle_behavior();

  lua_pushstring(l, enemy_obstacle_behavior_names[behavior]);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_obstacle_behavior(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int behavior = luaL_checkoption(l, 2, NULL, enemy_obstacle_behavior_names);

  enemy.set_obstacle_behavior(Enemy::ObstacleBehavior(behavior));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_optimization_distance(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_optimization_distance());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_optimization_distance(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int distance = luaL_checkinteger(l, 2);

  enemy.set_optimization_distance(distance);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_size(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_width());
  lua_pushinteger(l, enemy.get_height());
  return 2;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_size(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int width = luaL_checkinteger(l, 2);
  int height = luaL_checkinteger(l, 3);

  enemy.set_size(width, height);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_origin(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  const Rectangle& origin = enemy.get_origin();

  lua_pushinteger(l, origin.get_x());
  lua_pushinteger(l, origin.get_y());
  return 2;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_origin(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);

  enemy.set_origin(x, y);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_position(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_x());
  lua_pushinteger(l, enemy.get_y());
  lua_pushinteger(l, enemy.get_layer());
  return 3;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_position(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  int layer = -1;
  if (lua_gettop(l) >= 4) {
    layer = luaL_checkinteger(l, 4);
  }

  enemy.set_xy(x, y);
  if (layer != -1) {
    MapEntities& entities = enemy.get_map().get_entities();
    entities.set_entity_layer(enemy, Layer(layer));
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_distance_to_hero(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Hero& hero = enemy.get_map().get_entities().get_hero();

  lua_pushinteger(l, enemy.get_distance(hero));
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_angle_to_hero(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Hero& hero = enemy.get_map().get_entities().get_hero();
  double angle = Geometry::get_angle(enemy.get_x(), enemy.get_y(),
      hero.get_x(), hero.get_y());

  lua_pushnumber(l, angle);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_test_obstacles(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int dx = luaL_checkinteger(l, 2);
  int dy = luaL_checkinteger(l, 3);

  Rectangle bounding_box = enemy.get_bounding_box();
  bounding_box.add_xy(dx, dy);

  lua_pushboolean(l, enemy.get_map().test_collision_with_obstacles(
      enemy.get_layer(), bounding_box, enemy));
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_snap_to_grid(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.set_aligned_to_grid();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_movement(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Movement* movement = enemy.get_movement();
  if (movement == NULL) {
    lua_pushnil(l);
  }
  else {
    push_userdata(l, *movement);
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_start_movement(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Movement& movement = check_movement(l, 2);

  movement.set_suspended(false);
  enemy.clear_movement();
  enemy.set_movement(&movement);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_stop_movement(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.clear_movement();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_restart(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.restart();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int life_points = luaL_checkinteger(l, 2);

  if (enemy.is_in_normal_state() && !enemy.is_invulnerable()) {
    Hero& hero = enemy.get_map().get_entities().get_hero();
    enemy.set_attack_consequence(ATTACK_SCRIPT, EnemyReaction::HURT, life_points);
    enemy.try_hurt(ATTACK_SCRIPT, hero, NULL);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_create_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  const std::string& animation_set_id = luaL_checkstring(l, 2);

  Sprite& sprite = enemy.create_sprite(animation_set_id, true);

  push_userdata(l, sprite);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_remove_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Sprite* sprite;
  if (lua_gettop(l) >= 2) {
    sprite = &check_sprite(l, 2);
  }
  else {
    sprite = &enemy.get_sprite();
  }

  enemy.remove_sprite(sprite);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  push_sprite(l, enemy.get_sprite());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_create_son(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  const std::string& breed = luaL_checkstring(l, 3);
  int x = luaL_checkinteger(l, 4);
  int y = luaL_checkinteger(l, 5);
  int layer;
  if (lua_gettop(l) >= 6) {
    layer = luaL_checkinteger(l, 6);
  }
  else {
    layer = enemy.get_layer();
  }

  x += enemy.get_x();
  y += enemy.get_y();

  Game& game = enemy.get_game();
  MapEntities& entities = enemy.get_map().get_entities();
  Treasure treasure = Treasure(game, "_random", 1, -1);
  Enemy* son = (Enemy*) Enemy::create(game, breed, Enemy::RANK_NORMAL, -1,
      name, Layer(layer), x, y, 0, treasure);
  son->set_father_name(enemy.get_name());
  son->set_optimization_distance(enemy.get_optimization_distance());
  entities.add_entity(son);
  son->restart();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_father(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushstring(l, enemy.get_father_name().c_str());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_send_message(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  const std::string& dst_enemy_name = luaL_checkstring(l, 2);
  const std::string& message = luaL_checkstring(l, 3);

  MapEntities& entities = enemy.get_map().get_entities();
  MapEntity* entity = entities.get_entity(dst_enemy_name);
  Debug::check_assertion(entity->get_type() == ENEMY,
      "The destination entity is not an enemy");
  Enemy* dst_enemy = (Enemy*) entity;
  dst_enemy->notify_message_received(enemy, message);

  return 1;
}

/**
 * @brief Calls the on_() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_update(Enemy& enemy) {

  push_enemy(l, enemy);
  on_update();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_suspended() method of a Lua enemy.
 * @param enemy An enemy.
 * @param suspended true if the enemy is suspended.
 */
void LuaContext::enemy_on_suspended(Enemy& enemy, bool suspended) {

  push_enemy(l, enemy);
  on_suspended(suspended);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_appeared() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_appear(Enemy& enemy) {

  push_enemy(l, enemy);
  on_appear();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_enabled() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_enabled(Enemy& enemy) {

  push_enemy(l, enemy);
  on_enabled();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_disabled() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_disabled(Enemy& enemy) {

  push_enemy(l, enemy);
  on_disabled();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_restart() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_restart(Enemy& enemy) {

  push_enemy(l, enemy);
  on_restart();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_pre_draw() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_pre_draw(Enemy& enemy) {

  push_enemy(l, enemy);
  on_pre_draw();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_post_draw() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_post_draw(Enemy& enemy) {

  push_enemy(l, enemy);
  on_post_draw();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_position_changed() method of a Lua enemy.
 * @param enemy An enemy.
 * @param xy The new position.
 */
void LuaContext::enemy_on_position_changed(Enemy& enemy, const Rectangle& xy) {

  push_enemy(l, enemy);
  on_position_changed(xy);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_layer_changed() method of a Lua enemy.
 * @param enemy An enemy.
 * @param layer The new layer.
 */
void LuaContext::enemy_on_layer_changed(Enemy& enemy, Layer layer) {

  push_enemy(l, enemy);
  on_layer_changed(layer);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_obstacle_reached() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_obstacle_reached(Enemy& enemy) {

  push_enemy(l, enemy);
  on_obstacle_reached();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_movement_changed() method of a Lua enemy.
 * @param enemy An enemy.
 * @param movement Its movement.
 */
void LuaContext::enemy_on_movement_changed(Enemy& enemy,
    Movement& movement) {

  push_enemy(l, enemy);
  on_movement_changed(movement);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_movement_finished() method of a Lua enemy.
 * @param enemy An enemy.
 * @param movement Its movement.
 */
void LuaContext::enemy_on_movement_finished(Enemy& enemy,
    Movement& movement) {

  push_enemy(l, enemy);
  on_movement_finished(movement);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_sprite_animation_finished() method of a Lua enemy.
 * @param enemy An enemy.
 * @param sprite A sprite whose animation has just finished.
 * @param animation Name of the animation finished.
 */
void LuaContext::enemy_on_sprite_animation_finished(Enemy& enemy,
    Sprite& sprite, const std::string& animation) {

  push_enemy(l, enemy);
  on_sprite_animation_finished(sprite, animation);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_sprite_frame_changed() method of a Lua enemy.
 * @param enemy An enemy.
 * @param sprite A sprite whose animation frame has just changed.
 * @param animation Name of the sprite animation.
 * @param frame The new frame.
 */
void LuaContext::enemy_on_sprite_frame_changed(Enemy& enemy,
    Sprite& sprite, const std::string& animation, int frame) {

  push_enemy(l, enemy);
  on_sprite_frame_changed(sprite, animation, frame);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_collision_enemy() method of a Lua enemy.
 * @param enemy An enemy.
 * @param other_enemy Another enemy colliding with the first one.
 * @param other_sprite Colliding sprite of the other enemy.
 * @param this_sprite Colliding sprite of the first enemy.
 */
void LuaContext::enemy_on_collision_enemy(Enemy& enemy,
    Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite) {

  push_enemy(l, enemy);
  on_collision_enemy(other_enemy, other_sprite, this_sprite);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_custom_attack_received() method of a Lua enemy.
 * @param enemy An enemy.
 * @param attack The attack received.
 * @param sprite The sprite that receives the attack if any.
 */
void LuaContext::enemy_on_custom_attack_received(Enemy& enemy,
    EnemyAttack attack, Sprite* sprite) {

  push_enemy(l, enemy);
  on_custom_attack_received(attack, sprite);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_hurt() method of a Lua enemy.
 * @param enemy An enemy.
 * @param attack The attack received.
 * @param life_lost Number of life points just lost.
 */
void LuaContext::enemy_on_hurt(Enemy& enemy, EnemyAttack attack, int life_lost) {

  push_enemy(l, enemy);
  on_hurt(attack, life_lost);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_dead() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_dead(Enemy& enemy) {

  push_enemy(l, enemy);
  on_dead();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_immobilized() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_immobilized(Enemy& enemy) {

  push_enemy(l, enemy);
  on_immobilized();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_message_received() method of a Lua enemy.
 * @param enemy An enemy.
 * @param src_enemy The sender.
 * @param message The message received.
 */
void LuaContext::enemy_on_message_received(Enemy& enemy,
    Enemy& src_enemy, const std::string& message) {

  // TODO remove this (useless with the shared Lua context)
  push_enemy(l, enemy);
  on_message_received(src_enemy, message);
  lua_pop(l, 1);
}
