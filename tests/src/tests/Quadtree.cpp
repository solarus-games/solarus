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
#include "solarus/containers/Quadtree.h"
#include "solarus/core/Debug.h"
#include "solarus/core/Rectangle.h"
#include "test_tools/TestEnvironment.h"
#include <memory>
#include <sstream>

using namespace Solarus;

using Box = Solarus::Rectangle;

namespace {

class Element {

  public:

    Element() :
        rectangle() {
    }

    explicit Element(const Box& rectangle) :
        rectangle(rectangle) {
    }

    const Box& get_bounding_box() const {
      return rectangle;
    }

    Box& get_bounding_box() {
      return rectangle;
    }

  private:

    Box rectangle;
};

using ElementPtr = std::shared_ptr<Element>;

/**
 * \brief Checks the number of elements of a quadtree.
 */
void check_num_elements(const Quadtree<ElementPtr>& quadtree, int expected) {

  if (quadtree.get_num_elements() != expected) {
    std::ostringstream oss;
    oss << "Wrong number of elements: expected " << expected << ", got " << quadtree.get_num_elements();
    Debug::die(oss.str());
  }
}

/**
 * \brief Checks that an element was found after a quadtree query.
 */
void check_found(const std::vector<ElementPtr>& found_elements, ElementPtr& expected) {

  Debug::check_assertion(
      std::find(found_elements.begin(), found_elements.end(), expected) != found_elements.end(),
      "Element not found"
  );
}

/**
 * \brief Creates an element with the given coordinates and adds it to a quatree.
 */
ElementPtr add(Quadtree<ElementPtr>& quadtree, const Box& bounding_box) {

  int num_elements = quadtree.get_num_elements();
  ElementPtr element(std::make_shared<Element>(bounding_box));
  Debug::check_assertion(quadtree.add(element, element->get_bounding_box()), "Failed to add element");
  check_num_elements(quadtree, num_elements + 1);
  return element;
}

/**
 * \brief Removes an element from a quatree.
 */
void remove(Quadtree<ElementPtr>& quadtree, const ElementPtr& element) {

  int num_elements = quadtree.get_num_elements();
  Debug::check_assertion(quadtree.remove(element), "Failed to remove element");
  check_num_elements(quadtree, num_elements - 1);
}

/**
 * \brief Moves an element to its new coordinates in a quatree.
 */
void move(Quadtree<ElementPtr>& quadtree, const ElementPtr& element) {

  Debug::check_assertion(quadtree.contains(element), "Element not in quadtree");

  int num_elements = quadtree.get_num_elements();
  Debug::check_assertion(quadtree.move(element, element->get_bounding_box()), "Failed to move element");

  // The number of elements should be unchanged.
  check_num_elements(quadtree, num_elements);
}

/**
 * \brief Tests an empty quadtree.
 */
void test_empty(TestEnvironment& /* env */, Quadtree<ElementPtr>& quadtree) {

  check_num_elements(quadtree, 0);
}

/**
 * \brief Tests adding elements to a quadtree.
 */
void test_add(TestEnvironment& /* env */, Quadtree<ElementPtr>& quadtree) {

  const std::vector<Box> rectangles = {
      Box(100, 40, 16, 16),
      Box(200, 10, 16, 16),
      Box(250, 20, 16, 16),
      Box(300, 30, 16, 16),
      Box(300, 50, 16, 16),
      Box(800, 40, 16, 16),
      Box(500, 60, 16, 16),
      Box(600, 100, 16, 16),
      Box(400, 300, 16, 16),
      Box(700, 400, 16, 16)
  };

  std::vector<ElementPtr> added_elements;
  for (const Box& rectangle : rectangles) {
    ElementPtr element = add(quadtree, rectangle);
    added_elements.push_back(element);
  }

  Box region(220, 10, 100, 100);
  std::vector<ElementPtr> found_elements = quadtree.get_elements(region);

  Debug::check_assertion(found_elements.size() == 3, "Expected 3 elements found");
  check_found(found_elements, added_elements[2]);
  check_found(found_elements, added_elements[3]);
  check_found(found_elements, added_elements[4]);
}

/**
 * \brief Tests removing elements from a quadtree.
 */
void test_remove(TestEnvironment& /* env */, Quadtree<ElementPtr>& quadtree) {

  // Get all elements.
  std::vector<ElementPtr> elements = quadtree.get_elements(quadtree.get_space());

  // Remove some of them.
  Debug::check_assertion(quadtree.get_num_elements() > 5, "Wrong number of elements");
  remove(quadtree, elements[0]);
}

/**
 * \brief Tests adding elements whose size overlaps several cells.
 */
void test_add_big_size(TestEnvironment& /* env */, Quadtree<ElementPtr>& quadtree) {

  add(quadtree, Box(25, 25, 600, 600));
  add(quadtree, Box(100, 0, 16, 960));
}

/**
 * \brief Tests adding elements near the limit or outside the quadtree space.
 */
void test_add_limit(TestEnvironment& /* env */, Quadtree<ElementPtr>& quadtree) {

  add(quadtree, Box(-16, 0, 16, 960));

  // Try to add an element outside the quadtree space.
  add(quadtree, Box(-160, 0, 16, 960));
}

/**
 * \brief Tests adding an element partially outside the quadtree space, with the center outside.
 */
void test_add_center_outside(TestEnvironment& /* env */, Quadtree<ElementPtr>& quadtree) {

  add(quadtree, Box(-480, 32, 640, 640));
  add(quadtree, Box(1300, 32, 640, 640));
}

/**
 * \brief Tests moving elements in a quadtree.
 */
void test_move(TestEnvironment& /* env */, Quadtree<ElementPtr>& quadtree) {

  // Get all elements.
  std::vector<ElementPtr> elements = quadtree.get_elements(quadtree.get_space());

  // Move some of them.
  Debug::check_assertion(quadtree.get_num_elements() > 4, "Wrong number of elements");
  ElementPtr element = elements[0];
  element->get_bounding_box().set_xy(128, 256);
  move(quadtree, element);
}

/**
 * \brief Tests moving elements to outside and from outside a quadtree.
 */
void test_move_limit(TestEnvironment& /* env */, Quadtree<ElementPtr>& quadtree) {

  // Get all elements.
  std::vector<ElementPtr> elements = quadtree.get_elements(quadtree.get_space());

  // Move an element outside the bounds.
  int num_elements = quadtree.get_num_elements();
  Debug::check_assertion(num_elements > 4, "Wrong number of elements");

  ElementPtr element = elements[0];
  element->get_bounding_box().set_xy(-50000, -50000);
  move(quadtree, element);
  Debug::check_assertion(quadtree.get_num_elements() == num_elements, "Wrong number of elements");

  // Come back in the bounds.
  element->get_bounding_box().set_xy(42, 64);
  move(quadtree, element);
  Debug::check_assertion(quadtree.get_num_elements() == num_elements, "Wrong number of elements");
}

}

/**
 * Tests for the path movement.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  int margin = 64;
  Box space(-margin, -margin, 1280 + 2 * margin, 960 + 2 * margin);
  Quadtree<ElementPtr> quadtree(space);

  test_empty(env, quadtree);
  test_add(env, quadtree);
  test_add_big_size(env, quadtree);
  test_add_limit(env, quadtree);
  test_add_center_outside(env, quadtree);
  test_remove(env, quadtree);
  test_move(env, quadtree);
  test_move_limit(env, quadtree);

  return 0;
}

