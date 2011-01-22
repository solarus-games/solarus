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
#include "movements/PathFinding.h"
#include "Game.h"
#include "Savegame.h"
#include "Map.h"
#include "entities/MapEntities.h"
#include "entities/Enemy.h"
#include "entities/Hero.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

static void basic_test(MapEntity &e) {

  Game &game = e.get_game();
  Map &map = e.get_map();
  Hero &hero = game.get_hero();

  e.set_top_left_xy(144, 104);
  hero.set_top_left_xy(200, 144);

  PathFinding pf(map, e, hero);
  std::string path = pf.compute_path();

  Debug::check_assertion(path == "7777700",
      StringConcat() << "Unexpected path for 'basic_test': '" << path << "'");
}

/*
 * Test for the path finding algorithm.
 */
int main(int argc, char **argv) {

  Solarus solarus(argc, argv);
  Savegame savegame("save_initial.dat");
  Game game(solarus, savegame);
  game.update();

  Map &map = game.get_current_map();
  MapEntity *e = Enemy::create(game, "tentacle", Enemy::RANK_NORMAL, -1, "e", LAYER_LOW, 160, 117, 0,
      Treasure(game, "_none", 0, -1));
  map.get_entities().add_entity(e);

  basic_test(*e);

  return 0;
}

