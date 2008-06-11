#include "TransitionEffect.h"
#include "TransitionImmediate.h"
#include "TransitionFade.h"

/**
 * Constructor.
 */
TransitionEffect::TransitionEffect(TransitionDirection direction):
direction(direction) {

}

/**
 * Destructor.
 */
TransitionEffect::~TransitionEffect(void) {

}

/**
 * Creates a transition effect with the specified type and direction.
 * @param type type of transition: TRANSITION_IMMEDIATE, TRANSITION_FADE, etc.
 * @param direction TRANSITION_IN or TRANSITION_OUT
 */
TransitionEffect * TransitionEffect::create_transition(TransitionType type, TransitionDirection direction) {

  TransitionEffect *transition = NULL;

  switch (type) {

  case TRANSITION_IMMEDIATE:
    transition = new TransitionImmediate(direction);
    break;

  case TRANSITION_FADE:
    transition = new TransitionFade(direction);
    break;
  }

  return transition;
}

/**
 * Returns the direction of this transition effect.
 * @returns the direction of this transition effect: TRANSITION_IN or TRANSITION_OUT
 */
TransitionDirection TransitionEffect::get_direction(void) {
  return direction;
}
