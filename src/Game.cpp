/**
 * This module defines the class Game.
 */

#include <SDL/SDL.h>
#include "Types.h"
#include "maps/MapLinkHouse.h"
#include "Game.h"
#include "ZSDX.h"

/**
 * Constructor.
 */
Game::Game(void):
  current_map(NULL), current_music_id(MUSIC_NONE), current_music(NULL) {
  
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

  while (!quit) {
    
    // close the old map
    if (map != NULL) {
      map->leave();
      map->unload();
    }
    
    // load the new map
    map = current_map;
    map->load();
 
    // start the map (background music, etc)
    map->start();

    //  SDL_EnableKeyRepeat(5, 10);

    // SDL main loop
    while (this->current_map == map && !quit) { // loop until the map is changed
      if (SDL_PollEvent(&event)) {
	switch (event.type) {
	  
	  // quit if the user closes the window
	case SDL_QUIT:
	  quit = true;
	  break;
	
	  // a key is pressed
	case SDL_KEYDOWN:
	  switch (event.key.keysym.sym) {

	    // escape: quit
	  case SDLK_ESCAPE:
	    quit = true;
	    break;
	  
	    // F5: full screen / windowed mode
	  case SDLK_F5:
	    ZSDX::switch_fullscreen();
	    break;

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
	  
	    // space: pause the music
	  case SDLK_SPACE:
	    pause_or_resume_music();
	    break;

	    // temporary
	  case SDLK_F1:
	    set_current_map(MAP_LINKHOUSE);
	    break;

	    // temporary
	  case SDLK_F2:
	    set_current_map(MAP_LINKHOUSE, 1);
	    break;

	    // temporary
	  case SDLK_F3:
	    set_current_map(MAP_LINKHOUSESECRETROOM);
	    break;

	    // temporary
	  case SDLK_F4:
	    set_current_map(MAP_RUPEEHOUSE);
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

	  // check the obstacles
	  //     case SDL_MOUSEBUTTONDOWN:
	  // 	if (event.button.button == SDL_BUTTON_LEFT) {
	  // 	  int x = event.button.x;
	  // 	  int y = event.button.y;
	  // 	  Obstacle obstacle = obstacle_tiles[LAYER_LOW][width8*(y/8) + (x/8)];
	  // 	  cout << "obstacle: " << obstacle << "\n";
	  // 	}
	  // 	break;
	}
      }
    
      // update the sprites animations and positions
      map->update_sprites();

      // display everything each time frame
      ticks = SDL_GetTicks();
      if (ticks >= last_frame_date + FRAME_INTERVAL) {
	last_frame_date = ticks;
	map->display();
      }
    }
  }

  // quit the game
  stop_music();
}

/**
 * Changes the current map.
 * Call this function when you want Link to go to another map.
 * The map will be loaded with its first entrance.
 * @param map_id id of the map to launch
 */
void Game::set_current_map(MapID map_id) {

  set_current_map(map_id, 0);
}

/**
 * Changes the current map.
 * Call this function when you want Link to go to another map.
 * @param map_id id of the map to launch
 * @param entrance_index index of the entrance of the map you want to use
 */
void Game::set_current_map(MapID map_id, unsigned int entrance_index) {

  current_map = ZSDX::game_resource->get_map(map_id);
  current_map->set_entrance(entrance_index);
}

/**
 * Returns the music currently played.
 * @return the current music, or NULL is no music is being played
 */
// Music *Game::get_current_music(void) {
//   return current_music;
// }

/**
 * Plays a music. If the music is different from the current one,
 * the current one is stopped.
 * The music specified can also be MUSIC_NONE (then the current music is just stopped)
 * or even MUSIC_NO_CHANGE (nothing is done in this case).
 * @param new_music_id id of the music to play
 */
void Game::play_music(MusicID new_music_id) {
  
  if (new_music_id != MUSIC_NO_CHANGE && new_music_id != current_music_id) {
    // the music is changed

    if (new_music_id == MUSIC_NONE) {
      // stop the music
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
	current_music_id = MUSIC_NONE;
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
