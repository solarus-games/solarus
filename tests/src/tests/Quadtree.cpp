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
#include <sstream>

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

void check_num_elements(const Quadtree<ElementPtr>& quadtree, int expected) {

  if (quadtree.get_num_elements() != expected) {
    std::ostringstream oss;
    oss << "Wrong number of elements: expected " << expected << ", got " << quadtree.get_num_elements();
    Debug::die(oss.str());
  }
}

ElementPtr add(Quadtree<ElementPtr>& quadtree, int x, int y, int width, int height) {

  ElementPtr element(std::make_shared<Element>(Rectangle(x, y, width, height)));
  Debug::check_assertion(quadtree.add(element), "Failed to add element");
  return element;
}

/**
 * \brief Tests an empty quadtree.
 */
void test_empty(TestEnvironment& /* env */) {

  Size space_size(1280, 960);

  Quadtree<ElementPtr> quadtree(space_size);

  Debug::check_assertion(quadtree.get_space_size() == space_size, "Wrong space size");
  check_num_elements(quadtree, 0);
}

/**
 * \brief Tests adding elements to a quadtree.
 */
void test_add(TestEnvironment& /* env */) {

  Size space_size(1280, 960);

  Quadtree<ElementPtr> quadtree(space_size);

  add(quadtree, 800, 40, 16, 16);
  check_num_elements(quadtree, 1);
}

}

/**
 * Tests for the path movement.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  test_empty(env);
  test_add(env);

  return 0;
}

