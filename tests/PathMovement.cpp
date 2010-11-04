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
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

static void basic_test() {

  PathMovement m("0", 100, false, false, false); // 8 pixels to the right

  /* PathMovement does not work without entity
  while (!m.is_finished()) {

    m.update();
    System::update();
  }
  
  Debug::assert(m.get_x() == 8 && m.get_y() == 0,
      StringConcat() << "Unexcepted coordinates for 'basic_test': " << m.get_xy());
  */
}

/*
 * Test for the path movement.
 */
int main(int argc, char **argv) {

  Solarus solarus(argc, argv);

  basic_test();

  return 0;
}

