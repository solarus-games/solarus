/**
 * This module defines the class Game.
 */

#ifndef ZSDX_GAME_H
#define ZSDX_GAME_H

#include "Common.h"

/**
 * This class provides the game API for the maps.
 * This is the interface between the maps and the engine.
 */
class Game {

 private:

  /**
   * The game data saved.
   */
  Savegame *savegame;

  /**
   * The map to display.
   */
  Map *current_map;

  /**
   * Type of transition between the current map and the next one.
   */
  Transition transition;

  /**
   * Id of the music currently played (a valid music, or MUSIC_NONE if no music is being played).
   */
  MusicId current_music_id;

  /**
   * The music currently played (a valid music, or NULL if no music is being played).
   */
  Music *current_music;

 public:

  Game(Savegame *savegame);
  ~Game(void);

  Savegame *get_savegame(void);

  void play(void);
  void redraw_screen(Map *map);

  void set_current_map(MapId map_id);
  void set_current_map(MapId map_id, string entrance_name, Transition transition);

  /**
   * Returns the current map.
   * @return the current map
   */
  inline Map *get_current_map(void) { return current_map; }

  void play_music(MusicId new_music_id);
  void pause_or_resume_music(void);
  void stop_music(void);
};

#endif
