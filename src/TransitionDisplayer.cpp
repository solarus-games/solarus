/**
 * This module handles the transition effects between two maps.
 */

#include "TransitionDisplayer.h"

/**
 * Displays a fade in effect from a black screen.
 */
void TransitionDisplayer::fadeIn(void) {

}


/**
 * Displays a fade out effect from the current screen to a black screen.
 */
void TransitionDisplayer::fadeInOut(void) {

}


/**
 * Displays a specified transition effect.
 */
void showTransition(Transition transition) {

  switch (transition) {

  case TRANSITION_IMMEDIATE:
    // nothing to do
    break;

  case TRANSITION_FADE:
    fadeInOut();
    break;
  }

}
