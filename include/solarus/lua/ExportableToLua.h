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
#ifndef SOLARUS_EXPORTABLE_TO_LUA_H
#define SOLARUS_EXPORTABLE_TO_LUA_H

#include "solarus/Common.h"
#include <memory>
#include <string>

namespace Solarus {

class LuaContext;

/**
 * \brief Interface of a C++ type that can also exist as a Lua userdata.
 *
 * Use std::shared_ptr to safely share the object between C++ and Lua,
 * no matter which world stops using it first.
 */
class ExportableToLua:
    public std::enable_shared_from_this<ExportableToLua> {

  public:

    ExportableToLua();
    virtual ~ExportableToLua();

    LuaContext* get_lua_context() const;
    void set_lua_context(LuaContext* lua_context);
    bool is_known_to_lua() const;
    void set_known_to_lua(bool known_to_lua);
    bool is_with_lua_table() const;
    void set_with_lua_table(bool with_lua_table);

    /**
     * \brief Returns the name identifying this type in Lua.
     * \return the name identifying this type in Lua
     */
    virtual const std::string& get_lua_type_name() const = 0;

  private:

    LuaContext* lua_context;     /**< The Solarus Lua API, or nullptr if
                                  * not currently exported to Lua. */

    bool known_to_lua;           /**< Whether this object was exported to Lua
                                  * at least once. */
    bool with_lua_table;         /**< Whether a Lua table was created to make
                                  * this userdata indexable like a table. */

};

}

#endif

