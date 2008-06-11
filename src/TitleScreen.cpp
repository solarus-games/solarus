#include "TitleScreen.h"
#include "ZSDX.h"
#include "Music.h"
#include "GameResource.h"
#include "FileTools.h"
#include "Color.h"
#include "TransitionEffect.h"

/**
 * Creates a title screen.
 */
TitleScreen::TitleScreen(void) {

}

/**
 * Destroys the title screen.
 */
TitleScreen::~TitleScreen(void) {
  
}

/**
 * Shows the title screen.
 * @return true if the user wants to quit the program
 */
void TitleScreen::show(void) {

  phase_1_black_screen();

  if (!zsdx->is_exiting()) {
    phase_2_zs_presents();

    if (!zsdx->is_exiting()) {
      phase_3_title();
    }
  }
}

/**
 * Phase 1 of the title screen: shows a black screen
 * for a fraction of second.
 * @return true if the user wants to quit the program, false otherwise
 */
void TitleScreen::phase_1_black_screen(void) {

  SDL_Event event;

  // wait 0.3 second
  Uint32 start_intro_time = SDL_GetTicks() + 300;
  while (!zsdx->is_exiting() && SDL_GetTicks() < start_intro_time) {

    if (SDL_PollEvent(&event)) {
      zsdx->handle_event(event);
    }

    SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);
    SDL_Flip(zsdx->screen);
  }
}

/**
 * Phase 2 of the title screen: shows the message "Zelda Solarus presents".
 * @return true if the user wants to quit the program, false otherwise
 */
void TitleScreen::phase_2_zs_presents(void) {

  SDL_Event event;

  SDL_Surface *img_zs_presents = FileTools::open_image("menus/zelda_solarus_presents.png");
  zsdx->game_resource->get_sound("intro")->play();
  SDL_Rect position = {112, 96, 0, 0};
  Uint32 end_intro_time = SDL_GetTicks() + 2000; // intro: 2 seconds
  TransitionEffect *transition = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_OUT);

  while (!zsdx->is_exiting() && !transition->is_over()) {
    
    if (SDL_PollEvent(&event)) {
      zsdx->handle_event(event);
    }

    if (SDL_GetTicks() >= end_intro_time && !transition->is_started()) {
      transition->start();
    }

    SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);

    if (transition->is_started()) {
      transition->display(img_zs_presents);
    }
    SDL_BlitSurface(img_zs_presents, NULL, zsdx->screen, &position);
    SDL_Flip(zsdx->screen);
  }
  SDL_FreeSurface(img_zs_presents);
  delete transition;
}

/**
 * Phase 3 of the title screen: shows the title screen.
 * @return true if the user wants to quit the program, false otherwise
 */
void TitleScreen::phase_3_title(void) {

  SDL_Event event;

  SDL_Surface *img_title = FileTools::open_image("menus/title.png");
  Music *title_screen_music = zsdx->game_resource->get_music("title_screen_full.it");
  title_screen_music->play();
  TransitionEffect *transition = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_IN);
  transition->start();

  // wait until the user presses the space bar
  bool start = false;
  while (!zsdx->is_exiting() && !start) {

    if (SDL_PollEvent(&event)) {
      zsdx->handle_event(event);

      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
	start = true;
      }
    }

    SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);
    if (!transition->is_over()) {
      transition->display(img_title);
    }
    SDL_BlitSurface(img_title, NULL, zsdx->screen, NULL);
    SDL_Flip(zsdx->screen);
  }
  delete transition;

  // transition
  if (!zsdx->is_exiting()) {

    transition = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_OUT);
    transition->start();
    while (!zsdx->is_exiting() && !transition->is_over()) {

      if (SDL_PollEvent(&event)) {
	zsdx->handle_event(event);
      }

      SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);
      if (!transition->is_over()) {
	transition->display(img_title);
      }
      SDL_BlitSurface(img_title, NULL, zsdx->screen, NULL);
      SDL_Flip(zsdx->screen);
    }
    delete transition;
  }

  SDL_FreeSurface(img_title);

  // stop the title screen music
  title_screen_music->stop();
}
