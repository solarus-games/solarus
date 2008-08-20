#include "Screen.h"

/**
 * Creates a screen.
 */
Screen::Screen(void):
  screen_finished(false), next_screen(NULL) {

}

/**
 * Destroys the screen.
 */
Screen::~Screen(void) {
  if (next_screen != NULL) {
    delete next_screen;
  }
}

/**
 * Returns whether this screen is finished.
 * If true is returned, the get_next_screen()
 * function indicates what screen should be
 * displayed now.
 * @return true if the screen finished
 */
bool Screen::is_screen_finished(void) {
  return screen_finished;
}

/**
 * When this screen is finished, returns the
 * screen that should be displayed now.
 * @return the next screen
 */
Screen * Screen::get_next_screen(void) {
  return next_screen;
}
