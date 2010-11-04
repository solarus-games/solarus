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

static void one_step_test(InteractiveEntity &npc) {

  Rectangle old_xy = npc.get_xy();

  PathMovement *movement = new PathMovement("0", 40, false, false, false); // 8 pixels to the right
  npc.set_movement(movement);

  while (!movement->is_finished()) {

    npc.get_game().update();
    System::update();
  }

  Debug::assert(npc.get_x() - old_xy.get_x() == 8
      && npc.get_y() - old_xy.get_y() == 0,
      StringConcat() << "Unexcepted coordinates for 'basic_test': " << npc.get_xy());
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
  InteractiveEntity *npc = new InteractiveEntity("npc1", LAYER_LOW, 160, 117,
      InteractiveEntity::NON_PLAYING_CHARACTER, "npc/sahasrahla", 0, "_none");
  map.get_entities().add_entity(npc);

  one_step_test(*npc);

  return 0;
}

