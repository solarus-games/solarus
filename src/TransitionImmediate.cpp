#include "TransitionImmediate.h"

/**
 * Constructor.
 */
TransitionImmediate::TransitionImmediate(TransitionDirection direction):
TransitionEffect(direction) {

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
 * Returns whether the transition effect is over.
 * @return true
 */
bool TransitionImmediate::is_over(void) {
  return true;
}

/**
 * Displays the transition effect on a surface.
 */
void TransitionImmediate::display(SDL_Surface *surface) {

}
