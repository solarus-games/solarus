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
#include "solarus/Arguments.h"
#include <algorithm>

namespace Solarus {

/**
 * \brief Creates an empty argument object.
 */
Arguments::Arguments() {
}

/**
 * \brief Creates an argument object from standard main() arguments.
 * \param argc Number of command-line arguments.
 * \param argv Command-line arguments. The first one, if any, is the name
 * of the program.
 */
Arguments::Arguments(int argc, char** argv) {

  if (argc >= 1) {
    program_name = argv[0];
  }
  for (int i = 1; i < argc; ++i) {
    if (argv[i] != nullptr) {
      args.push_back(argv[i]);
    }
  }
}

/**
 * \brief Returns the program name, usually the first argument of main().
 * \return The program name, or an empty string if unknown.
 */
const std::string& Arguments::get_program_name() const {
  return program_name;
}

/**
 * \brief Sets the program name of this argument list.
 * \param program_name The program name.
 */
void Arguments::set_program_name(const std::string& program_name) {
  this->program_name = program_name;
}

/**
 * \brief Returns the list of arguments passed to main, except the program name.
 * \return The list of arguments.
 */
const std::vector<std::string>& Arguments::get_arguments() const {
  return args;
}

/**
 * \brief Returns whether the following exact string was passed as an argument.
 * \param argument The string to test.
 * \return \c true if an argument is exactly this string.
 */
bool Arguments::has_argument(const std::string& argument) const {

  return std::find(args.begin(), args.end(), argument) != args.end();
}

/**
 * \brief If there is an argument of the form \c key=value, returns the value.
 * \param key The key to look for.
 * \return The value that was passed for this key, or an empty string if
 * no such argument was passed.
 */
std::string Arguments::get_argument_value(const std::string& key) const {

  for (const std::string& arg: args) {
    if (arg.size() > key.size()
        && arg.substr(0, key.size() + 1) == key + '=') {
      return arg.substr(key.size() + 1);
    }
  }

  return "";
}

/**
 * \brief Adds an argument.
 * \param argument The string to add.
 */
void Arguments::add_argument(const std::string& argument) {

  args.push_back(argument);
}

/**
 * \brief Adds an argument as a key and value.
 *
 * This is equivalent to <tt>add_argument(key + "=" + value)</tt>.
 *
 * \param key Key of the argument to add.
 * \param value Value of the argument to add.
 */
void Arguments::add_argument(const std::string& key, const std::string& value) {

  add_argument(key + "=" + value);
}

}

