/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/CustomEntity.h"
#include "solarus/entities/EntityPtr.h"
#include "solarus/entities/Hero.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/movements/PathFinding.h"
#include "solarus/Game.h"
#include "test_tools/TestEnvironment.h"

using namespace Solarus;

namespace {

/**
 * \brief Checks that a path computed corresponds to the expected result.
 */
void basic_test(TestEnvironment& env) {

  Hero& hero = env.get_hero();
  CustomEntity& entity = *env.make_entity<CustomEntity>();

  entity.set_top_left_xy(144, 104);
  hero.set_top_left_xy(200, 144);

  PathFinding path_finder(env.get_map(), entity, hero);
  std::string path = path_finder.compute_path();

  Debug::check_assertion(path == "7777700", "Unexpected path");
}

}

/**
 * \brief Test for the path finding algorithm.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  basic_test(env);

  return 0;
}

