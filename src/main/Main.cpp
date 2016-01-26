/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Output.h"
#include "solarus/Arguments.h"
#include "solarus/MainLoop.h"
#include <iostream>
#include <string>

namespace Solarus {

namespace {

/**
 * \brief Prints the usage of the program.
 * \param argc number of command-line arguments
 * \param argv command-line arguments
 */
void print_help(const Arguments& args) {

  Output::initialize(args);
  std::string binary_name = args.get_program_name();
  if (binary_name.empty()) {
    binary_name = "solarus";
  }
  std::cout << "Usage: " << binary_name << " [options] [quest_path]"
    << std::endl << std::endl
    << "The quest path is the name of a directory that contains either the data"
    << std::endl
    << "directory or the data archive (data.solarus or data.solarus.zip) of the game to run."
    << std::endl
    << "If the quest path is not specified, the default directory will be: '"
    << SOLARUS_DEFAULT_QUEST << "'."
    << std::endl
    << std::endl
    << "Options:"
    << std::endl
    << "  -help                         shows this help message and exits"
    << std::endl
    << "  -no-audio                     disables sounds and musics"
    << std::endl
    << "  -no-video                     disables displaying"
    << std::endl
    << "  -video-acceleration=yes|no    enables or disables accelerated graphics (default yes)"
    << std::endl
    << "  -quest-size=<width>x<height>  sets the size of the drawing area (if compatible with the quest)"
    << std::endl
    << "  -lua-console=yes|no           accepts standard input lines as Lua commands (default yes)"
    << std::endl
    << "  -win-console=yes|no           allows to see output in a console, only needed on Windows (default no)"
    << std::endl
    << "  -lag=X                        slows down each frame of X milliseconds to simulate slower systems for debugging (default 0)"
    << std::endl;
}

}  // Anonymous namespace.

}  // namespace Solarus.

/**
 * \brief Usual entry point of the program.
 *
 * Usage: solarus [options] [quest_path]
 *
 * The quest path is the name of a directory that contains either the data
 * directory ("data") or the data archive ("data.solarus" or
 * "data.solarus.zip").
 * If the quest path is not specified, it is set to the preprocessor constant
 * DEFAULT_QUEST, which is the current directory "." by default.
 * In all cases, this quest path is relative to the working directory,
 * or to the solarus executable directory if no quest is found in the working
 * directory.
 *
 * The following options are supported:
 *   -help                             Shows a help message.
 *   -no-audio                         Disables sounds and musics.
 *   -no-video                         Disables displaying (used for unit tests).
 *   -video-acceleration=yes|no        Enables or disables 2D accelerated graphics if available (default: yes).
 *   -quest-size=<width>x<height>      Sets the size of the drawing area (if compatible with the quest).
 *   -lua-console=yes|no               Accepts lines from standard input as Lua commands (default: yes).
 *   -win-console=yes|no               Opens a console to see debug output (default: no).
 *                                     Windows only (other systems use their existing console if any).
 *   -lag=X                            (Advanced) Artificially slows down each frame of X milliseconds
 *                                     to simulate slower systems for debugging (default: 0).
 *
 * \param argc Number of command-line arguments.
 * \param argv Command-line arguments.
 */
int main(int argc, char** argv) {

  using namespace Solarus;

  Debug::set_abort_on_die(true);  // Better for debugging (get a callstack).

  // Store the command-line arguments.
  const Arguments args(argc, argv);

  // Check the -help option.
  if (args.has_argument("-help")) {
    // Print a help message.
    print_help(args);
  }
  else {
    // Run the main loop.
    MainLoop(args).run();
  }

  return 0;
}

#endif

