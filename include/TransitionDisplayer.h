/**
 * This module handles the transition effects between two maps.
 */

#ifndef ZSDX_TRANSITION_H
#define ZSDX_TRANSITION_H

#include "Common.h"

class TransitionDisplayer {

 private:

  TransitionDisplayer();

  static void fadeIn(Map *map);
  static void fadeOut(Map *map);
  
 public:

  static void showOutTransition(Map *map, Transition transition);
  static void showInTransition(Map *map, Transition transition);
};

#endif
