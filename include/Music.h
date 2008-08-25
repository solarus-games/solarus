#ifndef ZSDX_MUSIC_H
#define ZSDX_MUSIC_H

#include "Common.h"
#include "Sound.h"

/**
 * This class represents a music that can be played.
 * A music should be in format .it (Impulse Tracker Module).
 * Before using this class, the audio system should have been
 * initialized, by calling Sound::initialize().
 * Music and Sound should be the only FMOD dependent modules.
 */
class Music: public Sound {

 public:

  static const char *none;
  static const char *unchanged;

  Music(MusicId music_id);
  ~Music(void);

  static bool isNoneId(MusicId music_id);
  static bool isUnchangedId(MusicId music_id);
  static bool isEqualId(MusicId music_id, MusicId other_music_id);

  bool play(void);
  void stop(void);
  bool is_paused(void);
  void set_paused(bool pause);
};

#endif
