/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Game.h"
#include "ZSDX.h"
#include "Map.h"
#include "MapScript.h"
#include "ResourceManager.h"
#include "Savegame.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "Dungeon.h"
#include "GameoverSequence.h"
#include "hud/HUD.h"
#include "menus/TitleScreen.h"
#include "menus/PauseMenu.h"
#include "entities/Hero.h"
#include "entities/Tileset.h"
#include "entities/Detector.h"
#include "lowlevel/Music.h"
#include "lowlevel/Color.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Surface.h"

const Rectangle Game::outside_world_size(0, 0, 2080, 3584); // TODO load from external file

/**
 * Creates a game.
 * @param savegame the saved data of this game
 */
Game::Game(Savegame *savegame):
  savegame(savegame),
  pause_enabled(true), pause_menu(NULL), 
  dialog_box(NULL), treasure(NULL), gameover_sequence(NULL),
  reseting(false), restarting(false), keys_effect(NULL),
  current_map(NULL), next_map(NULL), previous_map_surface(NULL),
  transition_style(Transition::IMMEDIATE), transition(NULL),
  dungeon(NULL), crystal_switch_state(false), hud(NULL), hud_enabled(true),
  current_music_id(Music::none), current_music(NULL), previous_music_id(Music::none) {

  zsdx->set_game(this);
  controls = new Controls(this);

  // initialize the hero
  hero = new Hero(get_equipment());

  // initialize the keys effect and the HUD
  keys_effect = new KeysEffect();
  update_keys_effect();
  hud = new HUD(this);

  // launch the starting map
  set_current_map(savegame->get_integer(Savegame::STARTING_MAP),
		  "", Transition::FADE);
}

/**
 * Destroys the game.
 */
Game::~Game(void) {

  // quit the game
  current_map->leave(); // tell the map that the hero is not there anymore
  stop_music();

  delete transition;
  delete dialog_box;
  delete pause_menu;
  delete treasure;
  delete dungeon;
  delete gameover_sequence;

  delete keys_effect;
  delete hud;
  delete hero;
  delete savegame;
  delete controls;

  if (previous_map_surface != NULL) {
    delete previous_map_surface;
  }

  if (zsdx->game == this) {
    zsdx->set_game(NULL);
  }
}

/**
 * Returns the hero.
 * @return the hero
 */
Hero * Game::get_hero(void) {
  return hero;
}

/**
 * Returns the coordinates of the hero on the current map.
 * The coordinates returned are the coordinates of the hero's origin point
 * on the map.
 * The width and the height are not used.
 * @return the position of the hero
 */
const Rectangle & Game::get_hero_xy(void) {

  static Rectangle xy;
  xy.set_xy(hero->get_xy());
  return xy;
}

/**
 * Returns the game controls for the keyboard and the joypad.
 * @return the game controls
 */
Controls * Game::get_controls(void) {
  return controls;
}

/**
 * Returns the current effect of the main keys (action, sword, pause, etc.).
 * @return the current effect of the main keys
 */
KeysEffect * Game::get_keys_effect(void) {
  return keys_effect;
}

/**
 * Returns the saved data associated to this game.
 * @return the saved data
 */
Savegame * Game::get_savegame(void) {
  return savegame;
}

/**
 * Returns the equipment of the player.
 * It just calls get_savegame()->get_equipment().
 * @return the equipment
 */
Equipment * Game::get_equipment(void) {
  return savegame->get_equipment();
}

/**
 * Returns the dungeon equipment of the player.
 * It just calls get_savegame()->get_dungeon_equipment().
 * @return the dungeon equipment
 */
DungeonEquipment * Game::get_dungeon_equipment(void) {
  return savegame->get_dungeon_equipment();
}

/**
 * Returns the script of the current map.
 * @return the script of the current map, or NULL if no map is loaded
 */
MapScript * Game::get_current_script(void) {

  if (get_current_map() != NULL) {
    return get_current_map()->get_script();
  }

  return NULL;
}

/**
 * This function is called by the main loop
 * when an event occurs during the game.
 */
