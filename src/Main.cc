/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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

/**
 * @brief Usual entry point of the program.
 *
 * Usage: solarus [options] [quest_path]
 *
 * The quest path is the name of directory that contains either the data
 * directory or the data archive (data.solarus).
 * If the quest path is not set, the current directory is used.
 *
 * The following options are supported:
 *   -no-audio           disables sounds and musics
 *   -no-video           disables displaying (used for unitary tests)
 *   -language=l         sets the language l (overrides the one saved in the options file)
 *
 * @param argc number of command-line arguments
 * @param argv command-line arguments
 */
int main(int argc, char **argv) {

  Solarus(argc, argv).main_loop();
  return 0;
}

#endif

