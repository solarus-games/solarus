/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_DEBUG_H
#define SOLARUS_DEBUG_H

#include "Common.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#ifndef NDEBUG
#define SOLARUS_ASSERT(condition, message) Debug::check_assertion(condition, message)
#else
#define SOLARUS_ASSERT(condition, message)
#endif

/**
 * \brief Provides functionalities for printing error messages or making
 * runtime checks, especially when the code is compiled in debugging mode.
 */
class Debug {

  public:

    static void warning(const std::string& message);
    static void error(const std::string& message);
    static void check_assertion(bool assertion, const std::string& error_message = "");
    static void die(const std::string& error_message);

  private:

    Debug();    // don't instantiate this class
};

/**
 * \brief Prints "Warning: " and a message on stderr.
 * \param message The warning message to print.
 */
inline void Debug::warning(const std::string& message) {

  std::cerr << "Warning: " << message << std::endl;
}

/**
 * \brief Prints "Error: " and a message on stderr.
 *
 * Use this function for non fatal errors such as errors in quest data files.
 *
 * \param message The error message to print.
 */
inline void Debug::error(const std::string& message) {

  std::cerr << "Error: " << message << std::endl;
}

/**
 * \brief Throws an exception if the specified assertion fails.
 *
 * If the assertion fails, shows an error message and aborts.
 * The error message is saved in error.txt.
 * This function should be used to detect fatal errors only, that is,
 * internal errors in the C++ code that require to stop the program.
 *
 * \param assertion The boolean condition to check.
 * \param error_message The error message to attach to print when the
 * assertion fails.
 */
inline void Debug::check_assertion(bool assertion, const std::string& error_message) {

  if (!assertion) {
    die(error_message);
  }
}

/**
 * \brief Aborts the program.
 *
 * This function is equivalent to Debug::check_assertion(false, error_message).
 * The error message is printed on stdout and also saved in error.txt.
 *
 * \param error_message The error message to show.
 */
inline void Debug::die(const std::string& error_message) {

  std::cerr << "Fatal: " << error_message << std::endl;
  std::ofstream out("error.txt");
  out << error_message << std::endl << std::flush;
  std::abort();
}

#endif

