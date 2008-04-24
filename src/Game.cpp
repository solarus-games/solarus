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
#include "Equipment.h"
#include "AnimatedTile.h"

/**
 * Creates a game.
 * @param savegame the saved data of this game
 */
Game::Game(Savegame *savegame):
  savegame(savegame),
  paused(false), keys_effect(NULL),
  current_map(NULL), next_map(NULL),
  transition_type(TRANSITION_IMMEDIATE), transition(NULL), hud(NULL),
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
  link = zsdx->game_resource->get_link();
  link->initialize_sprites();

  // initialize the keys effect and the HUD
  keys_effect = new KeysEffect();
  update_keys_effect();
  hud = new HUD(this);

  // launch the starting map
  set_current_map(savegame->get_reserved_integer(SAVEGAME_STARTING_MAP),
		  savegame->get_reserved_integer(SAVEGAME_STARTING_ENTRANCE),
		  TRANSITION_FADE);

  // SDL main loop
  SDL_Event event;
  Uint32 ticks, last_frame_date = 0;
  SDL_EnableKeyRepeat(0, 0);

  while (!zsdx->is_exiting()) {

    // update the transitions between maps
    update_transitions();

    // handle the SDL events
    if (SDL_PollEvent(&event)) {

      zsdx->handle_event(event);

      switch (event.type) {

	// a key is pressed
      case SDL_KEYDOWN:
	
	switch (event.key.keysym.unicode) {
	  
	case SDLK_c:
	  
	  switch (keys_effect->get_sword_key_effect()) {
	    
	  case SWORD_KEY_SWORD:
	    if (!is_suspended()) {
	      link->start_sword();
	    }
	    break;
	    
	  default:
	    break;
	  }
	  break;

	  // TODO remove
	case SDLK_p:
	  savegame->get_equipment()->add_hearts(2);
	  break;
	  
	case SDLK_m:
	  savegame->get_equipment()->remove_hearts(1);
	  break;
	  
	case SDLK_o:
	  savegame->get_equipment()->add_rupees(23);
	  break;
	  
	case SDLK_l:
	  savegame->get_equipment()->remove_rupees(14);
	  break;
	  
	case SDLK_i:
	  savegame->get_equipment()->add_magic(10);
	  break;
	  
	case SDLK_k:
	  savegame->get_equipment()->remove_magic(4);
	  break;
	  
	case SDLK_j:
	  if (!savegame->get_equipment()->is_magic_decreasing()) {
	    savegame->get_equipment()->start_removing_magic(200);
	  }
	  else {
	    savegame->get_equipment()->stop_removing_magic();
	  }
	  break;
	  
	case SDLK_s:
	  savegame->save();
	  break;
	  
	case SDLK_d:
	  
	  // temoporary code to make like the game is paused
	  if (!is_paused()) {
	    
	    zsdx->game_resource->get_sound("pause_open")->play();
	    keys_effect->set_pause_key_effect(PAUSE_KEY_RETURN);
	    keys_effect->set_sword_key_effect(SWORD_KEY_SAVE);
	    paused = true;
	  }
	  else {
	    zsdx->game_resource->get_sound("pause_closed")->play();
	    keys_effect->set_pause_key_effect(PAUSE_KEY_PAUSE);
	    keys_effect->set_sword_key_effect(SWORD_KEY_SWORD);
	    paused = false;
	  }
	  break;

	case SDLK_a:
	  
	  if (keys_effect->get_action_key_effect() == ACTION_KEY_NONE) {
	    keys_effect->set_action_key_effect(ACTION_KEY_ACTION);
	  }
	  else if (keys_effect->get_action_key_effect() == ACTION_KEY_ACTION) {
	    keys_effect->set_action_key_effect(ACTION_KEY_THROW);
	  }
	  else if (keys_effect->get_action_key_effect() == ACTION_KEY_THROW) {
	    keys_effect->set_action_key_effect(ACTION_KEY_NONE);
	  }	    
	  break;
	  
	case SDLK_t:
	  if (!is_suspended()) {
	    if (link->get_state() != LINK_STATE_PUSHING) {
	      link->start_pushing();
	    }
	    else {
	      link->start_free();
	    }
	  }
	  break;

	default:
	  break;
	}

	if (!is_suspended()) {
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
		
	// TODO remove
	switch (event.key.keysym.sym) {
	  
	case SDLK_KP7:
	  savegame->get_equipment()->set_max_magic(0);
	  break;
	  
	case SDLK_KP8:
	  savegame->get_equipment()->set_max_magic(42);
	  break;
	  
	case SDLK_KP9:
	  savegame->get_equipment()->set_max_magic(84);
	  break;
	  
	case SDLK_KP1:
	  savegame->get_equipment()->set_tunic_number(MAX(savegame->get_equipment()->get_tunic_number() - 1, 0));
	  break;

	case SDLK_KP4:
	  savegame->get_equipment()->set_tunic_number(MIN(savegame->get_equipment()->get_tunic_number() + 1, 2));
	  break;
	  
	case SDLK_KP2:
	  savegame->get_equipment()->set_sword_number(MAX(savegame->get_equipment()->get_sword_number() - 1, 0));
	  break;
	  
	case SDLK_KP5:
	  savegame->get_equipment()->set_sword_number(MIN(savegame->get_equipment()->get_sword_number() + 1, 4));
	  break;
	  
	case SDLK_KP3:
	  savegame->get_equipment()->set_shield_number(MAX(savegame->get_equipment()->get_shield_number() - 1, 0));
	  break;
	  
	case SDLK_KP6:
	  savegame->get_equipment()->set_shield_number(MIN(savegame->get_equipment()->get_shield_number() + 1, 3));
	  break;
	  
	default:
	  break;
	}
	break;
	
	// stop Link's movement
      case SDL_KEYUP:
	if (!is_suspended()) {
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
      }
    }
    
    // update the entity's positions and animations
    AnimatedTile::update();
    current_map->update_entities();

    // update the equipment and HUD
    savegame->get_equipment()->update();
    update_keys_effect();
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
void Game::update_transitions(void) {

  // if the map has just changed, close the current map if any and play an out transition
  if (next_map != NULL && transition == NULL) { // the map has changed (e.g. set_current_map has been called)

    if (current_map == NULL) { // special case: no map was playing, so we don't have any out transition to do
      current_map = next_map;
      next_map = NULL;
    }
    else { // normal case: stop the control and play an out transition before leaving the current map
      link->restart_animation();
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
 * Returns whether the game is paused.
 * @return true if the game is paused
 */
bool Game::is_paused(void) {
  return paused;
}

/**
 * Returns whether the game is suspended.
 * This is true in the following cases:
 * - the game is paused,
 * - a message a being dispayed,
 * - a transition between two maps is playing.
 * @return true if the game is suspended
 */
bool Game::is_suspended(void) {
  return is_paused() || transition != NULL;
}

/**
 * Returns the current effect of the 3 main keys: action, sword and pause.
 * @return the current effect of the 3 main keys: action, sword and pause
 */
KeysEffect * Game::get_keys_effect(void) {
  return keys_effect;
}

/**
 * Makes sure the keys effects are coherent with Link's equipment and abilities.
 */
void Game::update_keys_effect(void) {

  if (!is_suspended()) {
    switch (link->get_state()) {

    case LINK_STATE_FREE:
    case LINK_STATE_SWORD_SWINGING:
    case LINK_STATE_SWORD_LOADING:

      // the sword key swings the sword <=> Link has a sword
      if (savegame->get_equipment()->has_sword()
	  && keys_effect->get_sword_key_effect() != SWORD_KEY_SWORD) {

	keys_effect->set_sword_key_effect(SWORD_KEY_SWORD);
      }
      else if (!savegame->get_equipment()->has_sword()
	       && keys_effect->get_sword_key_effect() == SWORD_KEY_SWORD) {
      
	keys_effect->set_sword_key_effect(SWORD_KEY_NONE);
      }
      break;

    default:
      break;
    }
  }
}
