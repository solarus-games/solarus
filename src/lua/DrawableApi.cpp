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
#include "lua/LuaContext.h"
#include "lua/LuaTools.h"
#include "lowlevel/Surface.h"
#include "movements/Movement.h"
#include "Drawable.h"
#include "TransitionFade.h"
#include <lua.hpp>

/* This file contains common code for all drawable types known by Lua,
 * i.e. surfaces, text surfaces and sprites.
 */

namespace solarus {

/**
 * \brief Returns whether a value is a userdata of a type.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a drawable.
 */
bool LuaContext::is_drawable(lua_State* l, int index) {
  return is_surface(l, index)
      || is_text_surface(l, index)
      || is_sprite(l, index);
}

/**
 * \brief Check that the userdata at the specified index is a drawable
 * object (surface, text surface of sprite) and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the surface
 */
Drawable& LuaContext::check_drawable(lua_State* l, int index) {

  if (is_drawable(l, index)) {
    const ExportableToLuaPtr& userdata = *(static_cast<ExportableToLuaPtr*>(
      lua_touserdata(l, index)
    ));
    return *std::static_pointer_cast<Drawable>(userdata);
  }
  else {
    LuaTools::type_error(l, index, "drawable");
    throw;
  }
}

/**
 * \brief Returns whether a drawable object was created by this script.
 * \param drawable A drawable object.
 * \return true if the drawable object was created by this script.
 */
bool LuaContext::has_drawable(Drawable* drawable) {

  return drawables.find(drawable) != drawables.end();
}

/**
 * \brief Registers a drawable object created by this script.
 * \param drawable a drawable object
 */
void LuaContext::add_drawable(Drawable* drawable) {

  Debug::check_assertion(!has_drawable(drawable),
      "This drawable object is already registered");

  RefCountable::ref(drawable);
  drawables.insert(drawable);
}

/**
 * \brief Unregisters a drawable object created by this script.
 * \param drawable a drawable object
 */
void LuaContext::remove_drawable(Drawable* drawable) {

  Debug::check_assertion(has_drawable(drawable),
      "This drawable object was not created by Lua");

  drawables_to_remove.insert(drawable);
}

/**
 * \brief Destroys from Lua all drawable objects created
 * by this script.
 */
void LuaContext::destroy_drawables() {

  for (Drawable* drawable: drawables) {
    RefCountable::unref(drawable);
  }
  drawables.clear();
  drawables_to_remove.clear();
}

/**
 * \brief Updates all drawable objects created by this script.
 */
void LuaContext::update_drawables() {

  // Update all drawables.
  for (Drawable* drawable: drawables) {
    if (has_drawable(drawable)) {
      drawable->update();
    }
  }

  // Remove the ones that should be removed.
  for (Drawable* drawable: drawables_to_remove) {
    drawables.erase(drawable);
    RefCountable::unref(drawable);
  }
  drawables_to_remove.clear();
}

/**
 * \brief Implementation of drawable:draw().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::drawable_api_draw(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    Drawable& drawable = check_drawable(l, 1);
    Surface& dst_surface = check_surface(l, 2);
    int x = LuaTools::opt_int(l, 3, 0);
    int y = LuaTools::opt_int(l, 4, 0);
    SurfacePtr shared_dst_surface = RefCountable::make_refcount_ptr(&dst_surface);  // TODO shared_ptr
    drawable.draw(shared_dst_surface, x, y);

    return 0;
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);
}

/**
 * \brief Implementation of drawable:draw_region().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::drawable_api_draw_region(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    Drawable& drawable = check_drawable(l, 1);
    Rectangle region = {
        LuaTools::check_int(l, 2),
        LuaTools::check_int(l, 3),
        LuaTools::check_int(l, 4),
        LuaTools::check_int(l, 5)
    };
    Surface& dst_surface = check_surface(l, 6);
    Point dst_position = {
        LuaTools::opt_int(l, 7, 0),
        LuaTools::opt_int(l, 8, 0)
    };
    SurfacePtr shared_dst_surface = RefCountable::make_refcount_ptr(&dst_surface);  // TODO shared_ptr
    drawable.draw_region(region, shared_dst_surface, dst_position);

    return 0;
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);
}

/**
 * \brief Implementation of drawable:fade_in().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::drawable_api_fade_in(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    uint32_t delay = 20;
    ScopedLuaRef callback_ref;

    Drawable& drawable = check_drawable(l, 1);

    if (lua_gettop(l) >= 2) {
      // the second argument can be the delay or the callback
      int index = 2;
      if (lua_isnumber(l, index)) {
        delay = lua_tonumber(l, index);
        index++;
      }
      // the next argument (if any) is the callback
      callback_ref = LuaTools::opt_function(l, index);
    }

    TransitionFade* transition = new TransitionFade(
        Transition::TRANSITION_OPENING,
        drawable.get_transition_surface()
    );
    transition->clear_color();
    transition->set_delay(delay);
    drawable.start_transition(*transition, callback_ref);

    return 0;
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);
}

/**
 * \brief Implementation of drawable:fade_out().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::drawable_api_fade_out(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    uint32_t delay = 20;
    ScopedLuaRef callback_ref;

    Drawable& drawable = check_drawable(l, 1);

    if (lua_gettop(l) >= 2) {
      // the second argument can be the delay or the callback
      int index = 2;
      if (lua_isnumber(l, index)) {
        delay = lua_tonumber(l, index);
        index++;
      }
      // the next argument (if any) is the callback
      callback_ref = LuaTools::opt_function(l, index);
    }

    TransitionFade* transition = new TransitionFade(
        Transition::TRANSITION_CLOSING,
        drawable.get_transition_surface());
    transition->clear_color();
    transition->set_delay(delay);
    drawable.start_transition(*transition, callback_ref);

    return 0;
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);
}

/**
 * \brief Implementation of drawable:get_xy().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::drawable_api_get_xy(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    Drawable& drawable = check_drawable(l, 1);

    lua_pushinteger(l, drawable.get_xy().x);
    lua_pushinteger(l, drawable.get_xy().y);
    return 2;
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);
}

/**
 * \brief Implementation of drawable:set_xy().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::drawable_api_set_xy(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    Drawable& drawable = check_drawable(l, 1);
    int x = LuaTools::check_int(l, 2);
    int y = LuaTools::check_int(l, 3);

    drawable.set_xy(Point(x, y));

    return 0;
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);
}

/**
 * \brief Implementation of drawable:get_movement().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::drawable_api_get_movement(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    Drawable& drawable = check_drawable(l, 1);

    Movement* movement = drawable.get_movement();
    if (movement == nullptr) {
      lua_pushnil(l);
    }
    else {
      push_userdata(l, *movement);
    }

    return 1;
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);
}

/**
 * \brief Implementation of drawable:stop_movement().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::drawable_api_stop_movement(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    Drawable& drawable = check_drawable(l, 1);

    drawable.stop_movement();

    return 0;
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);
}

/**
 * \brief Finalizer of types sprite, surface and text surface.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::drawable_meta_gc(lua_State* l) {

  SOLARUS_LUA_BOUNDARY_TRY() {
    LuaContext& lua_context = get_lua_context(l);
    Drawable& drawable = check_drawable(l, 1);

    if (lua_context.has_drawable(&drawable)) {
      // This drawable was created from Lua.
      lua_context.remove_drawable(&drawable);
    }
    userdata_meta_gc(l);
  }
  SOLARUS_LUA_BOUNDARY_CATCH(l);

  return 0;
}

}

