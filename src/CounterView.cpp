/**
 * This class defines the class CounterView.
 */

#include "CounterView.h"
#include "FileTools.h"
#include "Color.h"

/**
 * Constructor.
 * The counter's value is initialized to zero.
 * @param nb_digits number of digits to display
 * @param x x coordinate of the top-left corner of the counter on the destination surface
 * @param y y coordinate of the top-left corner of the counter on the destination surface
 */
CounterView::CounterView(unsigned int nb_digits, int x, int y):
  nb_digits(nb_digits) {
  
  surface_drawn = SDL_CreateRGBSurface(SDL_HWSURFACE, 8 * nb_digits, 8, 32, 0, 0, 0, 0);
  img_digits = FileTools::open_image("hud/digits.png");
 
  destination_position.x = x;
  destination_position.y = y;

  set_value(0);
}

/**
 * Destructor.
 */
CounterView::~CounterView(void) {
  SDL_FreeSurface(surface_drawn);
}

/**
 * Returns the value currently displayed by the counter.
 */
unsigned int CounterView::get_value(void) {
  return value;
}

/**
 * Changes the counter's value.
 * The surface is redrawn such that the counter displays
 * the new value.
 */
void CounterView::set_value(unsigned int value) {

  if (value != this->value) {

    this->value = value;

    SDL_FillRect(surface_drawn, NULL, COLOR_BLACK);

    SDL_Rect digit_position_in_src = {0, 0, 8, 8};
    SDL_Rect digit_position_in_counter = {0, 0, 8, 8};

    for (int i = nb_digits - 1; i >= 0; i--) {

      // compute each digit
      Uint8 digit = value % 10;
      value /= 10;
      
      // draw the surface
      digit_position_in_src.x = digit * 8;
      digit_position_in_counter.x = i * 8;
      
      SDL_BlitSurface(img_digits, &digit_position_in_src, surface_drawn, &digit_position_in_counter); 
    }
  }
}

/**
 * Adds 1 to the counter.
 * The surface is redrawn such that the counter displays
 * the new value.
 */
void CounterView::increase(void) {
  set_value(value + 1);
}

/**
 * Substracts 1 from the counter.
 * The surface is redrawn such that the counter displays
 * the new value.
 */
void CounterView::decrease(void) {
  set_value(value - 1);
}

/**
 * Displays the counter on a surface.
 * @param destination the destination surface
 */
void CounterView::display(SDL_Surface *destination) {
  SDL_BlitSurface(surface_drawn, NULL, destination, &destination_position);
}
