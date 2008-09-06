#include "Transition.h"
#include "TransitionImmediate.h"
#include "TransitionFade.h"

/**
 * Constructor.
 */
Transition::Transition(Transition::Direction direction):
direction(direction) {

}

/**
 * Destructor.
 */
Transition::~Transition(void) {

}

/**
 * Creates a transition effect with the specified type and direction.
 * @param style style of the transition: Transition::IMMEDIATE, Transition::FADE, etc.
 * @param direction Transition::IN or Transition::OUT
 */
Transition * Transition::create(Transition::Style style, Transition::Direction direction) {

  Transition *transition = NULL;

  switch (style) {

  case Transition::IMMEDIATE:
    transition = new TransitionImmediate(direction);
    break;

  case Transition::FADE:
    transition = new TransitionFade(direction);
    break;
  }

  return transition;
}

/**
 * Returns the direction of this transition effect.
 * @returns the direction of this transition effect: Transition::IN or Transition::OUT
 */
Transition::Direction Transition::get_direction(void) {
  return direction;
}
