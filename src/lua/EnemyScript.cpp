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
#include "enemies/CustomEnemy.h"
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

  std::string script_name = (std::string) "enemies/" + enemy.get_breed();
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

