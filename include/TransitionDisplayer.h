/**
 * This module handles the transition effects between two maps.
 */

#ifndef ZSDX_TRANSITION_H
#define ZSDX_TRANSITION_H

#include "Common.h"

class TransitionDisplayer {

 private:

  TransitionDisplayer();

  static void fade(Map *map, TransitionDirection direction);
  
 public:

  static void showTransition(Map *map, Transition transition, TransitionDirection direction);
};

#endif
