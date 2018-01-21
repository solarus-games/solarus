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
#include "solarus/movements/PixelMovement.h"
#include "test_tools/TestEnvironment.h"

using namespace Solarus;

namespace {

/**
 * \brief Tests that a simple movement results in the expected coordinates.
 */
void basic_test(TestEnvironment& env) {

  PixelMovement m("1 -2  2 1", 50, false, false);

  while (!m.is_finished()) {

    m.update();
    env.step();
  }

  Debug::check_assertion(m.get_x() == 3 && m.get_y() == -1,
      "Unexpected coordinates for 'basic_test'");
}

/**
 * \brief Tests that a pixel movement correctly loops as wanted.
 */
void loop_test(TestEnvironment& env) {

  uint32_t start_date = env.now();
  PixelMovement m("1 -2", 50, true, true);

  while (!m.is_finished() && env.now() < start_date + 200) {

    m.update();
    env.step();
  }

  Debug::check_assertion(m.get_x() == 3 && m.get_y() == -6,
      "Unexpected coordinates for 'loop_test'");
}

/**
 * \brief Tests that a pixel movement with empty trajectory is always finished.
 */
void empty_test(TestEnvironment& /* env */) {

  PixelMovement m("", 50, true, true);

  Debug::check_assertion(m.is_finished(),
      "Movement is not finished as expected in 'empty_test'");
}

/**
 * \brief Tests restarting a pixel movement after use.
 */
void restart_test(TestEnvironment& env) {

  PixelMovement m("2 1", 50, false, false);

  while (!m.is_finished()) {

    m.update();
    env.step();
  }

  Debug::check_assertion(m.get_x() == 2 && m.get_y() == 1,
      "Unexpected coordinates for 'restart_test #1'");

  m.set_trajectory("0 2");

  while (!m.is_finished()) {

    m.update();
    env.step();
  }

  Debug::check_assertion(m.get_x() == 2 && m.get_y() == 3,
      "Unexpected coordinates for 'restart_test #2'");
}

/**
 * \brief Tests a pixel movement whose trajectory is specified as a list.
 */
void list_test(TestEnvironment& env) {

  std::list<Point> trajectory;
  trajectory.push_back({ 3, 2 });
  trajectory.push_back({ -4, -5 });

  PixelMovement m("", 50, false, false);
  m.set_trajectory(trajectory);

  while (!m.is_finished()) {

    m.update();
    env.step();
  }

  Debug::check_assertion(m.get_x() == -1 && m.get_y() == -3,
      "Unexpected coordinates for 'list_test'");
}

}

/**
 * \brief Tests for the pixel movement.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  basic_test(env);
  loop_test(env);
  empty_test(env);
  restart_test(env);
  list_test(env);

  return 0;
}

