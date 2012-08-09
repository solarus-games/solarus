/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "lua/ExportableToLua.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates an object exportable to Lua.
 */
ExportableToLua::ExportableToLua():
  creator_script(NULL),
  refcount(0) {

}

/**
 * @brief Destroys this exportable object.
 */
ExportableToLua::~ExportableToLua() {

  Debug::check_assertion(refcount == 0, StringConcat()
      << "This object is still used somewhere else: refcount is " << refcount);
}

/**
 * @brief Returns the script that created this object if any.
 * @return the creator script or NULL
 */
LuaContext* ExportableToLua::get_creator_script() const {
  return creator_script;
}

/**
 * @brief Sets the creator script of this object.
 * @param creator_script the owner script or NULL
 */
void ExportableToLua::set_creator_script(LuaContext* creator_script) {

  Debug::check_assertion(this->creator_script == NULL || creator_script == NULL,
      "This object is already marked as created by another a Lua script");
  // we can remove this restriction one day by using a list of scripts

  this->creator_script = creator_script;
}

/**
 * @brief Returns the current refcount of this object.
 *
 * You must not delete this object if its refcount is not zero.
 *
 * @return the current refcount of this object
 */
int ExportableToLua::get_refcount() const {
  return refcount;
}

/**
 * @brief Increments the refcount of this object.
 */
void ExportableToLua::increment_refcount() {
  refcount++;
}

/**
 * @brief Decrements the refcount of this object.
 */
void ExportableToLua::decrement_refcount() {
  refcount--;
}

