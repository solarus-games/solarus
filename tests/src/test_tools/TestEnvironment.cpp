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
#include "solarus/entities/MapEntities.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/System.h"
#include "solarus/Map.h"
#include "solarus/Game.h"
#include "solarus/Savegame.h"
#include "test_tools/TestEnvironment.h"

namespace Solarus {

/**
 * \brief Creates a test environment.
 */
TestEnvironment::TestEnvironment(int argc, char** argv):
    arguments(argc, argv),
    main_loop(arguments),
    map_id("traversable") {

  Debug::set_show_popup_on_die(false);
  Debug::set_die_on_error(true);
  Debug::set_abort_on_die(true);
}

/**
 * \brief Returns the runtime arguments that were passed to the test.
 * \return The runtime arguments.
 */
const Arguments& TestEnvironment::get_arguments() const {
  return arguments;
}

/**
 * \brief Returns the main loop.
 */
MainLoop& TestEnvironment::get_main_loop() {
  return main_loop;
}

/**
 * \brief Returns the game, creating it if necessary.
 */
Game& TestEnvironment::get_game() {

  if (main_loop.get_game() == nullptr) {
    std::shared_ptr<Savegame> savegame = std::make_shared<Savegame>(
        main_loop, "save_initial.dat"
    );
    savegame->set_string(Savegame::KEY_STARTING_MAP, map_id);
    Game* game = new Game(main_loop, savegame);
    main_loop.set_game(game);
    step();  // Advance one tick to start the game.
  }

  Debug::check_assertion(main_loop.get_game() != nullptr, "Missing game");

  return *main_loop.get_game();
}

/**
 * \brief Returns the current map, creating it if necessary.
 */
Map& TestEnvironment::get_map() {

  if (!get_game().has_current_map()) {
    step();  // Advance one tick to start the map.
    Debug::check_assertion(get_game().has_current_map(), "Missing map");
  }

  return get_game().get_current_map();
}

/**
 * \brief Returns the entity manager of the current map.
 */
MapEntities& TestEnvironment::get_entities() {

  return get_map().get_entities();
}

/**
 * \brief Returns the hero of the game.
 */
Hero& TestEnvironment::get_hero() {

  return *get_game().get_hero();
}

/**
 * \brief Runs the main loop on the specified map.
 * \param map_id Id of the map to open.
 */
void TestEnvironment::run_map(const std::string& map_id) {

  this->map_id = map_id;
  get_game();  // Create a game and start it on the map.
  get_main_loop().run();
}

/**
 * \brief Creates a custom entity on the map and returns it.
 * \param xy Coordinates of the entity to create.
 * \param layer Layer of the entity to create.
 */
template<>
std::shared_ptr<CustomEntity> TestEnvironment::make_entity<CustomEntity>(
    const Point& xy,
    Layer layer
) {

  std::shared_ptr<CustomEntity> entity = std::make_shared<CustomEntity>(
     get_game(),
     "",
     0,
     layer,
     xy,
     Size(16, 16),
     "",
     ""
  );
  get_entities().add_entity(entity);

  return entity;
}

/**
 * \brief Returns the current simulated time.
 * \return The simulated time in milliseconds.
 */
uint32_t TestEnvironment::now() {
  return System::now();
}

/**
 * \brief Simulates one tick of the main loop.
 */
void TestEnvironment::step() {
  get_main_loop().step();
}

}

