#include "menus/SelectionMenuChooseMode.h"
#include "TransitionEffect.h"
#include "FileTools.h"
#include "Savegame.h"
#include "Game.h"

/**
 * Creates a selection menu with the phase where the
 * player has to choose the game mode.
 * @param previous the previous phase
 */
SelectionMenuChooseMode::SelectionMenuChooseMode(SelectionMenuPhase *previous):
  SelectionMenuPhase(previous, "Choisissez un mode de jeu"),
  adventure_mode(true) {

  this->img_mode = FileTools::open_image("menus/selection_menu_mode.png");
  this->savegame_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
}

/**
 * Destructor.
 */
SelectionMenuChooseMode::~SelectionMenuChooseMode(void) {
  SDL_FreeSurface(img_mode);
  SDL_FreeSurface(savegame_surface);
  selection_menu_finished();
}

/**
 * Handles an SDL event in this phase.
 * @param event the event
 */
void SelectionMenuChooseMode::handle_event(const SDL_Event &event) {

  if (transition == NULL && event.type == SDL_KEYDOWN) {

    bool finished = false;

    switch (event.key.keysym.sym) {

    case SDLK_SPACE:
      if (adventure_mode) {
	// the user chose "Adventure"
	play_ok_sound();
	finished = true;
      }
      else {
	// the user chose "Solarus Dreams"
	play_ok_sound();
	finished = true;
      }
      break;

    case SDLK_RIGHT:
    case SDLK_LEFT:
      play_cursor_sound();
      adventure_mode = !adventure_mode;
      break;

    default:
      break;
    }

    if (finished) {
      transition = TransitionEffect::create(TRANSITION_FADE, TRANSITION_OUT);
      transition->start();
    }
  }
}

/**
 * Updates the selection menu in this phase.
 */
void SelectionMenuChooseMode::update(void) {

  if (transition != NULL && transition->is_over()) {

    Savegame *savegame = get_savegame(get_cursor_position() - 1);
    Savegame *savegame_copy = new Savegame(savegame->get_file_name());
    // because the first one will be deleted

    if (adventure_mode) {
      set_next_screen(new Game(savegame_copy));
    }
    else {
      set_next_screen(NULL);

      /* TODO :)

      Lorsqu'il lance ce mode, le jeu affiche les écrans suivants :
      
      1. Le joueur doit saisir son login et son mot de passe du forum.
      2. Le jeu se connecte au serveur et en cas de réussite, télécharge les derniers niveaux éventuellement ajoutés.
      3. Ecran de sélection des niveaux. La liste des niveaux s'affiche, avec pour chaque niveau : son nom, le nom de son créateur, la difficulté, les médailles obtenues par le joueur.
      4. Une fois un niveau choisi, un écran plus détaillé affiche toutes les informations du niveau, avec une description textuelle, un screenshot, et le nombre de joueurs ayant déjà réussi chaque objectif. Le joueur doit choisir un des objectifs et peut ensuite lancer le jeu.
      */
    }
  }

  SelectionMenuPhase::update();
}

/**
 * Displays the selection menu in this phase.
 */
void SelectionMenuChooseMode::display(SDL_Surface *screen_surface) {

  start_display(screen_surface);

  // the savegame
  int i = get_cursor_position() - 1;
  display_savegame(i, savegame_surface);
  display_savegame_number(i, savegame_surface);

  // move the savegame to the top
  SDL_Rect savegame_position;
  savegame_position.x = 57;
  savegame_position.y = 75 + i * 27;
  savegame_position.w = 208;
  savegame_position.h = 23;

  SDL_Rect position = {57, 75};

  SDL_BlitSurface(savegame_surface, &savegame_position, destination_surface, &position);

  // the two boxes
  SDL_Rect box_position = {0, 0, 73, 54};
  if (adventure_mode) {
    box_position.y = 54; // highlight the selected box
  }

  position.x = 70;
  position.y = 115;
  SDL_BlitSurface(img_mode, &box_position, destination_surface, &position);

  box_position.x = 73;
  box_position.y = adventure_mode ? 0 : 54; // highlight the selected box
  position.x = 170;
  position.y = 115;
  SDL_BlitSurface(img_mode, &box_position, destination_surface, &position);

  finish_display(screen_surface);
}
