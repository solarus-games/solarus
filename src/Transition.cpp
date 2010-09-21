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
#include "Transition.h"
#include "TransitionImmediate.h"
#include "TransitionFade.h"
#include "TransitionScrolling.h"
#include "lowlevel/Debug.h"

/**
 * @brief Creates a transition effect.
 * @param direction direction of the transition effect (in or out)
 */
Transition::Transition(Transition::Direction direction):
  direction(direction), previous_surface(NULL) {

}

/**
 * @brief Destructor.
 */
Transition::~Transition(void) {

}

/**
 * @brief Creates a transition effect with the specified type and direction.
 * @param style style of the transition: Transition::IMMEDIATE, Transition::FADE, etc.
 * @param direction Transition::IN or Transition::OUT
 * @param game the current game if any (used by some kinds of transitions)
 */
Transition * Transition::create(Transition::Style style, Transition::Direction direction, Game *game) {

  Transition *transition = NULL;

  switch (style) {

  case Transition::IMMEDIATE:
    transition = new TransitionImmediate(direction);
    break;

  case Transition::FADE:
    transition = new TransitionFade(direction);
    break;

  case Transition::SCROLLING:
    transition = new TransitionScrolling(direction);
    break;
  }

  transition->game = game;

  return transition;
}

/**
 * @brief Returns the direction of this transition effect.
 * @returns the direction of this transition effect: Transition::IN or Transition::OUT
 */
Transition::Direction Transition::get_direction(void) {
  return direction;
}

/**
 * @brief Indicates the surface that was shown during the OUT transition
 * that was played before this IN transition.
 * @param previous_surface the previous surface
 */
void Transition::set_previous_surface(Surface *previous_surface) {

  Debug::assert(get_direction() != OUT, "Cannot show a previous surface with an OUT transition effect");

  this->previous_surface = previous_surface;
}

/**
 * @brief Returns whether this transition effect needs the previous surface.
 * @return false
 */
bool Transition::needs_previous_surface(void) {
  return false;
}

