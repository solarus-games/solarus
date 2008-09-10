#ifndef ZSDX_GAME_H
#define ZSDX_GAME_H

#include "Common.h"
#include "Screen.h"
#include "Transition.h"

/**
 * This class handles the game.
 * The game shows the current map and displays all current elements:
 * the current map, the HUD, the pause menu, the dialog box, etc.
 */
class Game: public Screen {

 private:

  // savegame
  Savegame *savegame;       /**< the game data saved */

  // Link
  Link *link;
  Movement8ByPlayer *link_movement;

  // current game state (elements currently shown)
  PauseMenu *pause_menu;    /**< the current pause menu, or NULL if the game is not paused */
  DialogBox *dialog_box;    /**< the dialog box currently shown, or NULL if no message is being shown */
  Treasure *treasure;       /**< the treasure currently being given to the player or NULL if it is not the case */

  // keyboard
  KeysEffect *keys_effect;  /**< current effect associated to the main game keys
			     * represented on the HUD by the action icon, the objects icons, etc.) */
  Keyboard *keyboard;       /**< this object receives the SDL keyboard events and treats them */

  // map
  Map *current_map;         /**< the map currently displayed */
  Map *next_map;            /**< the map where Link is going to; if not NULL, it means that Link is changing from
		             * current_map to next_map */

  Transition::Style transition_style; /**< the transition style between the current map and the next one */
  Transition *transition;             /**< the transition currently shown, or NULL if no transition is playing */

  // graphics
  HUD *hud;                 /**< the game HUD (displaying hearts, rupees, key icons, etc.) */

  // music
  MusicId current_music_id; /**< id of the music currently played (a valid music, or MUSIC_NONE if no music is being played) */
  Music *current_music;     /**< the music currently played, or NULL if no music is being played */

  // update functions
  void update_keys_effect(void);
  void update_dialog_box(void);
  void update_transitions(void);
  void update_treasure(void);

 public:

  // creation and destruction
  Game(Savegame *savegame);
  ~Game(void);

  // global objects
  Link *get_link(void);
  KeysEffect *get_keys_effect(void);
  Savegame *get_savegame(void);
  Equipment *get_equipment(void);
  DungeonEquipment *get_dungeon_equipment(void);

  // functions called by the SDL loop
  void handle_event(const SDL_Event &event);
  void update(void);
  void display(SDL_Surface *screen_surface);
  
  // map
  Map *get_current_map(void);
  void set_current_map(MapId map_id, unsigned int entrance_index, Transition::Style transition_style);
  void set_current_map(MapId map_id, string entrance_name, Transition::Style transition_style);

  // music
  void play_music(MusicId new_music_id);
  void pause_or_resume_music(void);
  void stop_music(void);

  // current game state
  bool is_paused(void);
  bool is_showing_message(void);
  bool is_playing_transition(void);
  bool is_suspended(void); // true if at least one of the three functions above returns true

  // pause
  void set_paused(bool paused);
  PauseMenu *get_pause_menu(void);

  // dialog box
  DialogBox *get_dialog_box(void);
  void show_message(MessageId message_id);

  // treasure
  void give_treasure(Treasure *treasure);
  bool is_giving_treasure(void);
};

#endif
