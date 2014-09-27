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
#include "CommandLine.h"
#include <algorithm>

namespace Solarus {

/**
 * \brief Creates a command-line object from standard main() arguments.
 * \param argc Number of command-line arguments.
 * \param argv Command-line arguments. The first one, if any, is the name
 * of the program.
 */
CommandLine::CommandLine(int argc, char** argv) {

  if (argc >= 1) {
    program_name = argv[0];
  }
  for (int i = 1; i < argc; ++i) {
    if (argv[i] != NULL) {
      args.push_back(argv[i]);
    }
  }
}

/**
 * \brief Returns the program name that was passed as first argument of main().
 * \return The program name, or an empty string if unknown.
 */
const std::string& CommandLine::get_program_name() const {
  return program_name;
}

/**
 * \brief Returns the list of arguments passed to main, except the program name.
 * \return The list of arguments.
 */
const std::vector<std::string>& CommandLine::get_arguments() const {
  return args;
}

/**
 * \brief Returns whether the following exact string was passed as an argument.
 * \param argument The string to test.
 * \return \c true if an argument is exactly this string.
 */
bool CommandLine::has_argument(const std::string& argument) const {

  return std::find(args.begin(), args.end(), argument) != args.end();
}

/**
 * \brief If there is an argument of the form \c key=value, returns the value.
 * \param key The key to look for.
 * \return The value that was passed for this key, or an empty string if
 * no such argument was passed.
 */
std::string CommandLine::get_argument_value(const std::string& key) const {

  for (const std::string& arg: args) {
    if (arg.size() > key.size()
        && arg.substr(0, key.size() + 1) == key + '=') {
      return arg.substr(key.size() + 1);
    }
  }

  return "";
}

}

