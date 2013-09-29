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
#include "lowlevel/Debug.h"
#include <SDL_messagebox.h>

namespace {

  const std::string error_output_file_name = "error.txt";
  std::ofstream error_output_file;
}

/**
 * \brief Prints "Warning: " and a message on both stderr and error.txt.
 * \param message The warning message to print.
 */
void Debug::warning(const std::string& message) {

  if (!error_output_file.is_open()) {
    error_output_file.open(error_output_file_name.c_str());
  }
  error_output_file << "Warning: " << message << std::endl << std::flush;
  std::cerr << "Warning: " << message << std::endl;
}

/**
 * \brief Prints "Error: " and a message on both stderr and error.txt.
 *
 * Use this function for non fatal errors such as errors in quest data files.
 *
 * \param message The error message to print.
 */
void Debug::error(const std::string& message) {

  if (!error_output_file.is_open()) {
    error_output_file.open(error_output_file_name.c_str());
  }
  error_output_file << "Error: " << message << std::endl << std::flush;
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
void Debug::check_assertion(bool assertion, const std::string& error_message) {

  if (!assertion) {
    die(error_message);
  }
}

/**
 * \brief Aborts the program.
 *
 * This function is equivalent to Debug::check_assertion(false, error_message).
 * The error message is printed on both stderr and error.txt.
 *
 * \param error_message The error message to show.
 */
void Debug::die(const std::string& error_message) {

  if (!error_output_file.is_open()) {
    error_output_file.open(error_output_file_name.c_str());
  }
  error_output_file << "Fatal: " << error_message << std::endl << std::flush;
  std::cerr << "Fatal: " << error_message << std::endl;

  SDL_ShowSimpleMessageBox(
      SDL_MESSAGEBOX_ERROR,
      "Error",
      error_message.c_str(),
      NULL);

  std::exit(EXIT_FAILURE);
}

