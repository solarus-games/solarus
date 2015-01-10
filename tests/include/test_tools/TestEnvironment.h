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
#ifndef SOLARUS_TEST_ENVIRONMENT_H
#define SOLARUS_TEST_ENVIRONMENT_H

#include "solarus/Common.h"
#include "solarus/entities/Layer.h"
#include "solarus/Arguments.h"
#include "solarus/MainLoop.h"
#include <cstdint>
#include <memory>

namespace Solarus {

class Hero;
class Map;
class MapEntities;

/**
 * \brief Wraps a Solarus main loop and provides features to easily make tests.
 *
 * Contrary to the traditional main loop, the test environment does not
 * automatically advance simulated time.
 * The simulation is paused at a specific cycle and will only advance if you
 * decide so.
 *
 * If whatever you want to test can be tested in the same cycle, you don't
 * need to simulate more cycles.
 * If you want to test timed objects like timers or movements,
 * you can make the simulation advance by manual steps.
 * This will also be much faster than real time.
 */
class TestEnvironment {

  public:

    TestEnvironment(int argc, char** argv);

    // Get important objects.
    const Arguments& get_arguments() const;
    MainLoop& get_main_loop();
    Game& get_game();
    Map& get_map();
    MapEntities& get_entities();
    Hero& get_hero();

    void run_map(const std::string& map_id);

    // Creating entities.
    template<typename T>
    std::shared_ptr<T> make_entity(
        const Point& xy = Point(0, 0),
        Layer layer = LAYER_LOW
    );

    // Simulation.
    uint32_t now();
    void step();

  private:

    Arguments arguments;
    MainLoop main_loop;
    std::string map_id;
};

}

#include "test_tools/TestEnvironment.inl"

#endif

