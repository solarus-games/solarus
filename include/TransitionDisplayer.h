/**
 * This module handles the transition effects between two maps.
 */

#ifndef ZSDX_TRANSITION_H
#define ZSDX_TRANSITION_H

#include "Common.h"

class TransitionDisplayer {

 private:

  TransitionDisplayer();

  static void fadeIn(void);
  static void fadeOut(void);
  
 public:

  static void showTransition(Transition transition);
}

#endif
