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
#include "solarus/core/Debug.h"
#include "solarus/core/Logger.h"
#include "solarus/core/SolarusFatal.h"
#include <cstdlib>  // std::abort
#include <SDL_messagebox.h>

namespace Solarus {
namespace Debug {

namespace {

  bool die_on_error = false;
  bool show_popup_on_die = true;
  bool abort_on_die = false;

}

/**
 * \brief Sets whether errors are fatal.
 * \param die If \c true, calling Debug::error() will stop Solarus.
 * The default is \c false.
 */
SOLARUS_API void set_die_on_error(bool die) {
  die_on_error = die;
}

/**
 * \brief Sets whether a dialog should pop when Solarus dies.
 * \param show Whether a dialog should pop when Solarus dies.
 * The default is \c true.
 */
SOLARUS_API void set_show_popup_on_die(bool show) {
  show_popup_on_die = show;
}

/**
 * \brief Sets whether the process should abort when Solarus dies.
 *
 * \param abort Whether std::abort should be called.
 * If \c false, the main loop will stop and a SolarusFatal exception will
 * be thrown.
 * This should be preferred if Solarus is used as a library.
 * The default is \c false.
 */
SOLARUS_API void set_abort_on_die(bool abort) {
  abort_on_die = abort;
}

/**
 * \brief Prints "Warning: " and a message on both stdout and error.txt.
 * \param message The warning message to print.
 */
SOLARUS_API void warning(const std::string& message) {

  Logger::warning(message);
}

/**
 * \brief Prints "Error: " and a message on both stdout and error.txt.
 *
 * Use this function for non fatal errors such as errors in quest data files.
 * Stops Solarus if set_die_on_error(true) was called.
 *
 * \param message The error message to print.
 */
SOLARUS_API void error(const std::string& message) {

  if (die_on_error) {
    // Errors are fatal.
    die(message);
  }

  Logger::error(message);
}

/**
 * \brief Like check_assertion(bool, const std::string&), but avoids
 * a useless conversion to std::string when the assertion is true.
 */
void SOLARUS_API check_assertion(bool assertion, const char* error_message) {

  // Don't build the std::string when the assertion succeeds.
  if (!assertion) {
    die(error_message);
  }
}

/**
 * \brief Stops Solarus if the specified assertion is \c false.
 * \param assertion The assertion to check.
 * \param error_message Error message to show in case of failure.
 */
void SOLARUS_API check_assertion(bool assertion, const std::string& error_message) {

  if (!assertion) {
    die(error_message);
  }
}

/**
 * \brief Stops Solarus on a fatal error.
 *
 * This function is equivalent to check_assertion(false, error_message).
 * The error message is printed on both stdout and error.txt.
 *
 * \param error_message The error message to report.
 */
void SOLARUS_API die(const std::string& error_message) {

  Logger::fatal(error_message);

  if (show_popup_on_die) {
    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR,
        "Error",
        error_message.c_str(),
        nullptr
    );
  }

  if (abort_on_die) {
    std::abort();
  }
  else {
    throw SolarusFatal(error_message);
  }
}

}
}
