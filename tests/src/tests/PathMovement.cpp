/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Debug.h"
#include "solarus/movements/PathMovement.h"
#include "test_tools/TestEnvironment.h"

using namespace Solarus;

namespace {

/**
 * \brief Tests a path movement composed of one step.
 */
void one_step_test(TestEnvironment& env, MapEntity& entity) {

  Point old_xy = entity.get_xy();

  std::shared_ptr<PathMovement> movement = std::make_shared<PathMovement>(
      "0", 100, false, false, false // 8 pixels to the right.
  );
  entity.set_movement(movement);

  while (!movement->is_finished()) {
    env.step();
  }

  Debug::check_assertion(entity.get_xy() - old_xy == Point(8, 0),
      "Unexpected coordinates for 'one_step_test #1'");
  Debug::check_assertion(movement->get_total_distance_covered() == 8,
      "Unexpected distance covered for 'one_step_test #1'");

  movement->set_path("4");  // 8 pixels to the left.
  while (!movement->is_finished()) {
    env.step();
  }
  Debug::check_assertion(entity.get_xy() == old_xy,
      "Unexpected coordinates for 'one_step_test #2'");
  Debug::check_assertion(movement->get_total_distance_covered() == 16,
      "Unexpected distance covered for 'one_step_test #2'");

  movement->set_path("3");
  while (!movement->is_finished()) {
    env.step();
  }
  Debug::check_assertion(entity.get_xy() - old_xy == Point(-8, -8),
      "Unexpected coordinates for 'one_step_test #3'");
  Debug::check_assertion(movement->get_total_distance_covered() == 24,
      "Unexpected distance covered for 'one_step_test #3'");

  movement->set_path("7");
  while (!movement->is_finished()) {
    env.step();
  }
  Debug::check_assertion(entity.get_xy() == old_xy,
      "Unexpected coordinates for 'one_step_test #4'");
  Debug::check_assertion(movement->get_total_distance_covered() == 32,
      "Unexpected distance covered for 'one_step_test #4'");

  entity.clear_movement();
}

/**
 * \brief Tests that a path movement returns the correct current direction.
 */
void direction_test(TestEnvironment& env, MapEntity& entity) {

  std::shared_ptr<PathMovement> movement = std::make_shared<PathMovement>(
      "5", 100, false, false, false  // 8 pixels to the right.
  );
  entity.set_movement(movement);

  Debug::check_assertion(movement->get_current_direction() == 5,
      "Unexpected current direction for 'direction_test #1'");

  while (!movement->is_finished()) {
    env.step();
  }

  // when the movement is finished, PathMovement::get_current_direction() must return the last direction
  Debug::check_assertion(movement->get_current_direction() == 5,
      "Unexpected last direction for 'direction_test #1'");
}

/**
 * \brief Tests a path movement composed of several 8-pixel steps.
 */
void multi_step_test(TestEnvironment& env, MapEntity& entity) {

  Point old_xy = entity.get_xy();

  std::shared_ptr<PathMovement> movement = std::make_shared<PathMovement>(
      "66", 100, false, false, false  // 16 pixels downwards.
  );
  entity.set_movement(movement);

  while (!movement->is_finished()) {
    env.step();
  }

  Debug::check_assertion(entity.get_xy() - old_xy == Point(0, 16),
      "Unexpected coordinates for 'multi_step_test #1'");
  Debug::check_assertion(movement->get_total_distance_covered() == 16,
      "Unexpected distance covered for 'multi_step_test #1'");

  movement->set_path("220");

  while (!movement->is_finished()) {
    env.step();
  }

  Debug::check_assertion(entity.get_xy() - old_xy == Point(8, 0),
      "Unexpected coordinates for 'multi_step_test #2'");
  Debug::check_assertion(movement->get_total_distance_covered() == 40,
      "Unexpected distance covered for 'multi_step_test #2'");

  entity.clear_movement();
}

/**
 * \brief Checks the snap-to-grid option of PathMovement.
 */
void snap_test(TestEnvironment& env, MapEntity& entity) {

  entity.set_top_left_xy(155, 108);  // Not aligned to the grid.
  Debug::check_assertion(!entity.is_aligned_to_grid(),
      "Entity should not be aligned on the grid");

  // #1: a path movement that does not require the entity to snap to the grid
  std::shared_ptr<PathMovement> movement = std::make_shared<PathMovement>(
      "56", 100, false, false, false
  );
  entity.set_movement(movement);
  while (!movement->is_finished()) {
    env.step();
  }
  Debug::check_assertion(entity.get_top_left_xy() == Point(147, 124),
      "Unexpected coordinates for 'snap_test #1'");
  Debug::check_assertion(movement->get_total_distance_covered() == 16,
      "Unexpected distance covered for 'snap_test #1'");
  entity.clear_movement();

  // #2: a path movement that requires the entity to snap to the grid
  movement = std::make_shared<PathMovement>(
      "21", 100, false, false, true
  );
  entity.set_movement(movement);
  while (!movement->is_finished()) {
    env.step();
  }
  Debug::check_assertion(entity.get_top_left_xy() == Point(152, 112),
      "Unexpected coordinates for 'snap_test #2'");
  Debug::check_assertion(movement->get_total_distance_covered() == 16,
      "Unexpected distance covered for 'snap_test #2'");

}

}

/**
 * Tests for the path movement.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  CustomEntity& entity = *env.make_entity<CustomEntity>({ 160, 177 });

  one_step_test(env, entity);
  multi_step_test(env, entity);
  direction_test(env, entity);
  snap_test(env, entity);

  return 0;
}

