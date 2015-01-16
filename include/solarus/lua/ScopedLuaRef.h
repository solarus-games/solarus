/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_SCOPED_LUA_REF_H
#define SOLARUS_SCOPED_LUA_REF_H

#include "solarus/Common.h"
#include <string>

struct lua_State;

namespace Solarus {

/**
 * \brief Holds a Lua ref and does luaL_unref() on destruction.
 *
 * This class is meant to make the usage of Lua refs safer, by making sure
 * that for each luaL_ref() call, there is exactly one luaL_unref() call.
 * This avoids memory leaks and duplicate luaL_unref() calls.
 *
 * It is recommended to use this class rather than calling luaL_unref()
 * directly.
 */
class ScopedLuaRef {

  public:

    ScopedLuaRef();
    ScopedLuaRef(lua_State* l, int ref);
    ScopedLuaRef(const ScopedLuaRef& other);
    ScopedLuaRef(ScopedLuaRef&& other);
    ~ScopedLuaRef();

    ScopedLuaRef& operator=(const ScopedLuaRef& other);
    ScopedLuaRef& operator=(ScopedLuaRef&& other);

    lua_State* get_lua_state() const;
    bool is_empty() const;
    int get() const;
    void clear();

    void push() const;
    void call(const std::string& function_name) const;
    void clear_and_call(const std::string& function_name);

  private:

    lua_State* l;  /**< The Lua state. nullptr means no ref. */
    int ref;       /**< Lua ref to a value. */

};

}

#endif
