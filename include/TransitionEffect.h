#ifndef ZSDX_TRANSITION_H
#define ZSDX_TRANSITION_H

#include "Common.h"

/**
 * Type of transition between two maps.
 */
enum TransitionType {
  TRANSITION_IMMEDIATE,  // no transition between the two maps
  TRANSITION_FADE,       // fade in and fade out
};

/**
 * Direction of the transition.
 */
enum TransitionDirection {
  TRANSITION_IN,
  TRANSITION_OUT
};

/**
 * A transition effect between two screens (maps or not).
 */
class TransitionEffect {

 protected:

  TransitionDirection direction;

  TransitionEffect(TransitionDirection direction);

 public:
  virtual ~TransitionEffect(void);
  static TransitionEffect *create(TransitionType type, TransitionDirection direction);

  TransitionDirection get_direction(void);

  virtual void start(void) = 0;
  virtual bool is_started(void) = 0;
  virtual bool is_over(void) = 0;
  virtual void display(SDL_Surface *surface) = 0;

};

#endif
