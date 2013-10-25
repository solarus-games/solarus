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
#ifndef SOLARUS_EXPORTABLE_TO_LUA_H
#define SOLARUS_EXPORTABLE_TO_LUA_H

#include "Common.h"
#include <string>

/**
 * \brief Interface of a C++ type that can also exist as a Lua userdata.
 *
 * An optional reference counting system is provided.
 * It allows you to share safely the object between C++ and Lua,
 * no matter which world stops using it first.
 *
 * Lua bindings use the this system for all userdata created by Lua.
 * When the refcount gets to zero, it means that there is no more Lua userdata
 * for this object, and that C++ is not using the object either.
 * Therefore, you know you can delete the object.
 *
 * Note that if you don't need the refcount system, you don't have to use it.
 * You can just leave the refcount to zero and delete the object normally.
 * You don't need to care about refcounts if your object is not allocated
 * dynamically or if you don't share pointers.
 * If you export your object to Lua, refcounts will be very helpful.
 */
class ExportableToLua {

  public:

    ExportableToLua();
    virtual ~ExportableToLua();

    bool is_exported_to_lua() const;
    void set_exported_to_lua(bool exported_to_lua);

    // Reference counting.
    int get_refcount() const;
    void increment_refcount();
    void decrement_refcount();

    /**
     * \brief Returns the name identifying this type in Lua.
     * \return the name identifying this type in Lua
     */
    virtual const std::string& get_lua_type_name() const = 0;

  private:

    int refcount;                /**< Number of pointers to the object
                                  * including the Lua ones
                                  * (0 means that it can be deleted). */
    bool exported_to_lua;        /**< Whether this object exists in the Lua side. */
};

#endif

