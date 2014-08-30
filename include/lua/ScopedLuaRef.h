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
#ifndef SOLARUS_SCOPED_LUA_REF_H
#define SOLARUS_SCOPED_LUA_REF_H

#include "Common.h"

namespace solarus {

/**
 * \brief Holds a Lua ref and does luaL_unref() on destruction.
 *
 * This class is meant to make the usage of Lua refs safer, by making sure
 * that for each luaL_ref() call, there is exaclty one luaL_unref() call.
 * This avoids memory leaks and duplicate luaL_unref() calls.
 *
 * It is recommended to use this class instead of calling luaL_unref()
 * directly.
 */
class ScopedLuaRef {

  public:

    explicit ScopedLuaRef(LuaContext& lua_context);
    ScopedLuaRef(LuaContext& lua_context, int ref);
    explicit ScopedLuaRef(ScopedLuaRef& other);
    explicit ScopedLuaRef(ScopedLuaRef&& other);
    ~ScopedLuaRef();

    ScopedLuaRef& operator=(const ScopedLuaRef& other);
    ScopedLuaRef& operator=(ScopedLuaRef&& other);

    bool is_empty() const;
    int get() const;
    void set(int ref);
    void clear();

  private:

    LuaContext& lua_context;      /**< The Lua state. */
    int ref;                      /**< Lua ref to a value. */

};

}

#endif
