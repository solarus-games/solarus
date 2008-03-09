/**
 * This module defines the class Music.
 * This should be the only FMOD dependent module.
 */

#ifndef ZSDX_MUSIC_H
#define ZSDX_MUSIC_H

#include <fmodex/fmod.h>
#include "Common.h"

/**
 * This class represents a music that can be played on a map.
 * A music should be in format .it (Impulse Tracker Module).
 * This class also handles the initialization of the whole sound system.
 * The sound and the music are played with the library FMOD Ex.
 */
class Music {

 private:

  static bool initialized;
  static FMOD_SYSTEM *system;
  static FMOD_CHANNEL *channel;

  /**
   * The module corresponding to this music.
   */
  FMOD_SOUND *module;
  
 public:

  static const char *none;
  static const char *unchanged;

  /**
   * Name of the music file.
   */
  string file_name;

  Music(MusicId music_id);
  ~Music(void);

  static void initialize(void);
  static void exit(void);

  static bool isNoneId(MusicId music_id);
  static bool isUnchangedId(MusicId music_id);
  static bool isEqualId(MusicId music_id, MusicId other_music_id);

  bool play(void);
  void stop(void);
  bool is_paused(void);
  void set_paused(bool pause);
};

#endif
