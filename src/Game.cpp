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
#include "Color.h"
#include "HUD.h"
#include "Sound.h"
#include "KeysEffect.h"

/**
 * Creates a game.
 * @param savegame the saved data of this game
 */
Game::Game(Savegame *savegame):
  savegame(savegame), control_enabled(false), keys_effect(new KeysEffect()),
  current_map(NULL), next_map(NULL),
  transition_type(TRANSITION_IMMEDIATE), transition(NULL), hud(new HUD(savegame)),
  current_music_id(Music::none), current_music(NULL) {
  
}

/**
 * Destroys the game.
 */
Game::~Game(void) {
  delete keys_effect;
  delete hud;
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

  // initialize Link
  Link *link = zsdx->game_resource->get_link();
  link->initialize_sprites();

  // launch the starting map
  set_current_map(savegame->get_reserved_integer(SAVEGAME_STARTING_MAP),
		  savegame->get_reserved_integer(SAVEGAME_STARTING_ENTRANCE),
		  TRANSITION_FADE);
  
  // SDL main loop
  SDL_Event event;
  Uint32 ticks, last_frame_date = 0;
  SDL_EnableKeyRepeat(0, 0);

  while (!zsdx->is_exiting()) {

    handle_transitions();

    // handle the SDL events
    if (SDL_PollEvent(&event)) {

      zsdx->handle_event(event);

      if (is_control_enabled()) {
	switch (event.type) {
	  
	  // a key is pressed
	case SDL_KEYDOWN:

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
	    
	  case SDLK_c:
	    link->start_sword();
	    break;

	    // TODO remove
	  case SDLK_p:
	    savegame->set_reserved_integer(SAVEGAME_CURRENT_HEARTS, savegame->get_reserved_integer(SAVEGAME_CURRENT_HEARTS) + 2);
	    //	    savegame->set_reserved_integer(SAVEGAME_CURRENT_RUPEES, savegame->get_reserved_integer(SAVEGAME_CURRENT_RUPEES) + 23);
	    break;

	  case SDLK_m:
	    savegame->set_reserved_integer(SAVEGAME_CURRENT_HEARTS, savegame->get_reserved_integer(SAVEGAME_CURRENT_HEARTS) - 1);
	    //	    savegame->set_reserved_integer(SAVEGAME_CURRENT_RUPEES, savegame->get_reserved_integer(SAVEGAME_CURRENT_RUPEES) - 15);
	    break;

	    // TODO remove
	  case SDLK_o:
	    savegame->set_reserved_integer(SAVEGAME_CURRENT_RUPEES, savegame->get_reserved_integer(SAVEGAME_CURRENT_RUPEES) + 23);
	    break;

	  case SDLK_l:
	    savegame->set_reserved_integer(SAVEGAME_CURRENT_RUPEES, savegame->get_reserved_integer(SAVEGAME_CURRENT_RUPEES) - 15);
	    break;

	  case SDLK_s:
	    savegame->save();
	    break;

	  default:
	    break;
	  }
	  break;
	
	  // stop Link's movement
	case SDL_KEYUP:
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
	  break;
	}
      }
    }
    
    // update the sprites animations and positions
    current_map->update_sprites();

    // update the HUD
    hud->update();

    // update the sound system
    Sound::update();

    // display everything each time frame
    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + FRAME_INTERVAL) {
	
      last_frame_date = ticks;
      display_map(current_map);
    }
  }

  // quit the game
  current_map->leave(); // tell the map that Link is not there anymore
  stop_music();
}

/**
 * Handles the transitions.
 * This functions changes the map when needed and plays the
 * transitions between the two maps. This function is called
 * by the SDL main loop.
 */
void Game::handle_transitions(void) {

  // if the map has just changed, close the current map if any and play an out transition
  if (next_map != NULL && transition == NULL) { // the map has changed (e.g. set_current_map has been called)

    if (current_map == NULL) { // special case: no map was playing, so we don't have any out transition to do
      current_map = next_map;
      next_map = NULL;
    }
    else { // normal case: stop the control and play an out transition before leaving the current map
      set_control_enabled(false);
      transition = TransitionEffect::create_transition(transition_type, TRANSITION_OUT);
      transition->start();
    }
  }

  // if a transition was playing and has just been finished
  if (transition != NULL && transition->is_over()) {

    if (transition->get_direction() == TRANSITION_OUT) {
      // change the map
      current_map->leave();
      current_map->unload();
      current_map = next_map;
      next_map = NULL;
    }
    else {
      // let the player play
      set_control_enabled(true);
      zsdx->game_resource->get_link()->set_state(LINK_STATE_FREE);
    }

    delete transition;
    transition = NULL;
  }

  // if a map has just been set as the current map, start it and play the in transition
  if (!current_map->is_started()) {
    current_map->start();
    transition = TransitionEffect::create_transition(transition_type, TRANSITION_IN);
    transition->start();
  }
}

/**
 * Displays the specified map on the screen.
 * @param map the map to display
 */
void Game::display_map(Map *map) {
  SDL_FillRect(zsdx->screen, NULL, COLOR_BLACK);

  // display the map
  map->display();
  if (transition != NULL) {
    transition->display(map->get_visible_surface());
  }
  SDL_BlitSurface(map->get_visible_surface(), NULL, zsdx->screen, NULL);

  // display the hud
  hud->display(zsdx->screen);

  SDL_Flip(zsdx->screen);
}

/**
 * Changes the current map.
 * Call this function when you want Link to go to another map.
 * @param map_id id of the map to launch
 * @param entrance_index index of the entrance of the map you want to use
 * @param transition_type type of transition between the two maps
 */
void Game::set_current_map(MapId map_id, unsigned int entrance_index, TransitionType transition_type) {

  next_map = zsdx->game_resource->get_map(map_id);

  if (!next_map->is_loaded()) {
    next_map->load();
  }

  next_map->set_entrance(entrance_index);
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

  next_map = zsdx->game_resource->get_map(map_id);

  if (!next_map->is_loaded()) {
    next_map->load();
  }

  next_map->set_entrance(entrance_name);
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
      current_music_id = Music::none;
      current_music = NULL;
    }
    else {

      // play another music
      if (current_music != NULL) {
	current_music->stop();
      }

      Music *new_music = zsdx->game_resource->get_music(new_music_id);

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

   if (enable != this->control_enabled) {

     Link *link = zsdx->game_resource->get_link();
     link->set_moving_enabled(enable);

     this->control_enabled = enable;
   }
 }
