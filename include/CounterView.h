#ifndef ZSDX_COUNTER_VIEW_H
#define ZSDX_COUNTER_VIEW_H

#include "Common.h"

/**
 * This class displays a counter on a surface.
 * The counter can only display positive integer numbers.
 */
class CounterView {

 private:

  unsigned int nb_digits;
  bool fill_with_zeros;

  unsigned int value;
  SDL_Surface *surface_drawn;
  SDL_Rect destination_position;
  SDL_Surface *img_digits;

  void rebuild_with_value(unsigned int value);
 
 public:
  
  CounterView(unsigned int nb_digits, bool fill_with_zeros,
	      int x, int y);
  ~CounterView(void);

  unsigned int get_value(void);
  void set_value(unsigned int value);
  void increase(void);
  void decrease(void);

  void display(SDL_Surface *destination);
};

#endif
