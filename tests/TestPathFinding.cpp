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
#include "entities/MapEntities.h"
#include "entities/CustomEntity.h"
#include "entities/Hero.h"
#include "lowlevel/Debug.h"
#include "movements/PathFinding.h"
#include "CommandLine.h"
#include "MainLoop.h"
#include "Game.h"
#include "Map.h"
#include "Savegame.h"

using namespace solarus;

namespace {

void basic_test(MapEntity& entity) {

  Game& game = entity.get_game();
  Map& map = entity.get_map();
  Hero& hero = *game.get_hero();

  entity.set_top_left_xy(144, 104);
  hero.set_top_left_xy(200, 144);

  PathFinding pf(map, entity, hero);
  std::string path = pf.compute_path();

  debug::check_assertion(path == "7777700", "Unexpected path");
}

}

/**
 * \brief Test for the path finding algorithm.
 */
int main(int argc, char** argv) {

  const CommandLine args(argc, argv);
  MainLoop main_loop(args);

  std::shared_ptr<Savegame> savegame = std::make_shared<Savegame>(main_loop, "save_initial.dat");
  Game game(main_loop, savegame);
  game.update();

  Map& map = game.get_current_map();
  MapEntityPtr entity = std::make_shared<CustomEntity>(
      game,
      "",
      0,
      LAYER_LOW,
      0,
      0,
      16,
      16,
      "",
      ""
  );

  map.get_entities().add_entity(entity);

  basic_test(*entity);

  return 0;
}

