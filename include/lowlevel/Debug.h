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
#ifndef SOLARUS_DEBUG_H
#define SOLARUS_DEBUG_H

#include "Common.h"
#include <stdexcept>
#include <iostream>

/**
 * @brief Provides functionalities for printing debug messages
 * or making runtime verifications, especially when the code is compiled in debugging mode.
 */
class Debug {

  private:

    Debug();	// don't instanciate this class

  public:

    static void print(const std::string &message, std::ostream &os = std::cout);
    static void check_assertion(bool assertion, const std::string &error_message = "");
    static void die(const std::string &error_message = "");
};


/**
 * @brief Prints a message if the program is in debug mode.
 *
 * This function does nothing if SOLARUS_DEBUG_LEVEL is 0.
 *
 * @param message the message to print.
 * @param os the output stream to write (default is std::cout)
 */
inline void Debug::print(const std::string &message, std::ostream &os) {

#if SOLARUS_DEBUG_LEVEL > 0
  os << message << std::endl;
#endif
}

/**
 * @brief Throws an exception if the specified assertion fails.
 *
 * If the assertion fails, an std::logic_error with the specified error message is thrown.
 * If SOLARUS_DEBUG_LEVEL is greater than 0, the error message is
 * printed on stderr first.
 * This function should be used to detect fatal errors only, that is,
 * errors in your code or in the quest (the data files) that require to stop the program.
 *
 * @param assertion the boolean condition to check
 * @param error_message the error message to attach to the exception when the assertion fails
 */
inline void Debug::check_assertion(bool assertion, const std::string &error_message) {

  if (!assertion) {
    print(error_message, std::cerr);
    die(error_message);
  }
}

/**
 * @brief Throws an exception to stop the program.
 *
 * This function is equivalent to assert(false, error_message).
 *
 * @param error_message the error message to attach to the exception
 */
inline void Debug::die(const std::string &error_message) {

  throw std::logic_error(error_message);
}

#endif

