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
#include "test_tools/TestEnvironment.h"

using namespace Solarus;

/**
 * \brief Runs a test written in Lua.
 *
 * The Lua test is implemented as a map of the testing quest.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  const std::string& map_id = env.get_arguments().get_argument_value("-map");
  Debug::check_assertion(!map_id.empty(), "No map specified");

  env.run_map(map_id);

  return 0;
}

