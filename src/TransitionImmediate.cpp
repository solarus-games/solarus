/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "TransitionImmediate.h"

/**
 * Constructor.
 */
TransitionImmediate::TransitionImmediate(Transition::Direction direction):
  Transition(direction) {

}

/**
 * Destructor.
 */
TransitionImmediate::~TransitionImmediate(void) {

}

/**
 * Starts this transition effect.
 */
void TransitionImmediate::start(void) {
  
}

/**
 * Returns whether the transition effect is started.
 * @return false
 */
bool TransitionImmediate::is_started(void) {
  return false;
}

/**
 * Returns whether the transition effect is finished.
 * @return true
 */
bool TransitionImmediate::is_finished(void) {
  return true;
}

/**
 * Displays the transition effect on a surface.
 */
void TransitionImmediate::display(Surface *surface) {

}

