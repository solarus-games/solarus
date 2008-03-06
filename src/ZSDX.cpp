/**
 * This module initializes the game and handles the screen.
 */

#include "ZSDX.h"
#include "FileTools.h"
#include "Color.h"
#include "Music.h"
#include "Game.h"
#include "GameResource.h"
#include "Savegame.h"

/**
 * Possible choices in the main menu.
 */
enum MainMenuChoice {
  MAIN_MENU_ADVENTURE,
  MAIN_MENU_SOLARUS_DREAMS,
  MAIN_MENU_QUIT
};

/**
 * Number of times the screen is redrawn is a second.
 */
const int FRAMES_PER_SECOND = 50;

/**
 * Delay between two frames in millisecond.
 */
const int FRAME_INTERVAL = 1000 / FRAMES_PER_SECOND;

/**
 * The game.
 */
Game *ZSDX::game;

/**
 * The screen.
 */
SDL_Surface *ZSDX::screen;

/**
 * True if we are in full screen mode.
 */
bool ZSDX::fullscreen;

/**
 * The tile animation manager object.
 */
TileAnimationManager ZSDX::tile_animation_manager;

/**
 * The game resource object.
 * It contains the whole game database: the maps, the tilesets,
 * the sprites, the musics, etc.
 */
GameResource *ZSDX::game_resource;


/**
 * Initializes the game engine.
 */
void ZSDX::initialize(void) {

  // initialise SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

  //  SDL_ShowCursor(SDL_DISABLE);
  SDL_ShowCursor(SDL_ENABLE);
  set_fullscreen(false);
 
  color_init();

  // set the timer
  SDL_AddTimer(250, TileAnimationManager::increment_frame_counter, NULL);

  // initialize FMOD
  Music::initialize();

  // initialize the game resource
  game_resource = new GameResource();
}

/**
 * Cleans everything.
 */
void ZSDX::exit(void) {
  SDL_Quit();
  Music::exit();
  delete game_resource;
}

/**
 * Sets the full screen mode or the windowed mode.
 * @param fullscreen true for full screen mode, false for the windowed mode
 */
void ZSDX::set_fullscreen(bool fullscreen) {
  if (fullscreen) {
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  }
  else {
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
  }
  ZSDX::fullscreen = fullscreen;
}

/**
 * Returns whether the game is in full screen.
 * @return true if the game is in full screen mode, false otherwise
 */
bool ZSDX::is_fullscreen(void) {
  return fullscreen;
}

/**
 * Switches between full screen mode and windowed mode.
 */
void ZSDX::switch_fullscreen(void) {
  set_fullscreen(!fullscreen);
}

/**
 * Launches the game.
 */
void ZSDX::main(void) {

  // initialize the game engine
  initialize();

  bool quit = false;
  while (!quit) {
    
    // title screen
    show_title_screen();
    
    // choose the game mode: Adventure or Solarus Dreams
    quit = show_main_menu();
  }

  // close the game engine
  exit();
}

/**
 * Shows the title screen.
 */
void ZSDX::show_title_screen(void) {
  // TODO
}

/**
 * Shows the main menu.
 * The player can choose Adventure, Solarus Dreams or Quit.
 * @return true if the user wants to quit ZSDX.
 */
bool ZSDX::show_main_menu(void) {
  bool quit = false;

  // TODO
  MainMenuChoice choice = MAIN_MENU_ADVENTURE;

  switch (choice) {
    
  case MAIN_MENU_ADVENTURE:
    launch_adventure_mode();
    quit = true; // TODO: remove this (temporary because we should display a menu to let the user choose to continue or quit)
    break;
    
  case MAIN_MENU_SOLARUS_DREAMS:
    launch_solarus_dreams_mode();
    break;
    
  case MAIN_MENU_QUIT:
    quit = true;
    break;
  }

  return quit;
}

/**
 * Launches the adventure mode: shows the game file selection screen
 * and starts the game with the selected file.
 */
void ZSDX::launch_adventure_mode(void) {

  // choose the game file (1, 2 or 3)
  game = NULL;
  show_game_file_selection();
  
  // if the user chose a savegame (i.e. he didn't cancelled)
  if (game != NULL) {
    
    // launch the starting map
    game->set_current_map(game->get_savegame()->get_starting_map(),
			  game->get_savegame()->get_starting_entrance(),
			  TRANSITION_IMMEDIATE);
    
    game->play();

    delete game;
    game = NULL;
  }
}

/**
 * Shows the selection menu of the savegames.
 * The player can choose an existing game file
 * or create a new one.
 * After this 
 */
void ZSDX::show_game_file_selection(void) {

  // TODO: selection menu

  char game_file_name[32] = "save1.zsd";
  char player_name[32] = "Link";

  Savegame *savegame = new Savegame(game_file_name);
  savegame->set_player_name(player_name);

  // create the game
  game = new Game(savegame);
}

/**
 * Launches the Solarus Dreams mode.
 */
void ZSDX::launch_solarus_dreams_mode(void) {

  /* TODO: make another class to display the menus of this mode

Lorsqu'il lance ce mode, le jeu affiche les écrans suivants :

1. Le joueur doit saisir son login et son mot de passe du forum.
2. Le jeu se connecte au serveur et en cas de réussite, télécharge les derniers niveaux éventuellement ajoutés.
3. Ecran de sélection des niveaux. La liste des niveaux s'affiche, avec pour chaque niveau : son nom, le nom de son créateur, la difficulté, les médailles obtenues par le joueur.
4. Une fois un niveau choisi, un écran plus détaillé affiche toutes les informations du niveau, avec une description textuelle, un screenshot, et le nombre de joueurs ayant déjà réussi chaque objectif. Le joueur doit choisir un des objectifs et peut ensuite lancer le jeu.
  */
}

/**
 * Entry point of the program.
 */
int main(int argc, char **argv) {

  ZSDX::main();
  
  return 0;
}
