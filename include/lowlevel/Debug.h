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

namespace solarus {

/**
 * \brief Provides features for printing error messages or making runtime checks.
 */
class Debug {

  public:

    static void warning(const std::string& message);
    static void error(const std::string& message);
    static void check_assertion(bool assertion, const char* error_message);
    static void check_assertion(bool assertion, const std::string& error_message);
    static void die(const std::string& error_message);

  private:

    Debug();    // don't instantiate this class
};

}

#endif

