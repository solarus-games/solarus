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
#ifndef SOLARUS_LUA_EXCEPTION_H
#define SOLARUS_LUA_EXCEPTION_H

#include "solarus/core/Common.h"
#include <exception>
#include <string>

struct lua_State;

namespace Solarus {

/**
 * \brief Exception to indicate that a Lua error occurred.
 *
 * Depending on how Lua was compiled and on whether we are using regular Lua
 * or LuaJIT, lua_error() may be implemented with longjmp()/setjmp() or with
 * exceptions. We don't want to assume anything about that.
 * So there is a problem when lua_error() is called: it is possible that C++
 * destructors are never called, resulting in memory leaks.
 * To fix this, in the C++ side, we use exceptions instead of calling
 * lua_error(). Stack-allocated objects are then correctly destroyed.
 *
 * C++/Lua boundary functions convert exceptions to Lua errors to make
 * everything correct.
 * - Any C++ function that calls Lua only makes protected calls.
 * - Any C++ function directly called by Lua catches LuaExceptions and makes a
 *   lua_error() if a LuaException occurred.
 * This way, the C++ side only uses exceptions and the Lua side only uses
 * Lua errors. The stack is correctly unwound (destructors are called),
 * including when it contains several C++/Lua and Lua/C++ calls.
 */
class LuaException: public std::exception {

  public:

    LuaException(
        lua_State* l,
        const std::string& error_message
    );

    lua_State* get_lua_state() const;
    virtual const char* what() const throw() override;

  private:

    lua_State* l;                      /**< The Lua state. */
    std::string error_message;         /**< Message of the Lua error. */

};

}

#endif

