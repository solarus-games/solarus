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
#include "Solarus.h"
#include "movements/PathMovement.h"
#include "Game.h"
#include "Savegame.h"
#include "Map.h"
#include "entities/MapEntities.h"
#include "entities/InteractiveEntity.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

static void syntax_test(MapEntity &e) {

  Game &game = e.get_game();

  bool no_error = false;
  try {

    PathMovement *movement = new PathMovement("abc", 100, false, false, false); // incorrect path
    e.set_movement(movement);
    while (!movement->is_finished()) {
      game.update();
      System::update();
    }

    no_error = true;
  }
  catch (std::logic_error &e) {
    // expected behavior
  }

  e.clear_movement();
  Debug::assert(!no_error, "'syntax_test' failed to detect a syntax error");
}

static void one_step_test(MapEntity &e) {

  Game &game = e.get_game();
  Rectangle old_xy = e.get_xy();

  PathMovement *movement = new PathMovement("0", 100, false, false, false); // 8 pixels to the right
  e.set_movement(movement);

  while (!movement->is_finished()) {
    game.update();
    System::update();
  }

  Debug::assert(e.get_x() - old_xy.get_x() == 8 && e.get_y() - old_xy.get_y() == 0,
      StringConcat() << "Unexcepted coordinates for 'one_step_test #1': " << e.get_xy());
  Debug::assert(movement->get_total_distance_covered() == 8,
      StringConcat() << "Unexpected distance covered for 'one_step_test #1': " << movement->get_total_distance_covered());

  movement->set_path("4");
  while (!movement->is_finished()) {
    game.update();
    System::update();
  }
  Debug::assert(e.get_x() == old_xy.get_x() && e.get_y() == old_xy.get_y(),
      StringConcat() << "Unexpected coordinates for 'one_step_test #2': " << e.get_xy());
  Debug::assert(movement->get_total_distance_covered() == 16,
      StringConcat() << "Unexpected distance covered for 'one_step_test #2': " << movement->get_total_distance_covered());

  movement->set_path("3");
  while (!movement->is_finished()) {
    game.update();
    System::update();
  }
  Debug::assert(e.get_x() - old_xy.get_x() == -8 && e.get_y() - old_xy.get_y() == -8,
      StringConcat() << "Unexpected coordinates for 'one_step_test #3': " << e.get_xy());
  Debug::assert(movement->get_total_distance_covered() == 24,
      StringConcat() << "Unexpected distance covered for 'one_step_test #3': " << movement->get_total_distance_covered());

  movement->set_path("7");
  while (!movement->is_finished()) {
    game.update();
    System::update();
  }
  Debug::assert(e.get_x() == old_xy.get_x() && e.get_y() == old_xy.get_y(),
      StringConcat() << "Unexpected coordinates for 'one_step_test #4': " << e.get_xy());
  Debug::assert(movement->get_total_distance_covered() == 32,
      StringConcat() << "Unexpected distance covered for 'one_step_test #4': " << movement->get_total_distance_covered());

  e.clear_movement();
}

static void direction_test(MapEntity &e) {

  Game &game = e.get_game();
  Rectangle old_xy = e.get_xy();

  PathMovement *movement = new PathMovement("5", 100, false, false, false); // 8 pixels to the right
  e.set_movement(movement);

  Debug::assert(movement->get_current_direction() == 5,
      StringConcat() << "Unexcepted current direction for 'direction_test #1': " << movement->get_current_direction());

  while (!movement->is_finished()) {
    game.update();
    System::update();
  }

  // when the movement is finished, PathMovement::get_current_direction() must return the last direction
  Debug::assert(movement->get_current_direction() == 5,
      StringConcat() << "Unexcepted last direction for 'direction_test #1': " << movement->get_current_direction());
}

static void multi_step_test(MapEntity &e) {

  Game &game = e.get_game();
  Rectangle old_xy = e.get_xy();

  PathMovement *movement = new PathMovement("66", 100, false, false, false); // 16 pixels downwards
  e.set_movement(movement);

  while (!movement->is_finished()) {
    game.update();
    System::update();
  }

  Debug::assert(e.get_x() - old_xy.get_x() == 0 && e.get_y() - old_xy.get_y() == 16,
      StringConcat() << "Unexcepted coordinates for 'multi_step_test #1': " << e.get_xy());
  Debug::assert(movement->get_total_distance_covered() == 16,
      StringConcat() << "Unexpected distance covered for 'multi_step_test #1': " << movement->get_total_distance_covered());

  e.clear_movement();
}

/*
 * Test for the path movement.
 */
int main(int argc, char **argv) {

  Solarus solarus(argc, argv);
  Savegame savegame("save_initial.dat");
  Game game(solarus, savegame);
  game.update();

  Map &map = game.get_current_map();
  MapEntity *e = new InteractiveEntity("e", LAYER_LOW, 160, 117,
      InteractiveEntity::CUSTOM, "npc/sahasrahla", 0, "_none");
  map.get_entities().add_entity(e);

  syntax_test(*e);
  one_step_test(*e);
  multi_step_test(*e);
  direction_test(*e);

  return 0;
}

