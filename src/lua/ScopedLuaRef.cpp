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
#include "lua/ScopedLuaRef.h"
#include "lua/LuaContext.h"
#include <lua.hpp>

namespace solarus {

/**
 * \brief Creates an empty scoped Lua ref.
 */
ScopedLuaRef::ScopedLuaRef():
    lua_context(nullptr),
    ref(LUA_REFNIL) {
}

/**
 * \brief Creates a scoped Lua ref.
 * \param lua_context The Lua context.
 * \param ref The Lua ref, possibly LUA_REFNIL or LUA_NOREF.
 */
ScopedLuaRef::ScopedLuaRef(LuaContext& lua_context, int ref):
    lua_context(&lua_context),
    ref(ref) {
}

/**
 * \brief Copy constructor.
 *
 * This creates a new reference to the same Lua value.
 *
 * \param other The object to copy.
 */
ScopedLuaRef::ScopedLuaRef(const ScopedLuaRef& other):
    lua_context(other.lua_context),
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
    lua_context(other.lua_context),
    ref(other.ref) {

  other.lua_context = nullptr;
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
 */
ScopedLuaRef& ScopedLuaRef::operator=(const ScopedLuaRef& other) {

  clear();
  this->lua_context = other.lua_context;
  if (lua_context != nullptr) {
    if (other.ref == LUA_REFNIL || other.ref == LUA_NOREF) {
      this->ref = other.ref;
    }
    else {
      lua_State* l = lua_context->get_internal_state();
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
 */
ScopedLuaRef& ScopedLuaRef::operator=(ScopedLuaRef&& other) {

  clear();
  this->lua_context = other.lua_context;
  this->ref = other.ref;
  other.lua_context = nullptr;
  other.ref = LUA_REFNIL;  // Don't unref from the other one.

  return *this;
}

/**
 * \brief Returns the Lua context this ref lives in.
 * \return The Lua context (nullptr means that the ref is empty).
 */
LuaContext* ScopedLuaRef::get_lua_context() const {
  return lua_context;
}

/**
 * \brief Returns whether this ref is empty.
 * \return \c true if the ref is empty.
 */
bool ScopedLuaRef::is_empty() const {

  return lua_context == nullptr || ref == LUA_REFNIL || ref == LUA_NOREF;
}

/**
 * \brief Returns the ref.
 * \return The encapsulated ref, possibly LUA_REFNIL or LUA_NOREF.
 */
int ScopedLuaRef::get() const {
  return ref;
}

/**
 * \briefs Changes the ref.
 *
 * Unrefs the previous one if any.
 */
void ScopedLuaRef::set(LuaContext& lua_context, int ref) {

  clear();
  this->lua_context = &lua_context;
  this->ref = ref;
}

/**
 * \brief Destroys the ref.
 *
 * This calls luaL_unref().
 */
void ScopedLuaRef::clear() {

  if (lua_context != nullptr) {
    luaL_unref(lua_context->get_internal_state(), LUA_REGISTRYINDEX, ref);
  }
  lua_context = nullptr;
  ref = LUA_REFNIL;
}

}

