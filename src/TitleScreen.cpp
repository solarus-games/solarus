/**
 * This module shows the title screen of the game.
 */

#include "TitleScreen.h"
#include "ZSDX.h"
#include "Music.h"
#include "GameResource.h"
#include "FileTools.h"

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

  // play the title screen music
  Music *title_screen_music = zsdx->game_resource->get_music("title_screen_full.it");
  Uint32 end_music_time = SDL_GetTicks() + 16000; // the music lasts 16 seconds

  title_screen_music->play();

  // show the title screen
  SDL_Surface *image = IMG_Load(FileTools::data_file_add_prefix("images/title.png"));

  // wait until the user presses the space bar
  bool quit = false;
  bool start = false;
  SDL_Event event;

  while (!start && !quit) {
    SDL_PollEvent(&event);

    quit = zsdx->handle_event(event);
    
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
      start = true;
    }
    
    SDL_BlitSurface(image, NULL, zsdx->screen, NULL);
    SDL_Flip(zsdx->screen);

    if (SDL_GetTicks() >= end_music_time && title_screen_music->is_playing()) {
      title_screen_music->stop();
    }
  }

  // stop the title screen music
  if (title_screen_music->is_playing()) {
    title_screen_music->stop();
  }
  SDL_FreeSurface(image);

  return quit;
}