void Game::handle_event(const SDL_Event &event) {

  // the only events we are interested in are the keyboard and joypad events
  controls->handle_event(event);
}

/**
 * This function is called when a game key is pressed.
 * @param key a key
 */
void Game::key_pressed(Controls::GameKey key) {

  if (!is_suspended()) {    

    if (key == Controls::PAUSE) {
      if (is_pause_enabled()) {
        set_paused(true);
      }
    }
    else {
      // when the game is not suspended, most of the keys apply to the hero
      hero->key_pressed(key);
    }
  }

  // is a message being shown?
  else if (is_showing_message()) {
    dialog_box->key_pressed(key);
  }

  // is the game paused?
  else if (is_paused()) {
    pause_menu->key_pressed(key);
  }

  // is the game over sequence shown?
  else if (is_showing_gameover()) {
    gameover_sequence->key_pressed(key);
  }
}

/**
 * This function is called when a game key is released.
 * @param key a key
 */
void Game::key_released(Controls::GameKey key) {

  if (!is_suspended()) {
    // if the game is not suspended, the keys apply to the hero
    hero->key_released(key);
  }
}

/**
 * Updates the game elements : the map, the equipment, the HUD, the sound system, etc.
 */
void Game::update(void) {

  // update the transitions between maps
  update_transitions();

  if (reseting || restarting) {
    return; // the game may have just been reset
  }

  // update the map
  current_map->update();

  // update the equipment and HUD
  get_equipment()->update();
  update_keys_effect();
  hud->update();

  // update the treasure (if any)
  if (treasure != NULL) {
    update_treasure();
  }

  // update the pause menu (if the game is paused)
  if (is_paused()) {
    pause_menu->update();
  }

  // update the dialog box (if any)
  if (is_showing_message()) {
    update_dialog_box();
  }

  // update the game over sequence (if any)
  if (is_showing_gameover()) {
    update_gameover_sequence();
  }
}

/**
 * Handles the transitions.
 * This functions changes the map when needed and plays the
 * transitions between the two maps. This function is called
 * by the update() function.
 * Note that the two maps can actually be the same.
 */
void Game::update_transitions(void) {

  // if the map has just changed, close the current map if any and play an out transition
  if (next_map != NULL && transition == NULL) { // the map has changed (i.e. set_current_map has been called)

    if (current_map == NULL) { // special case: no map was playing, so we don't have any out transition to do
      load_dungeon();
      current_map = next_map;
      next_map = NULL;
    }
    else { // normal case: stop the control and play an out transition before leaving the current map
      transition = Transition::create(transition_style, Transition::OUT);
      transition->start();
    }
  }

  // if a transition was playing and has just been finished
  if (transition != NULL && transition->is_over()) {

    Transition::Direction transition_direction = transition->get_direction();
    bool needs_previous_surface = transition->needs_previous_surface();
    delete transition;
    transition = NULL;

    if (reseting) {
      current_map->unload();
      set_next_screen(new TitleScreen());
    }
    else if (restarting) {
      current_map->unload();
      set_next_screen(new Game(new Savegame(savegame)));
    }
    else if (transition_direction == Transition::OUT) {

      if (next_map == current_map) {
	hero->place_on_destination_point(current_map);
	transition = Transition::create(transition_style, Transition::IN);
	transition->start();
	next_map = NULL;
      }
      else {

	// change the map
	current_map->leave();

	// special treatments for an inside/outside transition
	if ((current_map->is_in_outside_world() && !next_map->is_in_outside_world())
	    || (!current_map->is_in_outside_world() && next_map->is_in_outside_world())) {

	  // reset the crystal switch blocks
	  crystal_switch_state = false;

	  // save the location
	  savegame->set_integer(Savegame::STARTING_MAP, next_map->get_id());
	  savegame->set_string(Savegame::STARTING_POINT, next_map->get_destination_point_name());

          if (next_map->is_in_dungeon()) {
            // show the dungeon name
	    std::ostringstream oss;
	    oss << "dungeon_" << next_map->get_world_number();
	    next_map->set_welcome_message(oss.str());
	  }
	}

	// before closing the map, draw it on a backup surface for transition effects that display two maps
	if (needs_previous_surface) {
	  previous_map_surface = new Surface(320, 240);
	  current_map->display();
	  current_map->get_visible_surface()->blit(previous_map_surface);
	}

	// unload the previous tileset if the new map uses another one
	Tileset *old_tileset = current_map->get_tileset();
	Tileset *new_tileset = next_map->get_tileset();
	if (new_tileset != old_tileset) {
	  old_tileset->unload();
	}

	// set the next map
	load_dungeon();
	current_map->unload();
	current_map = next_map;
	next_map = NULL;
      }
    }
    else {
      current_map->opening_transition_finished();

      if (previous_map_surface != NULL) {
	delete previous_map_surface;
	previous_map_surface = NULL;
      }
    }
  }

  // if a map has just been set as the current map, start it and play the in transition
  if (!current_map->is_started()) {
    transition = Transition::create(transition_style, Transition::IN);

    if (previous_map_surface != NULL) {
      // some transition effects need to display both maps simultaneously
      transition->set_previous_surface(previous_map_surface);
    }

    hero->place_on_destination_point(current_map);
    transition->start();
    current_map->start();
  }
}

