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
#include "solarus/lua/LuaTools.h"
#include "solarus/lua/ScopedLuaRef.h"
#include <lua.hpp>

namespace Solarus {

/**
 * \brief Creates an empty scoped Lua ref.
 */
ScopedLuaRef::ScopedLuaRef():
    l(nullptr),
    ref(LUA_REFNIL) {
}

/**
 * \brief Creates a scoped Lua ref.
 * \param l The Lua state (cannot be nullptr).
 * \param ref The Lua ref, possibly LUA_REFNIL or LUA_NOREF.
 */
ScopedLuaRef::ScopedLuaRef(lua_State* l, int ref):
    l(l),
    ref(ref) {
  Debug::check_assertion(l != nullptr, "Missing Lua state");
}

/**
 * \brief Copy constructor.
 *
 * This creates a new reference to the same Lua value.
 *
 * \param other The object to copy.
 */
ScopedLuaRef::ScopedLuaRef(const ScopedLuaRef& other):
    l(other.l),
    ref(LUA_REFNIL) {

  *this = other;
}

/**
 * \brief Move constructor.
 *
 * This transfers the ref to the new object and leaves the existing one empty.
 *
 * \param other The object to move.
 */
ScopedLuaRef::ScopedLuaRef(ScopedLuaRef&& other):
    l(other.l),
    ref(other.ref) {

  other.l = nullptr;
  other.ref = LUA_REFNIL;  // Don't unref from the other one.
}

/**
 * \brief Destroys the ref.
 */
ScopedLuaRef::~ScopedLuaRef() {
  clear();
}

/**
 * \brief Copy assignment operator.
 * \param other The object to copy.
 * \return The current ref after modification.
 */
ScopedLuaRef& ScopedLuaRef::operator=(const ScopedLuaRef& other) {

  clear();
  this->l = other.l;
  if (l != nullptr) {
    if (other.ref == LUA_REFNIL || other.ref == LUA_NOREF) {
      this->ref = other.ref;
    }
    else {
      // Get the value and make another ref of it.
      lua_rawgeti(l, LUA_REGISTRYINDEX, other.ref);
      this->ref = luaL_ref(l, LUA_REGISTRYINDEX);
    }
  }

  return *this;
}

/**
 * \brief Move assignment operator.
 * \param other The object to move.
 * \return The current ref after modification.
 */
ScopedLuaRef& ScopedLuaRef::operator=(ScopedLuaRef&& other) {

  clear();
  this->l = other.l;
  this->ref = other.ref;
  other.l = nullptr;
  other.ref = LUA_REFNIL;  // Don't unref from the other one.

  return *this;
}

/**
 * \brief Returns the Lua state this ref lives in.
 * \return The Lua state (nullptr means that the ref is empty).
 */
lua_State* ScopedLuaRef::get_lua_state() const {
  return l;
}

/**
 * \brief Returns whether this ref is empty.
 * \return \c true if the ref is empty.
 */
bool ScopedLuaRef::is_empty() const {

  return l == nullptr || ref == LUA_REFNIL || ref == LUA_NOREF;
}

/**
 * \brief Returns the ref.
 * \return The encapsulated ref, possibly LUA_REFNIL or LUA_NOREF.
 */
int ScopedLuaRef::get() const {
  return ref;
}

/**
 * \brief Destroys the ref.
 *
 * This calls luaL_unref().
 */
void ScopedLuaRef::clear() {

  if (l != nullptr) {
    luaL_unref(l, LUA_REGISTRYINDEX, ref);
  }
  l = nullptr;
  ref = LUA_REFNIL;
}

/**
 * \brief Pushes the referenced value onto the Lua stack.
 *
 * The ref must not be empty.
 */
void ScopedLuaRef::push() const {

  Debug::check_assertion(!is_empty(), "Attempt to push an empty ref");

  lua_rawgeti(l, LUA_REGISTRYINDEX, ref);
}

/**
 * \brief Calls the referenced function.
 *
 * If the referenced value is not empty, it must be a function.
 * No parameters are passed to the function, and return values are ignored.
 *
 * \param function_name A name describing the Lua function (only used to
 * print the error message if any).
 */
void ScopedLuaRef::call(const std::string& function_name) const {

  if (!is_empty()) {
    push();
    LuaTools::call_function(l, 0, 0, function_name.c_str());
  }
}

/**
 * \brief Like call(), but clears the reference before calling
 * the function.
 *
 * This avoids reentrant calls: your reference will become clean before
 * the function is called.
 *
 * This is equivalent to:
 *
 * ScopedLuaRef copy = callback_ref;
 * callback_ref.clear();  // Avoid nasty problems in case of reentrant calls.
 * copy.call(s);
 *
 * If the reference is already empty, nothing happens.
 * \param function_name A name describing the Lua function (only used to
 * print the error message if any).
 */
void ScopedLuaRef::clear_and_call(const std::string& function_name) {

  if (is_empty()) {
    return;
  }

  lua_State* l = this->l;
  push();
  clear();  // The function is still alive, onto the stack.
  LuaTools::call_function(l, 0, 0, function_name.c_str());
}

}

