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
#include "Drawable.h"
#include "Transition.h"
#include "movements/Movement.h"
#include "lua/LuaContext.h"
#include "lowlevel/Debug.h"
#include <lua.hpp>

/**
 * @brief Constructor.
 */
Drawable::Drawable():
  last_position(),
  movement(NULL),
  movement_callback_ref(LUA_REFNIL),
  transition(NULL),
  transition_callback_ref(LUA_REFNIL),
  lua_context(NULL) {

}

/**
 * @brief Destructor.
 */
Drawable::~Drawable() {

  stop_transition();
  stop_movement();
}

/**
 * @brief Applies a movement to this object.
 *
 * Any previous movement is stopped.
 *
 * @param movement The movement to apply.
 * @param callback_ref A Lua registry ref to the function to call when
 * the movement finishes, or LUA_REFNIL.
 * @param lua_context The Lua world for the callback (or NULL).
 */
void Drawable::start_movement(Movement& movement,
    int callback_ref, LuaContext* lua_context) {

  stop_movement();
  this->movement = &movement;
  this->movement_callback_ref = callback_ref;
  this->lua_context = lua_context;
  movement.increment_refcount();
}

/**
 * @brief Stops the movement applied to the object, if any.
 *
 * The movement is deleted unless the owner script uses it elsewhere.
 */
void Drawable::stop_movement() {

  if (movement != NULL) {

    // keep the position after the movement
    last_position.add_xy(movement->get_xy());

    movement->decrement_refcount();
    if (movement->get_refcount() == 0) {
      delete movement;
    }
  }
  movement = NULL;

  if (lua_context != NULL) {
    lua_context->cancel_callback(this->movement_callback_ref);
    movement_callback_ref = LUA_REFNIL;
  }
}

/**
 * @brief Starts a transition effect on this object.
 *
 * The transition will be automatically deleted when finished or stopped.
 * Any previous transition is stopped.
 *
 * @param transition The transition to start.
 * @param callback_ref A Lua registry ref to the function to call when
 * the transition finishes, or LUA_REFNIL.
 * @param lua_context The Lua world for the callback (or NULL).
 */
void Drawable::start_transition(Transition& transition,
    int callback_ref, LuaContext* lua_context) {

  stop_transition();

  this->transition = &transition;
  this->transition_callback_ref = callback_ref;
  this->lua_context = lua_context;
  transition.start();
}

/**
 * @brief Stops the transition effect applied to this object, if any
 *
 * The transition is deleted.
 */
void Drawable::stop_transition() {

  delete transition;
  transition = NULL;

  if (lua_context != NULL) {
    lua_context->cancel_callback(this->transition_callback_ref);
    transition_callback_ref = LUA_REFNIL;
  }
}

/**
 * @brief Updates this object.
 *
 * This function is called repeatedly.
 * You can redefine it for your needs.
 */
void Drawable::update() {

  if (transition != NULL) {
    transition->update();
    if (transition->is_finished()) {

      if (lua_context != NULL) {
        lua_context->do_callback(transition_callback_ref);
        transition_callback_ref = LUA_REFNIL;
      }
      stop_transition();
    }
  }

  if (movement != NULL) {
    movement->update();
    if (movement->is_finished()) {

      if (lua_context != NULL) {
        lua_context->do_callback(movement_callback_ref);
        movement_callback_ref = LUA_REFNIL;
      }
      stop_movement();
    }
  }
}

/**
 * @brief Draws this object, applying dynamic effects.
 * @param dst_surface the destination surface
 */
void Drawable::draw(Surface& dst_surface) {

  draw(dst_surface, Rectangle(0, 0));
}

/**
 * @brief Draws this object, applying dynamic effects.
 * @param dst_surface the destination surface
 * @param x x coordinate of where to draw
 * @param y y coordinate of where to draw
 */
void Drawable::draw(Surface& dst_surface, int x, int y) {

  draw(dst_surface, Rectangle(x, y));
}

/**
 * @brief Draws this object, applying dynamic effects.
 * @param dst_surface the destination surface
 * @param dst_position position on this surface
 * (will be added to the position obtained by previous movements)
 */
void Drawable::draw(Surface& dst_surface,
    Rectangle dst_position) {

  dst_position.add_xy(last_position);
  if (movement != NULL) {
    dst_position.add_xy(movement->get_xy());
  }

  if (transition != NULL) {
    draw_transition(*transition);
  }

  raw_draw(dst_surface, dst_position);
}

