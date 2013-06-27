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
#include "lua/ExportableToLua.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * \brief Creates an object exportable to Lua.
 */
ExportableToLua::ExportableToLua():
  refcount(0) {

}

/**
 * \brief Destroys this exportable object.
 */
ExportableToLua::~ExportableToLua() {

  Debug::check_assertion(refcount == 0, StringConcat()
      << "This object is still used somewhere else: refcount is " << refcount);
}

/**
 * \brief Returns the current refcount of this object.
 *
 * You must not delete this object if its refcount is not zero.
 *
 * \return the current refcount of this object
 */
int ExportableToLua::get_refcount() const {
  return refcount;
}

/**
 * \brief Increments the refcount of this object.
 */
void ExportableToLua::increment_refcount() {

  refcount++;
}

/**
 * \brief Decrements the refcount of this object.
 */
void ExportableToLua::decrement_refcount() {

  Debug::check_assertion(refcount > 0, "The refcount is already 0");
  refcount--;
}