/**
 * Makes sure the keys effects are coherent with the hero's equipment and abilities.
 */
void Game::update_keys_effect(void) {

  if (is_paused() || is_showing_message()) {
    return;
  }

  switch (hero->get_state()) {
    
  case Hero::FREE:
  case Hero::SWORD_SWINGING:
  case Hero::SWORD_LOADING:

    // the sword key swings the sword <=> the hero has a sword
    if (get_equipment()->has_sword()
	&& keys_effect->get_sword_key_effect() != KeysEffect::SWORD_KEY_SWORD) {

      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SWORD);
    }
    else if (!get_equipment()->has_sword()
	     && keys_effect->get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD) {
      
      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_NONE);
    }
    break;

  default:
    break;
  }
}

/**
 * Updates the dialog box.
 * This function is called repeatedly while a dialog box is shown.
 */
void Game::update_dialog_box(void) {

  if (!dialog_box->is_finished()) {
    dialog_box->update();
  }
  else {

    std::string first_message_id = dialog_box->get_first_message_id();
    bool skipped = dialog_box->was_skipped();

    delete dialog_box;
    dialog_box = NULL;

    if (!skipped && first_message_id[0] != '_') {
      // a dialog of the quest was just finished: notify the script
      get_current_script()->event_message_sequence_finished(first_message_id, get_dialog_last_answer());
    }
  }
}

/**
 * Updates the treasure.
 * This function is called repeatedly while a treasure is being given.
 */
void Game::update_treasure(void) {

  if (treasure != NULL && !is_showing_message()) {

    // the game has finished giving the treasure to the player
    // and displaying the corresponding message

    Treasure::Content content = treasure->get_content();
    int savegame_variable = treasure->get_savegame_variable();

    delete treasure;
    treasure = NULL;

    get_current_script()->event_obtained_treasure(content, savegame_variable);
  }
}

/**
 * Updates the game over sequence.
 * This function is called repeatedly while the game over sequence is shown.
 */
void Game::update_gameover_sequence(void) {

  if (!gameover_sequence->is_finished()) {
    gameover_sequence->update();
  }
  else {
    delete gameover_sequence;
    gameover_sequence = NULL;
  }
}

/**
 * Displays the game.
 * @param screen_surface the surface where the game will be displayed
 */
void Game::display(Surface *screen_surface) {

  // display the map
  current_map->display();
  if (transition != NULL) {
    transition->display(current_map->get_visible_surface());
  }
  current_map->get_visible_surface()->blit(screen_surface);

  // display the pause screen if any
  if (is_paused()) {
    pause_menu->display(screen_surface);
  }

  // display the game over sequence if any
  else if (is_showing_gameover()) {
    gameover_sequence->display(screen_surface);
  }

  // display the hud
  if (hud_enabled) {
    hud->display(screen_surface);
  }

  // display the dialog box if any
  if (is_showing_message()) {
    dialog_box->display(screen_surface);
  }
}

