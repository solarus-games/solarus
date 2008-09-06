#ifndef ZSDX_TITLE_SCREEN_H
#define ZSDX_TITLE_SCREEN_H

#include "Common.h"
#include "Screen.h"

/**
 * This class shows the title screen of the game.
 */
class TitleScreen: public Screen {

 private:

  /**
   * The phases of the title screen.
   */
  enum Phase {
    PHASE_BLACK_SCREEN,
    PHASE_ZS_PRESENTS,
    PHASE_TITLE
  };
  
  Phase current_phase;
  Uint32 next_phase_date;
  Transition *transition_in;
  Transition *transition_out;

  // phase 1
  void init_phase_black_screen(void);

  // phase 2
  SDL_Surface *img_zs_presents;
  SDL_Rect zs_presents_position;

  void init_phase_zs_presents(void);
  void exit_phase_zs_presents(void);

  // phase 3
  SDL_Surface *img_title;
  Music *title_screen_music;

  void init_phase_title(void);
  void exit_phase_title(void);

 public:

  // creation and destruction
  TitleScreen(void);
  ~TitleScreen(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void update(void);
  void display(SDL_Surface *screen_surface);
};

#endif
