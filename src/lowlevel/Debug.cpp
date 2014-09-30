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
#include "solarus/lowlevel/Debug.h"
#include "solarus/SolarusFatal.h"
#include <cstdlib>  // std::abort
#include <fstream>
#include <iostream>
#include <string>
#include <SDL_messagebox.h>

namespace Solarus {
namespace Debug {

namespace {

  bool die_on_error = false;
  bool show_popup_on_die = true;
  bool abort_on_die = false;
  const std::string error_output_file_name = "error.txt";
  std::ofstream error_output_file;

}

/**
 * \brief Sets whether errors are fatal.
 *
 * If \c true, calling to Debug::error() will stop Solarus.
 * The default is \c false.
 */
void set_die_on_error(bool die) {
  die_on_error = die;
}

/**
 * \brief Sets whether a dialog should pop when Solarus dies.
 *
 * The default is \c true.
 */
void set_show_popup_on_die(bool show) {
  show_popup_on_die = show;
}

/**
 * \brief Sets whether the process should abort when Solarus dies.
 *
 * If \c true, std::abort will be called.
 * If \c false, the main loop will stop and a SolarusFatal exception will
 * be thrown.
 * This should be preferred if Solarus is used as a library.
 * The default is \c false.
 */
void set_abort_on_die(bool abort) {
  abort_on_die = abort;
}

/**
 * \brief Prints "Warning: " and a message on both stderr and error.txt.
 * \param message The warning message to print.
 */
void warning(const std::string& message) {

  if (!error_output_file.is_open()) {
    error_output_file.open(error_output_file_name.c_str());
  }
  error_output_file << "Warning: " << message << std::endl;
  std::cerr << "Warning: " << message << std::endl;
}

/**
 * \brief Prints "Error: " and a message on both stderr and error.txt.
 *
 * Use this function for non fatal errors such as errors in quest data files.
 *
 * \param message The error message to print.
 */
void error(const std::string& message) {

  if (die_on_error) {
    // Errors are fatal.
    die(message);
  }

  if (!error_output_file.is_open()) {
    error_output_file.open(error_output_file_name.c_str());
  }
  error_output_file << "Error: " << message << std::endl;
  std::cerr << "Error: " << message << std::endl;

}

/**
 * \brief Like check_assertion(bool, const std::string&), but avoids
 * a useless conversion to std::string when the assertion is true.
 */
void check_assertion(bool assertion, const char* error_message) {

  // Don't build the std::string when the assertion succeeds.
  if (!assertion) {
    die(error_message);
  }
}

/**
 * \brief Aborts the program if the specified assertion is \c false.
 * \param assertion The assertion to check.
 * \param error_message Error message to show in case of failure.
 */
void check_assertion(bool assertion, const std::string& error_message) {

  if (!assertion) {
    die(error_message);
  }
}

/**
 * \brief Aborts the program.
 *
 * This function is equivalent to check_assertion(false, error_message).
 * The error message is printed on both stderr and error.txt.
 *
 * \param error_message The error message to show.
 */
void die(const std::string& error_message) {

  if (!error_output_file.is_open()) {
    error_output_file.open(error_output_file_name.c_str());
  }
  error_output_file << "Fatal: " << error_message << std::endl;
  std::cerr << "Fatal: " << error_message << std::endl;

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