/**
 * Returns the current map.
 * @return the current map
 */
Map * Game::get_current_map(void) {
  return current_map;
}

/**
 * Changes the current map.
 * Call this function when you want the hero to go to another map.
 * @param map_id id of the map to launch
 * @param destination_point_name name of the destination point of the map you want to use,
 * or en ampty string to pick the destination point saved
 * @param transition_style type of transition between the two maps
 */
void Game::set_current_map(MapId map_id, const std::string &destination_point_name,
			   Transition::Style transition_style) {

  hero->reset_movement(); // stop the hero

  // load the next map
  next_map = ResourceManager::get_map(map_id);
  if (!next_map->is_loaded()) {
    next_map->load(this);
  }

  // initialize the destination point, from the specified name or from the savegame
  if (destination_point_name == "") {
    next_map->set_destination_point(savegame->get_string(Savegame::STARTING_POINT));
  }
  else {
    next_map->set_destination_point(destination_point_name);
  }
  this->transition_style = transition_style;
}

/**
 * Returns the size of the oustide world in pixels.
 * @return the size of the oustide world
 */
const Rectangle & Game::get_outside_world_size(void) {
  return outside_world_size;
}

/**
 * Returns whether the current map belongs to a dungeon.
 * @return true if the current map is in a dungeon
 */
bool Game::is_in_dungeon(void) {
  return current_map->is_in_dungeon();
}

/**
 * Returns the dungeon where the current map is, or NULL
 * if we are not in a dungeon.
 * @return the current dungeon
 */
Dungeon * Game::get_current_dungeon(void) {
  return dungeon;
}

/**
 * Returns false if the orange blocks are lowered
 * or true if the blue blocks are lowered.
 * @return the state of the crystal switchs or this world
 */
bool Game::get_crystal_switch_state(void) {
  return crystal_switch_state;
}

/**
 * Changes the state of the blue and orange blocks.
 */
void Game::change_crystal_switch_state(void) {
  crystal_switch_state = !crystal_switch_state;
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

    previous_music_id = current_music_id; // save the previous music

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

      Music *new_music = ResourceManager::get_music(new_music_id);

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
  play_music(Music::none);
}

/**
 * Plays the music that was playing before the last music change.
 */
void Game::restore_music(void) {
  play_music(previous_music_id);
}

/**
 * Returns the id of the music currently played.
 * @return the current music
 */
const MusicId& Game::get_current_music_id(void) {
  return current_music_id;
}

/**
 * Returns whether the game is paused.
 * @return true if the game is paused
 */
bool Game::is_paused(void) {
  return pause_menu != NULL;
}

/**
 * Returns whether we are showing a message.
 * @return true if a message is being shown.
 */
bool Game::is_showing_message(void) {
  return dialog_box != NULL;
}

/**
 * Returns whether we are playing a transition between two maps.
 * @return true if there is a transition
 */
bool Game::is_playing_transition(void) {
  return transition != NULL || next_map != NULL;
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
  return current_map == NULL || is_paused() || is_showing_message() ||
    is_playing_transition() || is_showing_gameover() || !current_map->is_camera_fixed_on_hero();
}

/**
 * Returns the dialog box currently displayed.
 * @return the dialog box, or NULL if no message is currently displayed
 */
DialogBox * Game::get_dialog_box(void) {
  return dialog_box;
}

/**
 * Shows the specified message.
 * If this message is followed by other messages, they will
 * be displayed too.
 * The dialog box y position depends on the hero's position on the screen.
 * @param message_id id of the message to show
 */
void Game::show_message(const MessageId &message_id) {

  const Rectangle &camera_position = current_map->get_camera_position();

  if (hero->get_y() < camera_position.get_y() + 130) {
    show_message(message_id, 1);
  }
  else {
    show_message(message_id, 0);
  }
}

/**
 * Shows the specified message.
 * If this message is followed by other messages, they will
 * be displayed too.
 * @param message_id id of the message to show
 * @param position the dialog box position: 0 means on the top, 1 means on the bottom
 */
