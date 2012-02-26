/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "DisplayableWithEffects.h"
#include "lua/Script.h"

/**
 * @brief Constructor.
 * @param displayable the displayable object to use
 * @param script the owner script of this object, or NULL
 */
DisplayableWithEffects::DisplayableWithEffects(Displayable* displayable, Script* script):
  displayable(displayable),
  movement(NULL),
  movement_callback_ref(LUA_REFNIL),
  transition(NULL),
  transition_callback_ref(LUA_REFNIL),
  script(script) {

}

/**
 * @brief Destructor.
 */
DisplayableWithEffects::~DisplayableWithEffects() {

  stop_transition();
  stop_movement();
  delete displayable;
}

/**
 * @brief Applies a movement to this object.
 *
 * Any previous movement is stopped.
 *
 * @param movement the movement to apply
 */
void DisplayableWithEffects::start_movement(Movement* movement) {

  stop_movement();
  this->movement = movement;
}

/**
 * @brief Stops the movement applied to the object, if any.
 *
 * The movement is deleted unless the owner script uses it elsewhere.
 */
void DisplayableWithEffects::stop_movement() {

  if (movement != NULL) {

    if (script != NULL) {
      delete movement;
    }
    else {
      script->decrement_refcount(movement);
    }
  }
  movement = NULL;
}

/**
 * @brief Sets a Lua function to be called when the movement is finished.
 * @param movement_callback_ref a Lua ref to the callback in the registry
 * (if you pass LUA_REFNIL, this function removes the previous callback that
 * was set, if any)
 */
void DisplayableWithEffects::set_movement_callback(int transition_callback_ref) {

  Debug::check_assertion(script != NULL,
      "Cannot set a transition callback without script");

  this->transition_callback_ref = transition_callback_ref;
}

/**
 * @brief Starts a transition effect on this object.
 *
 * The transition will be automatically deleted when finished or stopped.
 * Any previous transition is stopped.
 */
void DisplayableWithEffects::start_transition(Transition* transition) {

  stop_transition();
  this->transition = transition;
  transition->start();
}

/**
 * @brief Stops the transition effect applied to this object, if any
 *
 * The transition is deleted.
 */
void DisplayableWithEffects::stop_transition() {

  delete transition;
  transition = NULL;
}

/**
 * @brief Sets a Lua function to be called when the transition is finished.
 * @param tansition_callback_ref a Lua ref to the callback in the registry
 * (if you pass LUA_REFNIL, this function removes the previous callback that
 * was set, if any)
 */
void DisplayableWithEffects::set_transition_callback(int transition_callback_ref) {

  Debug::check_assertion(script != NULL,
      "Cannot set a transition callback without script");

  this->transition_callback_ref = transition_callback_ref;
}

/**
 * @brief Updates this object.
 *
 * This function is called repeatedly.
 */
void DisplayableWithEffects::update() {

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
 * @brief Displays this object with the effects applied.
 * @param dst_surface the destination surface
 * @param dst_position where you want the object to be displayed
 */
void DisplayableWithEffects::display_with_effects(Surface& dst_surface,
    Rectangle dst_position) {

  if (transition != NULL) {
    transition->display(dst_surface);
  }

  if (movement != NULL) {
    dst_position.add_xy(movement.get_xy());
  }

  displayable->display(dst_surface, dst_position);
}
