#include "Game.h"
#include "ZSDX.h"
#include "Music.h"
#include "Map.h"
#include "ResourceManager.h"
#include "Savegame.h"
#include "Color.h"
#include "HUD.h"
#include "Sound.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "Keyboard.h"
#include "entities/Link.h"
#include "entities/AnimatedTile.h"
#include "entities/Tileset.h"
#include "entities/EntityDetector.h"
#include "movements/Movement8ByPlayer.h"

/**
 * Creates a game.
 * @param savegame the saved data of this game
 */
Game::Game(Savegame *savegame):
  savegame(savegame),
  paused(false), dialog_box(NULL), treasure(NULL), keys_effect(NULL),
  current_map(NULL), next_map(NULL),
  transition_style(Transition::IMMEDIATE), transition(NULL), hud(NULL),
  current_music_id(Music::none), current_music(NULL) {

  zsdx->set_game(this);

  // initialize Link
  link = new Link(savegame->get_equipment());
  link_movement = link->get_movement();

  // initialize the keys effect and the HUD
  keys_effect = new KeysEffect();
  update_keys_effect();
  keyboard = new Keyboard(this);
  hud = new HUD(this);

  // launch the starting map
  set_current_map(savegame->get_integer(Savegame::STARTING_MAP),
		  savegame->get_integer(Savegame::STARTING_ENTRANCE),
		  Transition::FADE);
}

/**
 * Destroys the game.
 */
Game::~Game(void) {

  // quit the game
  current_map->leave(); // tell the map that Link is not there anymore
  stop_music();

  if (transition != NULL) {
    delete transition;
  }

  if (dialog_box != NULL) {
    delete dialog_box;
  }

  if (treasure != NULL) {
    delete treasure;
  }

  delete keys_effect;
  delete keyboard;
  delete hud;
  delete link;
  delete savegame;

  zsdx->set_game(NULL);
}

/**
 * Returns the saved data associated to this game.
 * @return the saved data
 */
Savegame * Game::get_savegame(void) {
  return savegame;
}

/**
 * Returns Link.
 * @return Link
 */
Link * Game::get_link(void) {
  return link;
}

/**
 * This function is called by the SDL main loop
 * when an SDL event occurs during the game.
 */
void Game::handle_event(const SDL_Event &event) {

  switch (event.type) {

    // a key is pressed
  case SDL_KEYDOWN:
    keyboard->key_pressed(event.key.keysym);
    break;

  case SDL_KEYUP:
    keyboard->key_released(event.key.keysym);
    break;
  }
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
      link->set_animation_stopped();
      transition = Transition::create(transition_style, Transition::OUT);
      transition->start();
    }
  }

  // if a transition was playing and has just been finished
  if (transition != NULL && transition->is_over()) {

    if (transition->get_direction() == Transition::OUT) {
      // change the map
      current_map->leave();

      // unload the previous tileset if the new map uses another one
      Tileset *old_tileset = current_map->get_tileset();
      Tileset *new_tileset = next_map->get_tileset();
      if (new_tileset != old_tileset) {
	old_tileset->unload();
      }

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
    transition = Transition::create(transition_style, Transition::IN);
    transition->start();
  }
}

/**
 * Updates the game elements : the map, the equipment, the HUD, the sound system, etc.
 */
void Game::update(void) {

  // update the transitions between maps
  update_transitions();

  // update the entity's positions and animations
  AnimatedTile::update();
  current_map->update();

  // update the equipment and HUD
  savegame->get_equipment()->update();
  update_keys_effect();
  hud->update();

  // update the treasure (if any)
  if (treasure != NULL) {
    update_treasure();
  }

  // update the dialog box (if any)
  if (is_showing_message()) {
    update_dialog_box();
  }

  // update the sound system
  Sound::update();
}

/**
 * Makes sure the keys effects are coherent with Link's equipment and abilities.
 */
void Game::update_keys_effect(void) {

  switch (link->get_state()) {
    
  case Link::FREE:
  case Link::SWORD_SWINGING:
  case Link::SWORD_LOADING:

    // the sword key swings the sword <=> Link has a sword
    if (savegame->get_equipment()->has_sword()
	&& keys_effect->get_sword_key_effect() != KeysEffect::SWORD_KEY_SWORD) {

      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SWORD);
    }
    else if (!savegame->get_equipment()->has_sword()
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

  if (!dialog_box->is_over()) {
    dialog_box->update();
  }
  else {
    delete dialog_box;
    dialog_box = NULL;
  }
}

/**
 * Updates the treasure.
 * This function is called repeatedly while a treasure is being given.
 */
void Game::update_treasure(void) {
  if (treasure != NULL && !is_showing_message()) {
    delete treasure;
    treasure = NULL;
  }
}

/**
 * Gives a treasure to Link.
 * Makes him brandish the treasure and shows a message.
 * @param treasure the treasure to give him (will be deleted it after Link brandishes it) 
 */
void Game::give_treasure(Treasure *treasure) {

  this->treasure = treasure;

  // brandish the treasure
  link->give_treasure(treasure);

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
 * Displays the game.
 * @param screen_surface the surface where the game will be displayed
 */
void Game::display(SDL_Surface *screen_surface) {

  // display the map
  current_map->display();
  if (transition != NULL) {
    transition->display(current_map->get_visible_surface());
  }
  SDL_BlitSurface(current_map->get_visible_surface(), NULL, screen_surface, NULL);

  // display the pause screen if any
  // TODO

  // display the hud
  hud->display(screen_surface);

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
 * Call this function when you want Link to go to another map.
 * @param map_id id of the map to launch
 * @param entrance_index index of the entrance of the map you want to use
 * @param transition_style type of transition between the two maps
 */
void Game::set_current_map(MapId map_id, unsigned int entrance_index, Transition::Style transition_style) {

  next_map = ResourceManager::get_map(map_id);

  if (!next_map->is_loaded()) {
    next_map->load();
  }

  next_map->set_entrance(entrance_index);
  this->transition_style = transition_style;
}

/**
 * Changes the current map.
 * Call this function when you want Link to go to another map.
 * @param map_id id of the map to launch
 * @param entrance_name name of the entrance of the map you want to use
 * @param transition_style type of transition between the two maps
 */
void Game::set_current_map(MapId map_id, string entrance_name, Transition::Style transition_style) {

  next_map = ResourceManager::get_map(map_id);

  if (!next_map->is_loaded()) {
    next_map->load();
  }

  next_map->set_entrance(entrance_name);
  this->transition_style = transition_style;
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
  return current_map == NULL || is_paused() || is_showing_message() || is_playing_transition();
}

/**
 * Returns the current effect of the main keys (action, sword, pause, etc.).
 * @return the current effect of the main keys
 */
KeysEffect * Game::get_keys_effect(void) {
  return keys_effect;
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
 */
void Game::show_message(MessageId message_id) {

  SDL_Rect *screen_position = current_map->get_screen_position();
  int y;
  if (link->get_y() < screen_position->y + 130) {
    y = 144;
  }
  else {
    y = 32;
  }

  dialog_box = new DialogBox(message_id, 51, y);
}

/**
 * Pauses or resumes the game.
 * @param pause true to pause the game, false to resume it.
 */
void Game::set_paused(bool paused) {
  this->paused = paused;
}