void Game::show_message(const MessageId &message_id, int position) {

  int y;
  if (position == 0) {
    y = 32;
  }
  else {
    y = 144;
  }

  dialog_box = new DialogBox(message_id, 50, y);
}

/**
 * Returns the answer selected by the player in the last dialog with a question.
 * @return the answer selected: 0 for the first one, 1 for the second one,
 * -1 if the last dialog was not a question
 */
int Game::get_dialog_last_answer(void) {
  return dialog_last_answer;
}

/**
 * Remembers the answer selected by the player in the last dialog with a question.
 * @param answer the answer selected: 0 for the first one, 1 for the second one,
 * -1 if the last dialog was not a question
 */
void Game::set_dialog_last_answer(int answer) {

  if (answer < -1 || answer > 1) {
    DIE("Invalid value of answer: " << answer);
  }

  this->dialog_last_answer = answer;
}

/**
 * Gives a treasure to the player.
 * Makes the hero brandish the treasure and shows a message.
 * @param treasure the treasure to give (will be deleted after the hero brandishes it) 
 */
void Game::give_treasure(Treasure *treasure) {

  this->treasure = treasure;

  // brandish the treasure
  hero->give_treasure(treasure);

  // give the treasure and show the message
  treasure->give_to_player();
}

/**
 * Returns whether a treasure is being given to the player.
 * @return true if a treasure is being given to the player.
 */
bool Game::is_giving_treasure(void) {
  return treasure != NULL && is_showing_message();
}

/**
 * Sets whether the HUD is currently displayed.
 * @param hud_enabled true to make the HUD displayed
 */
void Game::set_hud_enabled(bool hud_enabled) {
  this->hud_enabled = hud_enabled;
}

/**
 * Returns whether the player can pause the game.
 * @return true if the player is currently allowed to pause the game
 */
bool Game::is_pause_enabled(void) {
  return pause_enabled;
}

/**
 * Sets whether the player can pause the game.
 * @param pause_enabled true to allow the player to pause the game
 */
void Game::set_pause_enabled(bool pause_enabled) {
  this->pause_enabled = pause_enabled;
  keys_effect->set_pause_key_enabled(pause_enabled);
}

/**
 * Pauses or resumes the game.
 * @param paused true to pause the game, false to resume it.
 */
void Game::set_paused(bool paused) {

  if (paused != is_paused()) {

    if (paused) {
      pause_menu = new PauseMenu(this);
    }
    else {
      delete pause_menu;
      pause_menu = NULL;
    }
  }
}

/**
 * Returns the pause menu.
 * @return the pause menu, or NULL if the game is not paused
 */
PauseMenu * Game::get_pause_menu(void) {
  return pause_menu;
}

/**
 * Sets the dungeon field depending on the current map.
 * This function is called when the map changes.
 */
void Game::load_dungeon(void) {

  if (current_map == NULL || next_map->get_world_number() != current_map->get_world_number()) {

    if (current_map != NULL && current_map->is_in_dungeon()) {
      delete dungeon;
      dungeon = NULL;
    }

    if (next_map->is_in_dungeon()) {
      dungeon = new Dungeon(next_map->get_world_number());
    }
  }
}

/**
 * Go back to the title screen.
 */
void Game::reset(void) {

  transition = Transition::create(Transition::FADE, Transition::OUT);
  transition->start();
  reseting = true;
}

/**
 * Restarts the game with the current savegame state.
 */
void Game::restart(void) {

  transition = Transition::create(Transition::FADE, Transition::OUT);
  transition->start();
  restarting = true;
}

/**
 * Launches the gameover sequence.
 */
void Game::start_gameover_sequence(void) {
  gameover_sequence = new GameoverSequence(this, hero->get_animation_direction());
}

/**
 * Returns whether the gameover sequence is being shown.
 * @return true if the gameover sequence is being shown
 */
bool Game::is_showing_gameover(void) {
  return gameover_sequence != NULL;
}

/**
 * This function is called when the hero was dead but saved by a fairy.
 */
void Game::get_back_from_death(void) {
  hero->get_back_from_death();
}

