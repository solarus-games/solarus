/**
 * This module defines the class Music.
 * Music and Sound should be the only FMOD dependent modules.
 */

#ifndef ZSDX_MUSIC_H
#define ZSDX_MUSIC_H

#include <fmodex/fmod.h>
#include "Common.h"

/**
 * This class represents a music that can be played.
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
   * The FMOD music played.
   */
  FMOD_SOUND *stream;
  
 public:

  static const char *none;
  static const char *unchanged;

  /**
   * Name of the music file.
   */
  string file_name;

  Music(MusicId music_id);
  ~Music(void);

  // TODO: make Music inherit Sound and remove this
  static inline FMOD_SYSTEM *get_fmod_system(void) { return system; }

  static void initialize(void);
  static void exit(void);
  static bool is_initialized(void);

  static bool isNoneId(MusicId music_id);
  static bool isUnchangedId(MusicId music_id);
  static bool isEqualId(MusicId music_id, MusicId other_music_id);

  bool play(void);
  void stop(void);
  bool is_paused(void);
  void set_paused(bool pause);
};

#endif
