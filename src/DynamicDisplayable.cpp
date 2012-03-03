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
#include "DynamicDisplayable.h"
#include "Transition.h"
#include "movements/Movement.h"
#include "lua/Script.h"
#include "lowlevel/Debug.h"
#include <lua.hpp>

/**
 * @brief Constructor.
 */
DynamicDisplayable::DynamicDisplayable():
  last_position(),
  movement(NULL),
  movement_callback_ref(LUA_REFNIL),
  transition(NULL),
  transition_callback_ref(LUA_REFNIL),
  script(NULL) {

}

/**
 * @brief Destructor.
 */
DynamicDisplayable::~DynamicDisplayable() {

  stop_transition();
  stop_movement();
}

/**
 * @brief Sets the script that owns this object.
 * @param script the owner script or NULL
 */
void DynamicDisplayable::set_script(Script* script) {

  this->script = script;
}

/**
 * @brief Returns the surface that represents this displayable object, if any.
 *
 * This function is used for transitions.
 * If you return a valid surface, transitions will be applied to
 * that surface.
 * If you return NULL, transitions will have no visible effect.
 *
 * @return the surface of this object, or NULL
 */
Surface* DynamicDisplayable::get_surface() {
  return NULL;
}

/**
 * @brief Applies a movement to this object.
 *
 * Any previous movement is stopped.
 *
 * @param movement the movement to apply
 * @param callback_ref a Lua registry ref to the function to call when
 * the movement finishes, or LUA_REFNIL
 */
void DynamicDisplayable::start_movement(Movement& movement,
    int callback_ref) {

  stop_movement();
  this->movement = &movement;
  this->movement_callback_ref = callback_ref;

  if (script != NULL) {
    script->increment_refcount(this->movement);
  }
}

/**
 * @brief Stops the movement applied to the object, if any.
 *
 * The movement is deleted unless the owner script uses it elsewhere.
 */
void DynamicDisplayable::stop_movement() {

  if (movement != NULL) {

    // keep the position after the movement
    last_position.add_xy(movement->get_xy());

    if (script != NULL) {
      delete movement;
    }
    else {
      script->decrement_refcount(movement);
    }
  }
  movement = NULL;

  if (script != NULL) {
    script->cancel_callback(this->movement_callback_ref);
    movement_callback_ref = LUA_REFNIL;
  }
}

/**
 * @brief Sets a Lua function to be called when the movement is finished.
 * @param movement_callback_ref a Lua ref to the callback in the registry
 * (if you pass LUA_REFNIL, this function removes the previous callback that
 * was set, if any)
 */
void DynamicDisplayable::set_movement_callback(int movement_callback_ref) {

  Debug::check_assertion(script != NULL,
      "Cannot set a transition callback without script");

  this->movement_callback_ref = movement_callback_ref;
}

/**
 * @brief Starts a transition effect on this object.
 *
 * The transition will be automatically deleted when finished or stopped.
 * Any previous transition is stopped.
 *
 * @param callback_ref a Lua registry ref to the function to call when
 * the transition finishes, or LUA_REFNIL
 */
void DynamicDisplayable::start_transition(Transition& transition,
    int callback_ref) {

  stop_transition();
  this->transition = &transition;
  this->transition_callback_ref = callback_ref;
  transition.start();
}

/**
 * @brief Stops the transition effect applied to this object, if any
 *
 * The transition is deleted.
 */
void DynamicDisplayable::stop_transition() {

  delete transition;
  transition = NULL;

  if (script != NULL) {
    script->cancel_callback(this->transition_callback_ref);
    transition_callback_ref = LUA_REFNIL;
  }
}

/**
 * @brief Sets a Lua function to be called when the transition is finished.
 * @param tansition_callback_ref a Lua ref to the callback in the registry
 * (if you pass LUA_REFNIL, this function removes the previous callback that
 * was set, if any)
 */
void DynamicDisplayable::set_transition_callback(int transition_callback_ref) {

  Debug::check_assertion(script != NULL,
      "Cannot set a transition callback without script");

  script->cancel_callback(this->transition_callback_ref);
  this->transition_callback_ref = transition_callback_ref;
}

/**
 * @brief Updates this object.
 *
 * This function is called repeatedly.
 * You can redefine it for your needs.
 */
void DynamicDisplayable::update() {

  if (transition != NULL) {
    transition->update();
    if (transition->is_finished()) {
      stop_transition();

      if (script != NULL) {
        script->do_callback(transition_callback_ref);
        transition_callback_ref = LUA_REFNIL;
      }
    }
  }

  if (movement != NULL) {
    movement->update();
    if (movement->is_finished()) {
      stop_movement();

      if (script != NULL) {
        script->do_callback(movement_callback_ref);
        movement_callback_ref = LUA_REFNIL;
      }
    }
  }
}

/**
 * @brief Displays this object, applying dynamic effects.
 * @param dst_surface the destination surface
 */
void DynamicDisplayable::display(Surface& dst_surface) {

  display(dst_surface, Rectangle(0, 0));
}

/**
 * @brief Displays this object, applying dynamic effects.
 * @param dst_surface the destination surface
 * @param x x coordinate of where to display
 * @param y y coordinate of where to display
 */
void DynamicDisplayable::display(Surface& dst_surface, int x, int y) {

  display(dst_surface, Rectangle(x, y));
}

/**
 * @brief Displays this object, applying dynamic effects.
 * @param dst_surface the destination surface
 * @param dst_position position on this surface
 * (will be added to the position obtained by previous movements)
 */
void DynamicDisplayable::display(Surface& dst_surface,
    Rectangle dst_position) {

  dst_position.add_xy(last_position);
  if (movement != NULL) {
    dst_position.add_xy(movement->get_xy());
  }

  if (transition != NULL) {
    Surface* surface = get_surface();
    if (surface != NULL) {
      // this class defines a source surface: we can show the transition
      transition->display(*surface);
    }
  }

  raw_display(dst_surface, dst_position);
}

