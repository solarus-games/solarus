#ifndef ZSDX_MUSIC_H
#define ZSDX_MUSIC_H

#include <fmod/fmod.h>

/**
 * This class represents a music that can be played on a map.
 * A music should be in format .it (Impulse Tracker Module).
 * This class also handles the initialization of the whole sound system.
 * The sound and the music are played with the library FMOD.
 */
class Music {

 private:
  /**
   * True if the sound system has been initialized, i.e. if
   * the method initialize() has been called.
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

  Music(const char *file_name);
  ~Music(void);

  static void initialize(void);
  static void exit(void);

  void play(void);
  void stop(void);
  bool is_paused(void);
  void set_paused(bool pause);
};

#endif
