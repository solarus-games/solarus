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
#ifndef SOLARUS_NOMAIN

#include "Solarus.h"
#include <iostream>
#include <fstream>

/**
 * @brief Usual entry point of the program.
 *
 * The following optional command-line arguments are supported:
 *   -datapath=/some/path/to/datadir            loads the data files from the specified directory
 *   -no-audio                                  disables sounds and musics
 *   -no-video                                  disables displaying (used for unitary tests)
 *   -language=l                                sets the language l
 *
 * @param argc number of command-line arguments
 * @param argv command-line arguments
 */
int main(int argc, char **argv) {

  Solarus(argc, argv).main_loop();
  return 0;
}

#endif

