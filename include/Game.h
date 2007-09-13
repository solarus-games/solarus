/**
 * This module defines the class Game.
 */

#ifndef ZSDX_GAME_H
#define ZSDX_GAME_H

#include "GameResource.h"
#include "MapInitialState.h"

/**
 * This class provides the game API for the maps.
 * This is the interface between the maps and the engine.
 */
class Game {

 private:

  /**
   * The map to display.
   */
  Map *current_map;

  /**
   * ID of the music currently played (a valid music, or MUSIC_NONE if no music is being played).
   */
  MusicID current_music_id;

  /**
   * The music currently played (a valid music, or NULL if no music is being played).
   */
  Music *current_music;

  /**
   * Launches the new map.
   */
  void launch_map(void);

 public:

  /**
   * Constructor.
   */
  Game(void);

  /**
   * Destructor.
   */
  inline ~Game(void) { }
  
  /**
   * Lets the user play.
   * The SDL main loop is executed here.
   */
  void play(void);

  /**
   * Changes the current map.
   * Call this function when you want Link to go to another map.
   * The map will be loaded with its first initial state.
   * @param map_id id of the map to launch
   */
  void set_current_map(MapID map_id);

  /**
   * Changes the current map.
   * Call this function when you want Link to go to another map.
   * @param map_id id of the map to launch
   * @param initial_state_index index of the initial state of the map
   */
  void set_current_map(MapID map_id, unsigned int initial_state_index);

  /**
   * Returns the music currently played.
   * @return the current music, or NULL is no music is being played
   */
  //Music *get_current_music(void);
  
  /**
   * Plays a music. If the music is different from the current one,
   * the current one is stopped.
   * The music specified can also be MUSIC_NONE (then the current music is just stopped)
   * or even MUSIC_NO_CHANGE (nothing is done in this case).
   * @param music_id id of the music to play
   */
  void play_music(MusicID new_music_id);
  
  /**
   * Pauses or resumes the current music.
   * If no music is being played, nothing is done.
   */
  void pause_or_resume_music(void);

  /**
   * Stops playing the current music.
   * If no music is being played, nothing is done.
   */
  void stop_music(void);
};

#endif
