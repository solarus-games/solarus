#include "menus/TitleScreen.h"
#include "ZSDX.h"
#include "Music.h"
#include "GameResource.h"
#include "FileTools.h"
#include "Color.h"
#include "TransitionEffect.h"

/**
 * Creates a title screen.
 */
TitleScreen::TitleScreen(void):
  Screen() {
  
  // go to phase 1
  init_phase_black_screen();
}

/**
 * Destroys the title screen.
 */
TitleScreen::~TitleScreen(void) {

  if (!screen_finished) {
    
    switch (current_phase) {
      
    case PHASE_ZS_PRESENTS:
      exit_phase_zs_presents();
      break;
      
    case PHASE_TITLE:
      exit_phase_title();
      break;

    default:
      break;
    }
  }
}

/**
 * Updates the title screen.
 */
void TitleScreen::update(void) {

  Uint32 now = SDL_GetTicks();

  switch (current_phase) {

  case PHASE_BLACK_SCREEN:

    if (now >= next_phase_date) {
      // go to next phase after 0.3 second
      init_phase_zs_presents();
    }

    break;

  case PHASE_ZS_PRESENTS:

    if (now >= next_phase_date) {

      if (transition_out->is_over()) {

	// unload current phase
	exit_phase_zs_presents();

	// go to next phase
	init_phase_title();
      }
      else if (!transition_out->is_started()) {
	transition_out->start();
      }
    }

    break;

  case PHASE_TITLE:

    if (transition_out->is_over()) {
      exit_phase_title();
      screen_finished = true;
      next_screen = NULL;
    }

    break;
  }
}

/**
 * Displays the title screen.
 */
void TitleScreen::display(SDL_Surface *screen_surface) {

  switch (current_phase) {

  case PHASE_BLACK_SCREEN:
    // nothing to display
    break;

  case PHASE_ZS_PRESENTS:
    if (transition_out->is_started()) { // out transition
      transition_out->display(img_zs_presents);
    }
    SDL_BlitSurface(img_zs_presents, NULL, screen_surface, &zs_presents_position);
    break;

  case PHASE_TITLE:
    if (transition_in->is_started()) {
      transition_in->display(img_title);
    }
    else if (transition_out->is_started()) {
      transition_out->display(img_title);
    }
    SDL_BlitSurface(img_title, NULL, screen_surface, NULL);
    break;
  }
}

/**
 * Handles an SDL event.
 * This function is called by the SDL main loop when there is an event.
 * @param event the SDL event to handle
 */
void TitleScreen::handle_event(const SDL_Event &event) {

  if (current_phase == PHASE_TITLE
      && event.type == SDL_KEYDOWN
      && event.key.keysym.sym == SDLK_SPACE
      && !transition_out->is_started()) {

    transition_out->start();
  }
}

/**
 * Initializes phase 1 of the title screen.
 * A black screen will be shown for a fraction of second.
 */
void TitleScreen::init_phase_black_screen(void) {

  // black screen during 0.3 second
  current_phase = PHASE_BLACK_SCREEN;
  next_phase_date = SDL_GetTicks() + 300;
}

/**
 * Initializes phase 2 of the title screen.
 * The message "Zelda Solarus presents" will be shown.
 */
void TitleScreen::init_phase_zs_presents(void) {

  current_phase = PHASE_ZS_PRESENTS;

  img_zs_presents = FileTools::open_image("menus/zelda_solarus_presents.png");
  zsdx->game_resource->get_sound("intro")->play();
  zs_presents_position.x = 112;
  zs_presents_position.y = 96;

  next_phase_date = SDL_GetTicks() + 2000; // intro: 2 seconds
  transition_out = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_OUT);
}

/**
 * Exits phase 2 of the title screen.
 */
 void TitleScreen::exit_phase_zs_presents(void) {
 
   SDL_FreeSurface(img_zs_presents);
   delete transition_out;
 }

/**
 * Initializes phase 3 of the title screen.
 * The title screen will be shown.
 */
void TitleScreen::init_phase_title(void) {

  current_phase = PHASE_TITLE;

  img_title = FileTools::open_image("menus/title.png");
  title_screen_music = zsdx->game_resource->get_music("title_screen_full.it");
  title_screen_music->play();

  transition_in = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_IN);
  transition_in->start();
  transition_out = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_OUT);
}

/**
 * Exits phase 3 of the title screen.
 */
 void TitleScreen::exit_phase_title(void) {

   delete transition_in;
   delete transition_out;
   SDL_FreeSurface(img_title);
   title_screen_music->stop();
 }
