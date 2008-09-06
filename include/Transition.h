#ifndef ZSDX_TRANSITION_H
#define ZSDX_TRANSITION_H

#include "Common.h"

/**
 * A transition effect between two screens (maps or not).
 */
class Transition {

 public:

  /**
   * Style of transition between two maps.
   */
  enum Style {
    IMMEDIATE,  // no transition between the two maps
    FADE,       // fade in and fade out
  };

  /**
   * Direction of the transition.
   */
  enum Direction {
    IN,
    OUT,
  };

 protected:

  Direction direction;

  Transition(Direction direction);

 public:
  virtual ~Transition(void);
  static Transition *create(Style style, Direction direction);

  Direction get_direction(void);

  virtual void start(void) = 0;
  virtual bool is_started(void) = 0;
  virtual bool is_over(void) = 0;
  virtual void display(SDL_Surface *surface) = 0;

};

#endif
