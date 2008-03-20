/**
 * This module shows the title screen of the game.
 */

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
bool TitleScreen::show(void) {

  bool quit = false;
  SDL_Event event;

  SDL_Surface *img_zs_presents = IMG_Load(FileTools::data_file_add_prefix("images/zelda_solarus_presents.png"));
  SDL_Surface *img_title = IMG_Load(FileTools::data_file_add_prefix("images/title.png"));

  // wait 0.3 second
  Uint32 start_intro_time = SDL_GetTicks() + 300;
  while (SDL_GetTicks() < start_intro_time && !quit) {

    SDL_PollEvent(&event);
    quit = zsdx->handle_event(event);
    if (quit) {
      return true;
    }

    SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);
    SDL_Flip(zsdx->screen);
  }

  // intro: "Zelda Solarus presents"
  zsdx->game_resource->get_sound("intro")->play();
  
  SDL_Rect position = {112, 96, 0, 0};

  Uint32 end_intro_time = SDL_GetTicks() + 2000; // intro: 2 seconds
  while (SDL_GetTicks() < end_intro_time && !quit) {
    
    SDL_PollEvent(&event);
    quit = zsdx->handle_event(event);
    if (quit) {
      return true;
    }

    SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);
    SDL_BlitSurface(img_zs_presents, NULL, zsdx->screen, &position);
    SDL_Flip(zsdx->screen);
  }
  SDL_FreeSurface(img_zs_presents);

  // title screen
  Music *title_screen_music = zsdx->game_resource->get_music("title_screen_full.it");
  Uint32 end_music_time = SDL_GetTicks() + 16000; // the music lasts 16 seconds
  title_screen_music->play();

  TransitionEffect *transition = TransitionEffect::create_transition(TRANSITION_FADE, TRANSITION_IN);
  transition->start();

  // wait until the user presses the space bar
  bool start = false;
  while (!start && !quit) {

    SDL_PollEvent(&event);
    quit = zsdx->handle_event(event);
    
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
      start = true;
    }
    
    SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);
    if (!transition->is_over()) {
      transition->display(img_title);
    }
    SDL_BlitSurface(img_title, NULL, zsdx->screen, NULL);
    SDL_Flip(zsdx->screen);

    if (SDL_GetTicks() >= end_music_time && title_screen_music->is_playing()) {
      title_screen_music->stop();
    }
  }
  delete transition;
  SDL_FreeSurface(img_title);

  // stop the title screen music
  if (title_screen_music->is_playing()) {
    title_screen_music->stop();
  }

  return quit;
}
