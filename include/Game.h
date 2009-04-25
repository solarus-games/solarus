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
#ifndef ZSDX_GAME_H
#define ZSDX_GAME_H

#include "Common.h"
#include "Screen.h"
#include "Transition.h"
#include "Controls.h"

/**
 * This class handles the game.
 * The game shows the current map and displays all current elements:
 * the current map, the HUD, the pause menu, the dialog box, etc.
 */
class Game: public Screen {

 private:

  // savegame
  Savegame *savegame;        /**< the game data saved */

  // the hero
  Hero *hero;

  // current game state (elements currently shown)
  PauseMenu *pause_menu;     /**< the current pause menu, or NULL if the game is not paused */
  DialogBox *dialog_box;     /**< the dialog box currently shown, or NULL if no message is being shown */
  Treasure *treasure;        /**< the treasure currently being given to the player or NULL if it is not the case */
  GameoverSequence *gameover_sequence; /**< the game over sequence (if currently shown) */
  bool reseting;             /**< true if the game will be reset */
  bool restarting;           /**< true if the game will be restarted */

  // controls
  Controls *controls;        /**< this object receives the SDL keyboard and joypad events */
  KeysEffect *keys_effect;   /**< current effect associated to the main game keys
			      * (represented on the HUD by the action icon, the objects icons, etc.) */

  // map
  Map *current_map;          /**< the map currently displayed */
  Map *next_map;             /**< the map where the hero is going to; if not NULL, it means that the hero 
			      * is changing from current_map to next_map */
  SDL_Surface *previous_map_surface;  /**< a copy of the previous map surface for transition effects that display two maps */

  Transition::Style transition_style; /**< the transition style between the current map and the next one */
  Transition *transition;             /**< the transition currently shown, or NULL if no transition is playing */

  // world (i.e. the current set of maps)
  Dungeon *dungeon;          /**< the dungeon of the current map, of NULL if we are not in a dungeon */
  bool crystal_switch_state; /**< indicates that a crystal switch has been enabled (i.e. the orange blocks are raised) */
  static const SDL_Rect outside_world_size; /**< size of the outside world in pixels */

  // graphics
  HUD *hud;                  /**< the game HUD (displaying hearts, rupees, key icons, etc.) */

  // music
  MusicId current_music_id;  /**< id of the music currently played (a valid music,
			      * or MUSIC_NONE if no music is being played) */
  Music *current_music;      /**< the music currently played, or NULL if no music is being played */

  // update functions
  void update_keys_effect(void);
  void update_dialog_box(void);
  void update_transitions(void);
  void update_treasure(void);
  void update_gameover_sequence(void);

  void load_dungeon(void);

 public:

  // creation and destruction
  Game(Savegame *savegame);
  ~Game(void);

  // global objects
  Hero *get_hero(void);
  const SDL_Rect get_hero_coordinates(void);
  Controls *get_controls(void);
  KeysEffect *get_keys_effect(void);
  Savegame *get_savegame(void);
  Equipment *get_equipment(void);
  DungeonEquipment *get_dungeon_equipment(void);
  MapScript *get_current_script(void);

  // functions called by the SDL loop
  void handle_event(const SDL_Event &event);
  void update(void);
  void display(SDL_Surface *screen_surface);
  
  void key_pressed(Controls::GameKey key);
  void key_released(Controls::GameKey key);

  // map
  Map *get_current_map(void);
  void set_current_map(MapId map_id, const std::string &destination_point_name,
		       Transition::Style transition_style);

  // world
  const SDL_Rect *get_outside_world_size(void);
  bool is_in_dungeon(void);
  Dungeon *get_current_dungeon(void);
  bool get_crystal_switch_state(void);
  void change_crystal_switch_state(void);

  // music
  void play_music(const MusicId &new_music_id);
  void pause_or_resume_music(void);
  void stop_music(void);
  const MusicId& get_current_music_id(void);

  // current game state
  bool is_paused(void);
  bool is_showing_message(void);
  bool is_playing_transition(void);
  bool is_showing_gameover(void);
  bool is_suspended(void); // true if at least one of the three functions above returns true
  void reset(void);
  void restart(void);

  // pause
  void set_paused(bool paused);
  PauseMenu *get_pause_menu(void);

  // dialog box
  DialogBox *get_dialog_box(void);
  void show_message(const MessageId &message_id);
  void show_message(const MessageId &message_id, int position);

  // treasure
  void give_treasure(Treasure *treasure);
  bool is_giving_treasure(void);

  // game over
  void start_gameover_sequence(void);
  void get_back_from_death(void);
};

#endif
