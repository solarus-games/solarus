/**
 * This module defines the class Music.
 */

#ifndef ZSDX_MUSIC_H
#define ZSDX_MUSIC_H

#include <fmod/fmod.h>

/**
 * This class represents a music that can be played on a map.
 * A music should be in format .it (Impulse Tracker Module).
 * This class also handles the initialization of the whole sound system.
 * The sound and the music are played with the library FMOD 3.75.
 */
class Music {

 private:

  /**
   * True if the sound system has been initialized, i.e. if
   * the static method initialize() has been called.
   */
  static bool initialized;

  /**
   * The module corresponding to this music.
   */
  FMUSIC_MODULE *module;
  
 public:

  /**
   * Name of the music file.
   */
  char file_name[256];

  /**
   * Creates a new music.
   * @param file_name name of the music file (should end with .its)
   */
  Music(const char *file_name);

  /**
   * Destroys the music.
   */
  ~Music(void);

  /**
   * Initializes the sound system.
   * This method should be called when the application starts.
   */
  static void initialize(void);

  /**
   * Closes the sound system.
   * This method should be called when exiting the application.
   */
  static void exit(void);

  /**
   * Loads the file and plays the music.
   */
  void play(void);

  /**
   * Stops playing the music.
   */
  void stop(void);

  /**
   * Returns whether the music is paused.
   * @return true if the music is paused, false otherwise
   */
  bool is_paused(void);

  /**
   * Pauses or resumes the music.
   * @param pause true to pause the music, false to resume it
   */
  void set_paused(bool pause);
};

#endif
