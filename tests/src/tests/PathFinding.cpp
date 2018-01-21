/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/core/Game.h"
#include "solarus/entities/CustomEntity.h"
#include "solarus/entities/EntityPtr.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Npc.h"
#include "solarus/movements/PathFinding.h"
#include "test_tools/TestEnvironment.h"

using namespace Solarus;

namespace {

/**
 * \brief Checks that a path computed corresponds to the expected result.
 */
void test_path_to_hero(TestEnvironment& env, Entity& entity) {

  Hero& hero = env.get_hero();

  entity.set_top_left_xy(144, 104);
  entity.notify_position_changed();
  hero.set_top_left_xy(200, 144);
  hero.notify_position_changed();
  // FIXME notify automatically from Entity::set_xy()

  PathFinding path_finder(env.get_map(), entity, hero);
  std::string path = path_finder.compute_path();

  const bool traversable = !hero.is_obstacle_for(entity);
  const std::string expected_path = traversable ? "7777700" : "77777";
  Debug::check_assertion(path == expected_path,
      std::string("Unexpected path: '") + path + "', expected '" + expected_path + "'");
}

/**
 * \brief Checks path finding on a custom entity towards a hero.
 */
void custom_entity_test(TestEnvironment& env) {

  CustomEntity& entity = *env.make_entity<CustomEntity>();

  test_path_to_hero(env, entity);
}

/**
 * \brief Checks path finding on an NPC towards a hero.
 */
void npc_test(TestEnvironment& env) {

  Npc& entity = *env.make_entity<Npc>();

  test_path_to_hero(env, entity);
}

}

/**
 * \brief Test for the path finding algorithm.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  custom_entity_test(env);
  npc_test(env);

  return 0;
}

