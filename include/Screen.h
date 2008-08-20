#ifndef ZSDX_SCREEN_H
#define ZSDX_SCREEN_H

#include "Common.h"

/**
 * A screen is an execution phase of the program, like the title
 * screen, the selection menu or the game.
 * There is always a current screen and it is handled by the SDL main loop
 * in the ZSDX class.
 */
class Screen {

 private:

  bool screen_finished;
  Screen *next_screen;

 protected:

  Screen(void);

  void set_next_screen(Screen *next_screen);

 public:

  virtual ~Screen(void);

  virtual bool is_screen_finished(void);
  virtual Screen *get_next_screen(void);

  virtual void update(void) = 0;
  virtual void display(SDL_Surface *screen_surface) = 0;
  virtual void handle_event(const SDL_Event &event) = 0;
};

#endif

