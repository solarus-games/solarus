/**
 * This module defines the class Game.
 */

#include "Game.h"
#include "ZSDX.h"
#include "Music.h"
#include "TransitionEffect.h"
#include "Link.h"
#include "Map.h"
#include "GameResource.h"
#include "Savegame.h"

/**
 * Creates a game.
 * @param savegame the saved data of this game
 */
Game::Game(Savegame *savegame):
  savegame(savegame), current_map(NULL),
  transition_type(TRANSITION_IMMEDIATE), transition(NULL),
  current_music_id(Music::none), current_music(NULL) {
  
}

/**
 * Destroys the game.
 */
Game::~Game(void) {
  delete savegame;
}

/**
 * Returns the saved data associated to this game.
 * @return the saved data
 */
Savegame * Game::get_savegame(void) {
  return savegame;
}

/**
 * Lets the user play.
 * The SDL main loop is executed here.
 */
void Game::play(void) {

  SDL_Event event;
  Uint32 ticks, last_frame_date = 0;
  Map *map  = NULL;
  Link *link = ZSDX::game_resource->get_link();
  bool quit = false;
  /*
  while (!quit) {
    
    // close the old map
    if (map != NULL) {
      transition = TransitionEffect::get_transition(transition_type, TRANSITION_OUT);
      transition->start();
      map->leave();
      map->unload();
    }
    
    map = current_map;
 
    // start the map (background music, etc)
    map->start();
    TransitionDisplayer::showTransition(map, transition, TRANSITION_IN);

    //  SDL_EnableKeyRepeat(5, 10);
*/
  // SDL main loop
  while (!quit) {

    // close the map if needed
    if (map != NULL && current_map != map) { // the map has changed (i.e. set_current_map has been called)
      transition = TransitionEffect::create_transition(transition_type, TRANSITION_OUT);
      transition->start();
    }

    // if a transition just finished
    if (transition != NULL && transition->is_over()) {

      if (transition->get_direction() == TRANSITION_OUT) {
	// change the map
	map->leave();
	map->unload();
	map = current_map;
      }
      else {
	set_control_enabled(true);
      }

      delete transition;
      transition = NULL;
    }

    // start the map if needed
    if (!current_map->is_started()) {
      current_map->start();
      set_control_enabled(false);
      transition = TransitionEffect::create_transition(transition_type, TRANSITION_IN);
      transition->start();

      map = current_map;
    }

    // handle the SDL events
    if (SDL_PollEvent(&event)) {

      quit = ZSDX::handle_event(event);

      switch (event.type) {
	
	// a key is pressed
      case SDL_KEYDOWN:

	if (is_control_enabled()) {
	  switch (event.key.keysym.sym) {
	    
	    // move Link
	  case SDLK_RIGHT:
	    link->start_right();
	    break;
	    
	  case SDLK_UP:
	    link->start_up();
	    break;
	    
	  case SDLK_LEFT:
	    link->start_left();
	    break;
	    
	  case SDLK_DOWN:
	    link->start_down();
	    break;

	  default:
	    break;
	  }
	}
	break;
	
	// stop Link's movement
      case SDL_KEYUP:
	if (is_control_enabled()) {
	  switch (event.key.keysym.sym) {
	    
	  case SDLK_RIGHT:
	    link->stop_right();
	    break;
	    
	  case SDLK_UP:
	    link->stop_up();
	    break;
	    
	  case SDLK_LEFT:
	    link->stop_left();
	    break;
	    
	  case SDLK_DOWN:
	    link->stop_down();
	    break;

	  default:
	    break;
	  }
	}
	break;
      }
    }
    
    // update the sprites animations and positions
    map->update_sprites();

    // display everything each time frame
    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + FRAME_INTERVAL) {
	
      last_frame_date = ticks;
      redraw_screen(map);
    }
  }

  // quit the game
  stop_music();
}

/**
 * Redraws the screen.
 */
void Game::redraw_screen(Map *map) {
  SDL_FillRect(ZSDX::screen, NULL, 0);

  if (transition != NULL) {
    transition->display(map->get_visible_surface());
  }

  map->display();
  SDL_BlitSurface(map->get_visible_surface(), NULL, ZSDX::screen, NULL); // TODO optimize

  // TODO rupees, hearts...

  SDL_Flip(ZSDX::screen);
}

/**
 * Changes the current map.
 * Call this function when you want Link to go to another map.
 * @param map_id id of the map to launch
 * @param entrance_index index of the entrance of the map you want to use
 * @param transition_type type of transition between the two maps
 */
void Game::set_current_map(MapId map_id, unsigned int entrance_index, TransitionType transition_type) {

  current_map = ZSDX::game_resource->get_map(map_id);

  if (!current_map->is_loaded()) {
    current_map->load();
  }

  current_map->set_entrance(entrance_index);
  this->transition_type = transition_type;
}

/**
 * Changes the current map.
 * Call this function when you want Link to go to another map.
 * @param map_id id of the map to launch
 * @param entrance_name name of the entrance of the map you want to use
 * @param transition_type type of transition between the two maps
 */
void Game::set_current_map(MapId map_id, string entrance_name, TransitionType transition_type) {

  current_map = ZSDX::game_resource->get_map(map_id);

  if (!current_map->is_loaded()) {
    current_map->load();
  }

  current_map->set_entrance(entrance_name);
  this->transition_type = transition_type;
}

/**
 * Plays a music. If the music is different from the current one,
 * the current one is stopped.
 * The music specified can also be Music::none_id (then the current music is just stopped)
 * or even Music::unchanged_id (nothing is done in this case).
 * @param new_music_id id of the music to play
 */
void Game::play_music(MusicId new_music_id) {
  
  if (!Music::isUnchangedId(new_music_id) && !Music::isEqualId(new_music_id, current_music_id)) {
    // the music is changed

    if (Music::isNoneId(new_music_id) && current_music != NULL) {
      
      current_music->stop();
    }
    else {

      // play another music
      if (current_music != NULL) {
	current_music->stop();
      }

      Music *new_music = ZSDX::game_resource->get_music(new_music_id);

      if (new_music->play()) {
	current_music_id = new_music_id;
	current_music = new_music;
      }
      else {
	current_music_id = Music::none;
	current_music = NULL;
      }
    }
  }
}

/**
 * Pauses or resumes the current music.
 * If no music is being played, nothing is done.
 */
void Game::pause_or_resume_music(void) {
  if (current_music != NULL) {
    current_music->set_paused(!current_music->is_paused());
  }
}

/**
 * Stops playing the current music.
 * If no music is being played, nothing is done.
 */
void Game::stop_music(void) {
  if (current_music != NULL) {
    current_music->stop();
  }
}

/**
 * Returns whether the player has control.
 * @return true if the player has control
 */
 bool Game::is_control_enabled(void) {
   return control_enabled;
 }

 /**
  * Sets whether the player has control.
  * @param true to enable the control
  */
 void Game::set_control_enabled(bool enable) {
   this->control_enabled = enable;
 }
