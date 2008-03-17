/**
 * This module shows the menu to select the save file and the game mode.
 */

#include "SelectionMenu.h"
#include "Savegame.h"
#include "ZSDX.h"
#include "Music.h"
#include "GameResource.h"

/**
 * Creates a selection menu.
 */
SelectionMenu::SelectionMenu(void):
  savegame(NULL), adventure_mode(true) {

}

/**
 * Destroys the selection menu.
 */
SelectionMenu::~SelectionMenu(void) {
  
}

/**
 * Shows the selection menu.
 * After calling this function, you can call
 * get_selected_save() and is_adventure_mode() to
 * know the user choice.
 */
void SelectionMenu::show(void) {

  // TODO
  SDL_Color white = {255, 255, 255};
  SDL_Surface *text = TTF_RenderText_Solid(zsdx->font, "Welcome to ZSDX!", white);
  SDL_Rect position = {100, 200, 0, 0};

  // play the selection menu music
  Music *music = zsdx->game_resource->get_music("game_over.it");

  music->play();



  bool quit = false;
  SDL_Event event;

  while (!quit) {

    SDL_PollEvent(&event);

    quit = zsdx->handle_event(event);
    
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
      savegame = new Savegame("save1.zsd");
      quit = true;
    }

    SDL_FillRect(zsdx->screen, NULL, 0);
    SDL_BlitSurface(text, NULL, zsdx->screen, &position);
    SDL_Flip(zsdx->screen);
  }

  // stop the music
  music->stop();
}

/**
 * Returns the savegame selected by the user.
 * The savegame returned is not freed when the menu is freed.
 * The show() function must have been called first.
 * @return the savegame selected by the user, or NULL
 * if he wants to quit.
 */
Savegame * SelectionMenu::get_selected_save(void) {
  return savegame;
}

/**
 * Returns whether the user has selected the Adventure mode
 * or the Solarus Dreams mode.
 * The show() function must have been called first and the
 * user should have selected a savegame, otherwise calling
 * this function doesn't make sense.
 * @return true if the user has selected the Adventure mode,
 * false if he has selected the Solarus Dreams mode 
 */
bool SelectionMenu::is_adventure_mode(void) {
  return adventure_mode;
}
