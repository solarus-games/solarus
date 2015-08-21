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
#include "solarus/containers/Quadtree.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Rectangle.h"
#include "test_tools/TestEnvironment.h"
#include <memory>

using namespace Solarus;

namespace {

class Element {

  public:

    Element() :
        rectangle() {
    }

    explicit Element(const Rectangle& rectangle) :
        rectangle(rectangle) {
    }

    const Rectangle& get_bounding_box() const {
      return rectangle;
    }

    Rectangle& get_bounding_box() {
      return rectangle;
    }

  private:

    Rectangle rectangle;
};

using ElementPtr = std::shared_ptr<Element>;

/**
 * \brief Tests an empty quadtree.
 */
void test_empty(TestEnvironment& /* env */) {

  Size space_size(1280, 960);

  Quadtree<ElementPtr> quadtree(space_size);

  Debug::check_assertion(quadtree.get_space_size() == space_size, "Wrong space size");
  Debug::check_assertion(quadtree.get_num_elements() == 0, "Expected empty quadtree");
}

}

/**
 * Tests for the path movement.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  test_empty(env);

  return 0;
}

