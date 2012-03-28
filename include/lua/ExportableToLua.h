/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-engine.org
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

#include "Common.h"

/**
 * @brief Interface of a C++ type that can also exist as a Lua userdata.
 */
class ExportableToLua {

  public:

    ExportableToLua();
    virtual ~ExportableToLua();

    // creator script
    Script* get_creator_script() const;
    void set_creator_script(Script* script);
    int get_refcount() const;
    void increment_refcount();
    void decrement_refcount();

    /**
     * @brief Returns the name identifying this type in Lua.
     * @return the name identifying this type in Lua
     */
    virtual const std::string& get_lua_type_name() const = 0;

  private:

    Script* creator_script;     /**< the script that owns this object, or NULL */
    int refcount;       /**< number of pointers to the object
                         * including the Lua ones
                         * (0 means that it can be deleted) */
};

#endif

